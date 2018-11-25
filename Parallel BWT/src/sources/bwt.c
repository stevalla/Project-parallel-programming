#include "../headers/bwt.h"

ResultBwt *bwtTransformation(Ascii text)
{

	int phases = 0;
	Node *suffixTree = buildSuffixTree(text, &phases);

	return getBWT(text, suffixTree);
}

ResultBwt *getBWT(Ascii text, Node *root)
{
	int len = strlen((String)text);
	int suffixArray[len];
	int i;
	ResultBwt *result;

	for(i=0; i<len; i++)
		suffixArray[i] = -1;

	i = 0;
	result = (ResultBwt *) malloc(sizeof(ResultBwt));
	result->text = (Ascii ) malloc(sizeof(unsigned char)*len + 1);

	createSuffixArray(root, &i, suffixArray, text);

	for(i=0; i<len; i++) {
		int index = suffixArray[i];

		//Store the BWT text
		if(index == 0)
			result->text[i] = text[len - 1];
		else
			result->text[i] = text[suffixArray[i] - 1];

		//Store the row of the original string
		if(text[len] == text[suffixArray[i] - 1])
			result->index = i;
	}

	return result;
}

//Lexicographic DFS traversal to create the suffix array
void createSuffixArray(Node *node, int *i, int suffixArray[], Ascii text)
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
		createSuffixArray(hashChild->node, i, suffixArray, text);

	deleteNode(node);
}

int sortNodesByFirstChar(HashChildren *el1, HashChildren *el2)
{
	if(el1->firstChar[0] < el2->firstChar[0])			return -1;
	else if(el1->firstChar[0] > el2->firstChar[0])		return  1;
	else 												return  0;
}
