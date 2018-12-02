#include "../headers/unarith.h"

static const unsigned          RANGE_MIN = 0;
static const unsigned          RANGE_MAX = MAX_RANGE;
static const unsigned         RANGE_BITS = BITS_RANGE;
static const unsigned         RANGE_HALF = MAX_RANGE / 2;
static const unsigned   RANGE_ONE_FOURTH = MAX_RANGE / 4;
static const unsigned RANGE_THREE_FOURTH = (MAX_RANGE * 3) / 4;

static const unsigned CONTINUATION_BIT = 0;
static const unsigned EOS = 1 << (8 * sizeof(unsigned char));

Text *decodingRoutine(unsigned char *const input)
{
	Decoder *de;
	Model *model;
	IOBuffer *inBuf;
	Interval currentInt;
	IOHelper *in, *o;
	Text *result;
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

	result = (Text *) malloc(sizeof(Text));
	result->len = o->index;
	result->text = (unsigned char *)
			malloc(sizeof(unsigned char) * o->index);

	for(unsigned i=0; i<o->index; i++)
		result->text[i] = o->text[i];

	free(o->text);
	free(o);
	free(in);
	free(de);
	free(model->freq);
	free(model);
	free(inBuf);

	return result;
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
