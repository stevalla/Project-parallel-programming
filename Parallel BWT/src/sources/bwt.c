#include "../headers/bwt.h"

unsigned char *bwtTransformation(unsigned char *const in,
								 const size_t inputLen)
{
	const size_t inLen = inputLen + 1;
	int phases = 0;
	short *input;

	//Append the sentinel
	input = initText(in, inLen);

	Node *suffixTree = buildSuffixTree(input, &phases, inLen);

	return getBWT(input, suffixTree, inLen);
}

unsigned char *getBWT(short *const input,
				 	  Node *const root,
					  const size_t inputLen)
{
	short *suffixArray;
	int i, index;
	unsigned char *result;

	i = 0;
	suffixArray = (short *) malloc(sizeof(short)*(inputLen));
	result = (unsigned char *) malloc(sizeof(unsigned char)*(inputLen + 8));

	createSuffixArray(root, &i, suffixArray, input);

	for(i=0; i<inputLen; i++) {
		index = suffixArray[i];

		if(index == 0) {			//Index of the sentinel character

			result[i + 8] = 0;		//Dummy character
			encodeIndex(i, result, 4);

		} else if(index == 1) {		//Index of first character

			result[i + 8] = input[0];
			encodeIndex(i, result, 0);

		} else
			result[i + 8] = input[index - 1];
	}

	free(suffixArray);

	return result;
}

//Lexicographic DFS traversal to create the suffix array
void createSuffixArray(Node *const node,
					   int *const i,
					   short *const suffixArray,
					   short *const input)
{
	HashChildren *child;
	int num_children = HASH_COUNT(node->children);

	if(num_children == 0) {
		suffixArray[*i] = node->suffixIndex;
		(*i)++;
		deleteNode(node);
		return;
	}

	HASH_SORT(node->children, sortNodesByFirstChar);

	for(child=node->children; child!=NULL; child=child->hh.next)
		createSuffixArray(child->node, i, suffixArray, input);

	deleteNode(node);
}

int sortNodesByFirstChar(HashChildren *const el1, HashChildren *const el2)
{
	if(el1->firstChar < el2->firstChar)			return -1;
	else if(el1->firstChar > el2->firstChar)	return  1;
	else 										return  0;
}

//Add a sentinel at the end of the text for the transformation
short *initText(unsigned char *const text, const size_t len)
{
	short *input = (short *) malloc(sizeof(short)*(len));
	int i;

	for(i=0; i<len-1; i++)
		input[i] = text[i];

	input[i] = SENTINEL;

	return input;
}


unsigned char decomposeUnsigned(unsigned ul, int n) {
    return (ul >> (24 - 8 * n)) & 0xFF;
}

void encodeIndex(const unsigned index, unsigned char *const output, int j)
{
	for(unsigned i=0; i<4; i++) {
		output[j++] = decomposeUnsigned(index, i);
	}
}
