#include "bwt.h"

ResultBwt *bwtTransformation(String text)
{

	int phases = 0;
	Node *suffixTree = buildSuffixTree(text, &phases);

	return getBWT(text, suffixTree);
}

ResultBwt *getBWT(String text, Node *root)
{
	int len = strlen(text);
	int suffixArray[len];
	int i;
	ResultBwt *result;

	for(i=0; i<len; i++)
		suffixArray[i] = -1;

	i = 0;
	result = (ResultBwt *) malloc(sizeof(ResultBwt));
	result->text = (String) malloc(sizeof(text));

	createSuffixArray(root, &i, suffixArray, text);

	for(i=0; i<len; i++) {
		int index = suffixArray[i];

		//Store the BWT text
		if(index == 0)
			result->text[i] = text[strlen(text) - 1];
		else
			result->text[i] = text[suffixArray[i] - 1];

		//Store the row of the original string
		if(text[strlen(text)] == text[suffixArray[i] - 1])
			result->index = i;
	}

	return result;
}

//Lexicographic DFS traversal to create the suffix array
void createSuffixArray(Node *node, int *i, int suffixArray[], String text)
{
	HashChildren *hashChild;
	int num_children = HASH_COUNT(node->children);

	if(num_children == 0) {
		suffixArray[*i] = node->suffixIndex;
		(*i)++;
		deleteNode(node);
		return;
	}

	HASH_SORT(node->children, sortNodesByFirstLetter);

	for(hashChild=node->children; hashChild!=NULL; hashChild=hashChild->hh.next)
		createSuffixArray(hashChild->node, i, suffixArray, text);

	deleteNode(node);
}

int sortNodesByFirstLetter(HashChildren *el1, HashChildren *el2)
{
	if(el1->firstLetter[0] < el2->firstLetter[0])			return -1;
	else if(el1->firstLetter[0] > el2->firstLetter[0])		return  1;
	else 													return  0;
}
