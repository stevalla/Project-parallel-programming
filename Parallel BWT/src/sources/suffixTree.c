#include "../headers/suffixTree.h"

#include "../headers/bwt.h"

Node *buildSuffixTree(short *input, int *phases, size_t inputLen)
{
	//Variables
	ActivePoint *ap = (ActivePoint *) malloc(sizeof(ActivePoint));
	Node *root;
	int remainder = 0;	//Counts how many suffixes we still need to insert
	int labelLen = 0;
	int *endLeaf = phases;
	int *endRoot = (int *) malloc(sizeof(int));

	//Initialization of the root (special node with suffixLink NULL)
	*endRoot = -1;
	root = createNode(-1, endRoot, NULL);

	//Initialization of the active point
	ap->activeNode = root;
	ap->activeEdge = -1;
	ap->activeLen = 0;

	//Append the sentinel
	initText(input, inputLen);

	for(*phases=0; *phases<=inputLen; (*phases)++) {
		remainder++;
		applyExtensions(input, &remainder, endLeaf, ap, root);
	}

	(*endLeaf)--;

	addSuffixIndex(root, labelLen, input, inputLen);

	free(ap);

	return root;
}

Node *createNode(int start, int *end, Node *root)
{
	Node *node = (Node *) malloc(sizeof(Node));

	//Default initialization
	node->children = NULL;
	node->suffixLink = root;
	node->suffixIndex = -1;
	node->start = start;
	node->end = end;

	return node;
}

HashChildren *findChildren(ActivePoint *ap, short *inputText)
{
	HashChildren *hashChild;
	int key;

	key = (int)inputText[ap->activeEdge];
	HASH_FIND_INT(ap->activeNode->children, &key, hashChild);

	return hashChild;
}

void applyExtensions(short *input,
					 int *remainder,
					 int *endLeaf,
					 ActivePoint *ap,
					 Node *root)
{
	Node *child;
	Node *newest = NULL;
	HashChildren *hashChild;

	while(*remainder) {

		if(ap->activeLen == 0)
			ap->activeEdge = *endLeaf;

		hashChild = findChildren(ap, input);

		if(hashChild == NULL) {

			//Apply rule 2 and create a new leaf
			createLeaf(endLeaf, ap->activeNode, root, input);
			checkSuffixLinkNeeded(ap->activeNode, newest, root);

		} else {

			child = hashChild->node;

			//Walk down the tree
			if(walkDown(ap, child))
				continue;	//restart the loop with the child as activeNode


			//Rule 3
			if(input[child->start + ap->activeLen] == input[*endLeaf]) {
				ap->activeLen++;
				checkSuffixLinkNeeded(ap->activeNode, newest, root);
				break;	//Move on to next phase
			}

			/*
			 * Rule 2 apply to an internal node,
			 * activePoint in the middle of an edge.
			 */
			Node *internalNode = createInternalNode(ap, root, hashChild,
					endLeaf, input);
			checkSuffixLinkNeeded(internalNode, newest, root);
			newest = internalNode;

		}

		(*remainder)--;
		updateAP(ap, root, *endLeaf, *remainder);
	}
}

Node *createInternalNode(ActivePoint *ap,
						 Node *root,
						 HashChildren *hashChild,
						 int *endLeaf,
						 short *inputText)
{
	Node *oldChild = hashChild->node;
	int *endInternalNode = (int *) malloc(sizeof(int));
	Node *internalNode;

	HASH_DEL(ap->activeNode->children, hashChild);

	//Create internal node and attach to its parent
	*endInternalNode = oldChild->start + ap->activeLen - 1;
	internalNode = createNode(oldChild->start, endInternalNode, root);

	//Attach the old child to the internal node and the last one to its parent
	hashChild->node->start += ap->activeLen;
	hashChild->firstChar = inputText[hashChild->node->start];
	addNewChild(internalNode, ap->activeNode, inputText);
	HASH_ADD_INT(internalNode->children, firstChar, hashChild);

	//Create new leaf from the internal node
	createLeaf(endLeaf, internalNode, root, inputText);

	return internalNode;
}

void addNewChild(Node *child, Node *parent, short *inputText)
{
	HashChildren *hash = (HashChildren *) malloc(sizeof(HashChildren));
	hash->node = child;
	hash->firstChar = inputText[child->start];
	HASH_ADD_INT(parent->children, firstChar, hash);
}

void createLeaf(int *endLeaf, Node *parent, Node *root, short *inputText)
{
	Node *child = createNode(*endLeaf, endLeaf, root);
	addNewChild(child, parent, inputText);
}

//Free the hash table that contains the children of a node
void deleteChildren(HashChildren *children)
{
	HashChildren *current, *tmp;

	HASH_ITER(hh, children, current, tmp) {
		HASH_DEL(children, current);
		free(current);
	}
}

//Delete by free all the memory allocated for a node
void deleteNode(Node *node)
{
	if(node->suffixIndex == -1)
		free(node->end);
	deleteChildren(node->children);
	free(node);
}

void checkSuffixLinkNeeded(Node *node, Node *newest, Node *root)
{
	/*
	 * Update suffix link if newest is pointing to a node,
	 * then reset newest for next iterations.
	 *
	 * If newest is not null means that in the last iteration
	 * an internal node was created and now we set its suffix link.
	 *
	 * The only node with suffixLink null is the root and all the other
	 * nodes have the root as default suffixLink, so no need to update.
	 */
	if(newest != NULL && *node->end != -1 && node->children != NULL) {
		newest->suffixLink = node;
		newest = NULL;
	}
}

//Apply skip/count trick to walk down the tree quickly
int walkDown(ActivePoint *ap, Node *currentNode)
{
	int edgeLength = getEdgeLen(currentNode);
	if(ap->activeLen >= edgeLength) {
		ap->activeEdge += edgeLength;
		ap->activeLen -= edgeLength;
		ap->activeNode = currentNode;

		return 1;
	}

	return 0;
}

void updateAP(ActivePoint *ap, Node *root, int phases, int remainder)
{
	if(ap->activeNode == root && ap->activeLen > 0) {

		ap->activeLen--;
		ap->activeEdge = phases - remainder + 1;

	} else if(ap->activeNode != root)
		ap->activeNode = ap->activeNode->suffixLink;
}

//DFS traversal to set the suffix indexes of the leaves
void addSuffixIndex(Node *node, int labelLen, short *inputText, size_t inputLen)
{
	HashChildren *hashChild;
	int num_children = HASH_COUNT(node->children);

	if(num_children == 0) {
		node->suffixIndex = (inputLen + 1) - labelLen;
		return;
	}

	for(hashChild=node->children; hashChild!=NULL; hashChild=hashChild->hh.next)
		addSuffixIndex(hashChild->node,
				labelLen + getEdgeLen(hashChild->node), inputText, inputLen);
}


int getEdgeLen(Node *node)
{
	return *(node->end) - (node->start) + 1;
}

//Add a sentinel at the end of the text for the transformation
void initText(short *text, size_t len)
{
	text[len] = 257;
}
