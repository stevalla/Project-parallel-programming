#include <assert.h>

#include "../headers/arith.h"

#define STATIC_ASSERT(name, cond) \
	typedef char assert_##name[(cond)? 1: -1]

#define BITS(n) (8 * sizeof(n))

STATIC_ASSERT(freq_bits_fits_in_type,
		FREQUENCY_BITS <= BITS(unsigned));
STATIC_ASSERT(range_bits_fits_in_type,
		BITS_RANGE <= BITS(unsigned));
STATIC_ASSERT(range_times_freq_not_overflow,
		BITS_RANGE + FREQUENCY_BITS <= BITS(unsigned));
STATIC_ASSERT(range_one_fourth_contains_freqs,
		BITS_RANGE >= FREQUENCY_BITS + 2);

static const unsigned         RANGE_BITS = BITS_RANGE;
static const unsigned          RANGE_MIN = 0;
static const unsigned          RANGE_MAX = MAX_RANGE;
static const unsigned         RANGE_HALF = MAX_RANGE / 2;
static const unsigned   RANGE_ONE_FOURTH = MAX_RANGE / 4;
static const unsigned RANGE_THREE_FOURTH = (MAX_RANGE * 3) / 4;

static const unsigned EOS = 1 << (8 * sizeof(unsigned char));
static const unsigned CONTINUATION_BIT = 0;

unsigned char *encodingRoutine(unsigned char *const input,
							   const size_t inputLen)
{
	Encoder * en;
	Interval currentInt;
	IOBuffer * out;
	Model * model;
	IOHelper *o;

	en = (Encoder *) malloc(sizeof(Encoder));
	out = (IOBuffer *) malloc(sizeof(IOBuffer));
	model = (Model *) malloc(sizeof(Model));
	o = (IOHelper *) malloc(sizeof(IOHelper));
	o->text = (unsigned char *) malloc(sizeof(unsigned char)*inputLen);
	o->index = 0;

	initEncoder(en);
	initModel(model, EOS + 1);
	initIOBuffer(out);

	for(unsigned i=0; i<inputLen; i++) {

		findInterval(&currentInt, model, input[i]);

		encodeSymbol(&en->low, &en->high, en->range, currentInt);

		checkForOutputBit(en, out, o);

	}

	findInterval(&currentInt, model, EOS);
	encodeSymbol(&en->low, &en->high, en->range, currentInt);
	checkForOutputBit(en, out, o);

	finishEncoding(en, out, o);

	unsigned char *output = (unsigned char *)
							 malloc(sizeof(unsigned char) * o->index);

	for(unsigned i=0; i<o->index; i++)
		output[i] = o->text[i];

	free(o->text);
	free(o);
	free(en);
	free(out);
	free(model->freq);
	free(model);

	return output;
}


void encodeSymbol(unsigned *const low,
				  unsigned *const high,
				  const unsigned range,
				  const Interval interval)
{

	assert(*high <= MAX_RANGE);
	assert(*low < *high);
	assert(range == *high - *low);
	assert(interval.lowCount < interval.highCount);
	assert(interval.highCount <= interval.scale);
	assert(interval.scale <= MAX_FREQUENCY);
	assert(range >= interval.scale);

	*high = *low + ((range * interval.highCount) / interval.scale);
	*low  = *low + ((range * interval.lowCount)  / interval.scale);
}


void initModel(Model *const model, const unsigned size)
{
	assert(MAX_FREQUENCY >= size);
	model->size = size + 1;
	model->freq = (unsigned short *)
				  malloc(sizeof(model->freq[0]) * model->size);

	for(unsigned i=0; i<model->size; i++)
		model->freq[i] = i;
}


void initEncoder(Encoder *const en)
{

	en->low = RANGE_MIN;
	en->high = RANGE_MAX;
	en->range = RANGE_MAX - RANGE_MIN;
	en->underflow = 0;

}

void initIOBuffer(IOBuffer *const buf)
{
	buf->buf = 0;
	buf->bufBits = 0;

}

