#include "../headers/bwt.h"

ResultBwt *bwtTransformation(short *input, size_t inputLen)
{
	int phases = 0;
	Node *suffixTree = buildSuffixTree(input, &phases, inputLen);

	return getBWT(input, suffixTree, inputLen);
}

ResultBwt *getBWT(short *input, Node *root, size_t inputLen)
{
	int *suffixArray = (int *) malloc(sizeof(int)*(inputLen+1));
	int i;
	ResultBwt *result;

	i = 0;
	result = (ResultBwt *) malloc(sizeof(ResultBwt));
	result->text = (short *) malloc(sizeof(short)*(inputLen+2));

	createSuffixArray(root, &i, suffixArray, input);

	for(i=0; i<=inputLen; i++) {
		int index = suffixArray[i];

		//Store the BWT text
		if(index == 0)
			result->text[i] = input[inputLen];
		else
			result->text[i] = input[suffixArray[i] - 1];

		//Store the row of the original string
		if(input[inputLen] == result->text[i])
			result->index = i;
	}

	free(suffixArray);

	return result;
}

//Lexicographic DFS traversal to create the suffix array
void createSuffixArray(Node *node, int *i, int suffixArray[], short *inputText)
{
	HashChildren *hashChild;
	int num_children = HASH_COUNT(node->children);

	if(num_children == 0) {
		suffixArray[*i] = node->suffixIndex;
		(*i)++;
		deleteNode(node);
		return;
	}

	HASH_SORT(node->children, sortNodesByFirstChar);

	for(hashChild=node->children; hashChild!=NULL; hashChild=hashChild->hh.next)
		createSuffixArray(hashChild->node, i, suffixArray, inputText);

	deleteNode(node);
}

int sortNodesByFirstChar(HashChildren *el1, HashChildren *el2)
{
	if(el1->firstChar < el2->firstChar)			return -1;
	else if(el1->firstChar > el2->firstChar)	return  1;
	else 										return  0;
}
