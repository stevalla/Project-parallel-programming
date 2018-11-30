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

static const unsigned RANGE_BITS = BITS_RANGE;
static const unsigned RANGE_MIN	= 0;
static const unsigned RANGE_MAX = MAX_RANGE;
static const unsigned RANGE_HALF = MAX_RANGE / 2;
static const unsigned RANGE_ONE_FOURTH = MAX_RANGE / 4;
static const unsigned RANGE_THREE_FOURTH = (MAX_RANGE * 3) / 4;

static const unsigned EOS = 1 << (8 * sizeof(unsigned char));
static const unsigned CONTINUATION_BIT = 0;

void encodingRoutine(FILE *const input)
{
	Encoder * en;
	Interval currentInt;
	IOBuffer * out;
	Model * model;
	FILE * output;
	unsigned char sym;

	en = (Encoder *) malloc(sizeof(Encoder));
	out = (IOBuffer *) malloc(sizeof(IOBuffer));
	model = (Model *) malloc(sizeof(Model));
	output = fopen("encoded.zip", "wb");
	if(output == NULL)
		printf("Could not open output file");


	initEncoder(en);
	initModel(model, EOS + 1);
	initIOBuffer(out);

	while(fread(&sym, sizeof(sym), 1, input) > 0) {

		findInterval(&currentInt, model, sym);

		encodeSymbol(&en->low, &en->high, en->range, currentInt);

		checkForOutputBit(en, out, output);

	}

	findInterval(&currentInt, model, EOS);
	encodeSymbol(&en->low, &en->high, en->range, currentInt);
	checkForOutputBit(en, out, output);

	finishEncoding(en, out, output);

	free(en);
	free(out);
	free(model->freq);
	free(model);

	fclose(output);
	fclose(input);
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

void initIOBuffer(IOBuffer *const out)
{
	out->buf = 0;
	out->bufBits = 0;

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


void checkForOutputBit(Encoder *const en, IOBuffer *const out, FILE *const f)
{

	for(;;) {

		if(en->high <= RANGE_HALF) {

			outputBits(en, 0, out, f);
			en->low  = 2 * en->low;
			en->high = 2 * en->high;

		} else if(en->low >= RANGE_HALF) {

			outputBits(en, 1, out, f);
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
			   FILE *const f)
{
	assert(bit == 0 || bit == 1);

	out->buf |= bit << (BUF_BITS - ++out->bufBits);
	if(BUF_BITS == out->bufBits) {

		(void) BUF_BITS;
		fwrite(&out->buf, sizeof(out->buf), 1, f);
		out->buf = 0;
		out->bufBits = 0;
	}

}

void outputBits(Encoder *const en,
		 	 	const unsigned bit,
				IOBuffer *const out,
				FILE *const f)
{

	assert(bit == 0 || bit == 1);
	outputBit(out, bit, f);

	for(const unsigned inv = !bit; en->underflow > 0; --en->underflow)
		outputBit(out, inv, f);

}


void finishEncoding(Encoder *const en, IOBuffer *const out, FILE *const f)
{
	if(RANGE_MIN == en->low && en->underflow == 0) {
		assert(RANGE_HALF < en->high);
		if(RANGE_MAX != en->high)
			outputBits(en, 0, out, f);

	} else if (RANGE_MAX == en->high && en->underflow == 0) {
		assert(RANGE_HALF > en->low);
		if (en->low != 0)
			outputBits(en, 1, out, f);

	} else {
		if (en->low != RANGE_MIN && en->high != RANGE_MAX)
			++en->underflow;
		outputBits(en, RANGE_ONE_FOURTH <= en->low, out, f);
	}

	if (out->bufBits != 0) {
		(void) BUF_BITS;
		fwrite(&out->buf, sizeof(out->buf), 1, f);
	}

}

void decodingRoutine(FILE *const input)
{
	FILE * out;
	Decoder * de;
	Model * model;
	IOBuffer * in;
	Interval currentInt;
	unsigned ch;


	de = (Decoder *) malloc(sizeof(Decoder));
	model = (Model *) malloc(sizeof(Model));
	in = (IOBuffer *) malloc(sizeof(IOBuffer));
	out = fopen("decoded.txt", "wb");

	initDecoder(de);
	initModel(model, EOS + 1);
	initIOBuffer(in);

	while((ch = decodeSymbol(de, model, &currentInt, in, input)) != EOS) {
		const unsigned char sym = (unsigned char) ch;
		fwrite(&sym, sizeof(sym), 1, out);
	}


	free(de);
	free(model->freq);
	free(model);
	free(in);

	fclose(input);
	fclose(out);

}

void updateInterval(Decoder *const de, IOBuffer *const in, FILE *const f)
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

		de->value = de->value << 1 | inputBit(de, in, f);
		assert(de->value >= 0 && de->value < RANGE_MAX);
	}

	de->range = de->high - de->low;
}

unsigned decodeSymbol(Decoder *const de,
					  Model *const model,
					  Interval *const interval,
					  IOBuffer *const in,
					  FILE *const input)
{
	if(de->valueBits == 0) {

		for(unsigned i = RANGE_BITS; i > 0; i--)
			de->value = de->value << 1 | inputBit(de, in, input);

		de->valueBits = RANGE_BITS;
	}

	assert(de->low <= de->value);
	assert(de->high > de->value);

	const unsigned ch = findChar(de, interval, model);

	encodeSymbol(&de->low, &de->high, de->range, *interval);
	updateInterval(de, in, input);

	return ch;
}

unsigned inputBit(Decoder *const de, IOBuffer *const in, FILE *const f)
{
	if(de->fin)
		return CONTINUATION_BIT;

	if(in->bufBits == 0) {

		in->bufBits = (8 * fread(&in->buf, sizeof(in->buf), 1, f));
		if(in->bufBits == 0) {
			de->fin = !de->fin;
			return CONTINUATION_BIT;
		}
	}

	const unsigned bit = in->buf >> (BUF_BITS - 1);
	assert(bit == 0 || bit == 1);
	in->buf <<= 1;
	--in->bufBits;

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