void findInterval(Interval *const interval,
				  Model *const model,
				  const unsigned ch)
{
	interval->lowCount = model->freq[ch];
	interval->highCount = model->freq[ch + 1];
	interval->scale = model->freq[model->size - 1];

	updateModel(model, ch);
}


void checkForOutputBit(Encoder *const en,
					   IOBuffer *const out,
					   IOHelper *const o)
{

	for(;;) {

		if(en->high <= RANGE_HALF) {

			outputBits(en, 0, out, o);
			en->low  = 2 * en->low;
			en->high = 2 * en->high;

		} else if(en->low >= RANGE_HALF) {

			outputBits(en, 1, out, o);
			en->low  = 2 * (en->low  - RANGE_HALF);
			en->high = 2 * (en->high - RANGE_HALF);

		} else if(en->low >= RANGE_ONE_FOURTH &&
				  en->high <= RANGE_THREE_FOURTH) {

			en->underflow++;
			en->low  = 2 * (en->low  - RANGE_ONE_FOURTH);
			en->high = 2 * (en->high - RANGE_ONE_FOURTH);

		} else
			break;
	}

	en->range = en->high - en->low;
}


void updateModel(Model *const model, const unsigned ch)
{
	assert(ch >= 0 && ch < model->size - 1);

	for(unsigned i=ch+1; i<model->size; i++)
		++model->freq[i];

	if(MAX_FREQUENCY > model->freq[model->size - 1])
		return;

	unsigned base = 0;
	for(unsigned i=1; i<model->size; i++) {

		unsigned d = model->freq[i] - base;

		if(d > 1)
			d /= 2;
		base = model->freq[i];
		model->freq[i] = model->freq[i - 1] + d;
	}
}

void outputBit(IOBuffer *const out,
			   const unsigned bit,
			   IOHelper *const o)
{
	assert(bit == 0 || bit == 1);

	out->buf |= bit << (BUF_BITS - ++out->bufBits);
	if(BUF_BITS == out->bufBits) {

		(void) BUF_BITS;
		o->text[o->index] = out->buf;
		o->index++;
		out->buf = 0;
		out->bufBits = 0;
	}

}

void outputBits(Encoder *const en,
		 	 	const unsigned bit,
				IOBuffer *const out,
				IOHelper *const o)
{

	assert(bit == 0 || bit == 1);
	outputBit(out, bit, o);

	for(const unsigned inv = !bit; en->underflow > 0; --en->underflow)
		outputBit(out, inv, o);

}


void finishEncoding(Encoder *const en,
					IOBuffer *const out,
					IOHelper *const o)
{
	if(RANGE_MIN == en->low && en->underflow == 0) {
		assert(RANGE_HALF < en->high);
		if(RANGE_MAX != en->high)
			outputBits(en, 0, out, o);

	} else if (RANGE_MAX == en->high && en->underflow == 0) {
		assert(RANGE_HALF > en->low);
		if (en->low != 0)
			outputBits(en, 1, out, o);

	} else {
		if (en->low != RANGE_MIN && en->high != RANGE_MAX)
			++en->underflow;
		outputBits(en, RANGE_ONE_FOURTH <= en->low, out, o);
	}

	if (out->bufBits != 0) {
		(void) BUF_BITS;
		o->text[o->index] = out->buf;
		o->index++;
	}

}

unsigned char *decodingRoutine(unsigned char *const input)
{
	Decoder *de;
	Model *model;
	IOBuffer *inBuf;
	Interval currentInt;
	IOHelper *in, *o;
	unsigned char *output;
	unsigned ch;

	de = (Decoder *) malloc(sizeof(Decoder));
	model = (Model *) malloc(sizeof(Model));
	inBuf = (IOBuffer *) malloc(sizeof(IOBuffer));
	in = (IOHelper *) malloc(sizeof(IOHelper));
	in->text = input;
	in->index = 0;
	o = (IOHelper *) malloc(sizeof(IOHelper));
	o->index = 0;
	o->text = (unsigned char *)
			   malloc(sizeof(unsigned char) * MAX_CHUNK_SIZE);

	initDecoder(de);
	initModel(model, EOS + 1);
	initIOBuffer(inBuf);

	while((ch = decodeSymbol(de, model, &currentInt, inBuf, in)) != EOS) {
		o->text[o->index] = (unsigned char) ch;
		o->index++;
	}

	output = (unsigned char *) malloc(sizeof(unsigned char) * o->index);
	for(unsigned i=0; i<o->index; i++)
		output[i] = o->text[i];

	free(o->text);
	free(o);
	free(in);
	free(de);
	free(model->freq);
	free(model);
	free(inBuf);

	return output;
}

