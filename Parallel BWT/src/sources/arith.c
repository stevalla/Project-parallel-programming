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

static const unsigned          RANGE_MIN = 0;
static const unsigned          RANGE_MAX = MAX_RANGE;
static const unsigned         RANGE_HALF = MAX_RANGE / 2;
static const unsigned   RANGE_ONE_FOURTH = MAX_RANGE / 4;
static const unsigned RANGE_THREE_FOURTH = (MAX_RANGE * 3) / 4;

static const unsigned EOS = 1 << (8 * sizeof(unsigned char));

Text encodingRoutine(const Text input)
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
	o->text = (unsigned char *) malloc(sizeof(unsigned char)*input.len*2);
	o->index = 0;

	initEncoder(en);
	initModel(model, EOS + 1);
	initIOBuffer(out);

	for(unsigned i=0; i<input.len; i++) {

		findInterval(&currentInt, model, input.text[i]);

		encodeSymbol(&en->low, &en->high, en->range, currentInt);

		checkForOutputBit(en, out, o);

	}

	findInterval(&currentInt, model, EOS);
	encodeSymbol(&en->low, &en->high, en->range, currentInt);
	checkForOutputBit(en, out, o);

	finishEncoding(en, out, o);

	Text output;
	output.text = (unsigned char *) malloc(sizeof(unsigned char)*o->index);
	output.len = o->index;

	for(unsigned i=0; i<o->index; i++)
		output.text[i] = o->text[i];

	free(o->text);
	free(o);
	free(en);
	free(out);
	free(model->freq);
	free(model);
	free(input.text);

	output.id = input.id;

	return output;
}


void encodeSymbol(unsigned *const low,
				  unsigned *const high,
				  const unsigned range,
				  const Interval interval)
{

	assert(*high <= MAX_RANGE);
	assert(*low < *high);
	assert(range == *high - *low + 1);
	assert(interval.lowCount < interval.highCount);
	assert(interval.highCount <= interval.scale);
	assert(interval.scale <= MAX_FREQUENCY);
	assert(range >= interval.scale);

	*high = *low + range * interval.highCount / interval.scale;
	*low  = *low + range * interval.lowCount  / interval.scale;
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
	en->range = RANGE_MAX - RANGE_MIN + 1;
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

	en->range = en->high - en->low + 1;
}


void updateModel(Model *const model, const unsigned ch)
{
	assert(ch >= 0 && ch < model->size - 1);

	for(unsigned i=ch + 1; i<model->size; i++)
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
		o->text[o->index++] = out->buf;
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
		(void) out->bufBits;
		o->text[o->index++] = out->buf;
	}

}




