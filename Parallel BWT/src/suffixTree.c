#include "suffixTree.h"
#include "bwt.h"

Node *buildSuffixTree(String text, int *phases)
{
	int remainingExt = 0;

	//Initialization of the root (special node with suffixLink NULL)
	int *endRoot = (int *) malloc(sizeof(int));
	*endRoot = -1;
	Node *root = createNode(-1, endRoot, NULL);

	//Initialization of the active point
	ActivePoint *ap = (ActivePoint *) malloc(sizeof(ActivePoint));
	ap->activeNode = root;
	ap->activeEdge = -1;
	ap->activeLen = 0;

	//Last created node
	Node *newest = NULL;

	initText(text);

	int *endLeaf = phases;

	for(*phases=0; *phases<strlen(text); (*phases)++) {

		//Apply rule 2 and 3
		remainingExt++;
		applyExtensions(text, &remainingExt, endLeaf, ap, newest, root);
	}

	(*endLeaf)--;

	int labelLen = 0;
	addSuffixIndex(root, labelLen, text);

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

HashChildren *findChildren(ActivePoint *ap, String text)
{
	HashChildren *hashChild;
	char key[1];
	key[0] = text[ap->activeEdge];
	HASH_FIND_STR(ap->activeNode->children, key, hashChild);

	return hashChild;
}

void applyExtensions(String text,
					 int *remainingExt,
					 int *endLeaf,
					 ActivePoint *ap,
					 Node *newest,
					 Node *root)
{
	while(*remainingExt) {

		if(ap->activeLen == 0)
			ap->activeEdge = *endLeaf;

		HashChildren *hashChild = findChildren(ap, text);

		if(hashChild == NULL) {

			//Apply rule 2 and create a new leaf
			createLeaf(endLeaf, ap->activeNode, root, text);

		} else {

			Node *child = hashChild->node;

			//Walk down the tree
			if(walkDown(ap, child))
				continue;	//restart the loop with the child as activeNode

			//Rule 3
			if(text[child->start + ap->activeLen] == text[*endLeaf]) {
				ap->activeLen++;
				break;	//Move on to next phase
			}

			/*
			 * Rule 2 apply to an internal node,
			 * activePoint in the middle of an edge.
			 */
			Node *internalNode = createInternalNode(ap, root, hashChild, endLeaf, text);
			checkSuffixLinkNeeded(internalNode, newest, root);
			newest = internalNode;

		}

		(*remainingExt)--;
		updateAP(ap, root, *endLeaf, *remainingExt);
	}
}

Node *createInternalNode(ActivePoint *ap,
						 Node *root,
						 HashChildren *hashChild,
						 int *endLeaf,
						 String text)
{
	Node *oldChild = hashChild->node;
	HASH_DEL(ap->activeNode->children, hashChild);

	//Create internal node and attach to its parent
	int *endInternalNode = (int *) malloc(sizeof(int));
	*endInternalNode = oldChild->start + ap->activeLen - 1;

	Node *internalNode = createNode(oldChild->start, endInternalNode, root);
	addNewChild(internalNode, ap->activeNode, text);

	//Update old child to be the new child of the internal node
	hashChild->node->start += ap->activeLen;
	char key = text[hashChild->node->start];
	strncpy(hashChild->firstLetter, &key, 1);
	HASH_ADD_STR(internalNode->children, firstLetter, hashChild);

	//Create new leaf from the internal node
	createLeaf(endLeaf, internalNode, root, text);

	printf("Rule 2 - Create internal node: %c\n", text[internalNode->start]);

	return internalNode;
}

void addNewChild(Node *child, Node *parent, String text)
{
	HashChildren *hash = (HashChildren *) malloc(sizeof(HashChildren));
	hash->node = child;
	char key = text[child->start];
	strncpy(hash->firstLetter, &key, 1);
	HASH_ADD_STR(parent->children, firstLetter, hash);
}

void createLeaf(int *endLeaf, Node *parent, Node *root, String text)
{
	Node *child = createNode(*endLeaf, endLeaf, root);
	addNewChild(child, parent, text);
	printf("Rule 2 continue - Create leaf: %c\n", text[child->start]);
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
	if(newest != NULL) {
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

void updateAP(ActivePoint *ap, Node *root, int phases, int remainingExt)
{
	if(ap->activeNode == root && ap->activeLen > 0) {

		ap->activeLen--;
		ap->activeEdge = phases - remainingExt + 1;

	} else if(ap->activeNode != root)
		ap->activeNode = ap->activeNode->suffixLink;

}

//DFS traversal to set the suffix indexes of the leaves
void addSuffixIndex(Node *node, int labelLen, String text)
{
	HashChildren *hashChild;
	int num_children = HASH_COUNT(node->children);

	if(num_children == 0) {
		node->suffixIndex = strlen(text) - labelLen;
		return;
	}

	for(hashChild=node->children; hashChild!=NULL; hashChild=hashChild->hh.next)
		addSuffixIndex(hashChild->node, labelLen + getEdgeLen(hashChild->node), text);

}


int getEdgeLen(Node *node)
{
	return *(node->end) - (node->start) + 1;
}

//Add a sentinel at the end of the text for the transformation
void initText(String text)
{
	text[strlen(text)] = (char)-1;
}