void updateInterval(Decoder *const de,
					IOBuffer *const inBuf,
					IOHelper *const in)
{
	for(;;) {

		if(de->high <= RANGE_HALF) {
			de->high = 2 * de->high;
			de->low  = 2 * de->low;

		} else if(de->low >= RANGE_HALF) {

			de->high = (de->high - RANGE_HALF) * 2;
			de->low  = (de->low  - RANGE_HALF) * 2;
			de->value -= RANGE_HALF;

		} else if(de->low >= RANGE_ONE_FOURTH &&
				  de->high <= RANGE_THREE_FOURTH) {

			de->high = (de->high - RANGE_ONE_FOURTH) * 2;
			de->low  = (de->low  - RANGE_ONE_FOURTH) * 2;
			de->value -= RANGE_ONE_FOURTH;

		} else
			break;

		de->value = de->value << 1 | inputBit(de, inBuf, in);
		assert(de->value >= 0 && de->value < RANGE_MAX);
	}

	de->range = de->high - de->low;
}

unsigned decodeSymbol(Decoder *const de,
					  Model *const model,
					  Interval *const interval,
					  IOBuffer *const inBuf,
					  IOHelper *const in)
{
	if(de->valueBits == 0) {

		for(unsigned i = RANGE_BITS; i > 0; i--)
			de->value = de->value << 1 | inputBit(de, inBuf, in);

		de->valueBits = RANGE_BITS;
	}

	assert(de->low <= de->value);
	assert(de->high > de->value);

	const unsigned ch = findChar(de, interval, model);

	encodeSymbol(&de->low, &de->high, de->range, *interval);
	updateInterval(de, inBuf, in);

	return ch;
}

unsigned inputBit(Decoder *const de,
				  IOBuffer *const inBuf,
				  IOHelper *const in)
{
	if(de->fin)
		return CONTINUATION_BIT;

	if(inBuf->bufBits == 0) {

		inBuf->buf = in->text[in->index];

		if((in->text + in->index) != NULL)
			inBuf->bufBits = 8;
		else
			inBuf->bufBits = 0;

		in->index++;

		if(inBuf->bufBits == 0) {
			de->fin = !de->fin;
			return CONTINUATION_BIT;
		}
	}

	const unsigned bit = inBuf->buf >> (BUF_BITS - 1);
	assert(bit == 0 || bit == 1);
	inBuf->buf <<= 1;
	--inBuf->bufBits;

	return bit;
}

void initDecoder(Decoder *const de)
{
	de->value = 0;
	de->valueBits = 0;
	de->fin = 0;
	de->low = RANGE_MIN;
	de->high= RANGE_MAX;
	de->range = RANGE_MAX - RANGE_MIN;
}

unsigned findChar(Decoder *const de,
				  Interval *const interval,
				  Model *const model)
{
	const unsigned v = de->value - de->low;
	const unsigned last = model->size - 1;
	const unsigned freq = scaleFrequencies(v, de->range, model->freq[last]);

	for(unsigned i=0; i<last; i++) {

		if(model->freq[i] <= freq && model->freq[i + 1] > freq) {

			interval->lowCount = model->freq[i];
			interval->highCount = model->freq[i + 1];
			interval->scale = model->freq[last];

			updateModel(model, i);
			return i;
		}
	}

	assert(!"Bad range");
	return -1;
}



