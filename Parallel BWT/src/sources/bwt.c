#include "../headers/bwt.h"

Text *bwtTransformation(Text *const in)
{
	const size_t bwtLen = in->len + 1;
	int phases = 0;
	short *input;

	//Append the sentinel
	input = initText(in->text, bwtLen);

	Node *suffixTree = buildSuffixTree(input, &phases, bwtLen);

	return getBWT(input, suffixTree, bwtLen);
}

Text *getBWT(short *const input,
			 Node *const root,
			 const size_t inputLen)
{
	short *suffixArray;
	int i, index;
	Text *output;

	i = 0;
	suffixArray = (short *) malloc(sizeof(short)*(inputLen));
	output = (Text *) malloc(sizeof(Text));
	output->len = inputLen + 8;
	output->text = (unsigned char *)
			malloc(sizeof(unsigned char)*(output->len));

	createSuffixArray(root, &i, suffixArray, input);

	for(i=0; i<inputLen; i++) {
		index = suffixArray[i];

		if(index == 0) {			//Index of the sentinel character

			output->text[i + 8] = 0;		//Dummy character
			encodeIndex(i, output->text, 4);

		} else if(index == 1) {		//Index of first character

			output->text[i + 8] = input[0];
			encodeIndex(i, output->text, 0);

		} else
			output->text[i + 8] = input[index - 1];
	}

	free(suffixArray);

	return output;
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
