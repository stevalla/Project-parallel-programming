#include "../headers/suffixTree.h"

Node *buildSuffixTree(unsigned *const in,
					  const size_t inLen)
{
	//Variables
	ActivePoint ap;
	Node *root;
	int remainder = 0;	//Counts how many suffixes we still need to insert
	int labelLen = 0;
	int phases;
	int *endLeaf = &phases;
	int *endRoot = (int *) malloc(sizeof(int));

	//Initialization of the root (special node with suffixLink NULL)
	*endRoot = -1;
	root = createNode(-1, endRoot, NULL);

	//Initialization of the active point
	ap.activeNode = root;
	ap.activeEdge = -1;
	ap.activeLen = 0;
	for(phases=0; phases < inLen; phases++) {

		remainder++;
		applyExtensions(in, &remainder, endLeaf, &ap, root);
	}

	(*endLeaf)--;

//	printTree(root, in);
	addSuffixIndex(root, labelLen, in, inLen);

	return root;
}

Node *createNode(const int start, int *const end, Node *const root)
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

HashChildren *findChildren(ActivePoint *const ap, unsigned *const input)
{
	HashChildren *child;
	int key;

	key = input[ap->activeEdge];
	HASH_FIND_INT(ap->activeNode->children, &key, child);

	return child;
}

void printTree(Node *node, unsigned *input)
{
	HashChildren *child;
	int numChildren = HASH_COUNT(node->children);

	if(numChildren == 0) {
		printf("\tLeaf: %d %d %d %d\n",
				node->start, *node->end, input[node->start], input[*node->end]);
		return;
	}
	printf("Internal node: %d %d %d %d\n",
			node->start, *node->end, input[node->start], input[*node->end]);
	printf("Num children %d\n", numChildren);
	for(child=node->children; child!=NULL; child=child->hh.next) {
		printTree(child->node, input);
	}
}

void applyExtensions(unsigned *const input,
					 int *const remainder,
					 int *const endLeaf,
					 ActivePoint *const ap,
					 Node *const root)
{
	Node *child;
	Node *newest = NULL;
	HashChildren *hashChild;

	while(*remainder) {

//		printf("Remainder: %d\n", *remainder);

		if(ap->activeLen == 0) {
			ap->activeEdge = *endLeaf;
//			printf("Active edge = 0, new active edge = %d\n",
//					ap->activeEdge);
		}

		hashChild = findChildren(ap, input);

		if(hashChild == NULL) {
//			printf("Rule 2:\n");
			//Apply rule 2 and create a new leaf
			createLeaf(endLeaf, ap->activeNode, root, input);
			if(newest != NULL) {
				addSuffixLink(ap->activeNode, &newest, root);
//				printf("Newest: %d %d\n", newest->start, *newest->end);
			}


		} else {
//			printf("Found children\n");
			child = hashChild->node;

			//Walk down the tree
			if(walkDown(ap, child))
				continue;	//restart the loop with the child as activeNode

			//Rule 3
			if(input[child->start + ap->activeLen] == input[*endLeaf]) {
//				printf("Rule 3 %d %d\n", input[child->start + ap->activeLen],
//						input[*endLeaf]);
				ap->activeLen++;
				if(newest != NULL && ap->activeNode != root)
					addSuffixLink(ap->activeNode, &newest, root);
				break;	//Move on to next phase
			}

			/*
			 * Rule 2 apply to an internal node,
			 * activePoint in the middle of an edge.
			 */
//			printf("Rule 2 internal node\n");
			Node *internalNode = createInternalNode(ap, root, &hashChild,
					endLeaf, input);
			if(newest != NULL)
				addSuffixLink(internalNode, &newest, root);
			newest = internalNode;

		}

		(*remainder)--;
		updateAP(ap, root, *endLeaf, *remainder);
//		printf("End of extension.\n");
//		printTree(root, input);
//		puts("\n");
	}
}

Node *createInternalNode(ActivePoint *const ap,
						 Node *const root,
						 HashChildren **const child,
						 int *const endLeaf,
						 unsigned *const input)
{
	Node *oldChild = (*child)->node;
	int *endInternalNode = (int *) malloc(sizeof(int));
	Node *internalNode;

	HASH_DEL(ap->activeNode->children, *child);

	//Create internal node and attach to its parent
	*endInternalNode = oldChild->start + ap->activeLen - 1;
	internalNode = createNode(oldChild->start, endInternalNode, root);

	//Attach the old child to the internal node and the last one to its parent
	(*child)->node->start += ap->activeLen;
	(*child)->firstChar = input[(*child)->node->start];
	addNewChild(internalNode, ap->activeNode, input);
	HASH_ADD_INT(internalNode->children, firstChar, *child);

//	printf("Create internal node %d %d\n", internalNode->start, *endInternalNode);
	//Create new leaf from the internal node
	createLeaf(endLeaf, internalNode, root, input);

	return internalNode;
}

void addNewChild(Node *const child,
				 Node *const parent,
				 unsigned *const input)
{
	HashChildren *hash = (HashChildren *) malloc(sizeof(HashChildren));
	hash->node = child;
	hash->firstChar = input[child->start];
	HASH_ADD_INT(parent->children, firstChar, hash);
}

void createLeaf(int *const endLeaf,
				Node *const parent,
				Node *const root,
				unsigned *const input)
{
	Node *child = createNode(*endLeaf, endLeaf, root);
	addNewChild(child, parent, input);
//	printf("Create leaf to parent: %d\n", *parent->end);
}

//Free the hash table that contains the children of a node
void deleteChildren(HashChildren **children)
{
	HashChildren *current, *tmp;

	HASH_ITER(hh, *children, current, tmp) {
		HASH_DEL(*children, current);
		free(current);
	}
}

//Delete by free all the memory allocated for a node
void deleteNode(Node *node)
{
	if(node->suffixIndex == -1) {
		deleteChildren(&node->children);
		free(node->end);
	}
	free(node);
}

void addSuffixLink(Node *const node,
				   Node **newest,
				   Node *const root)
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
	(*newest)->suffixLink = node;
//	printf("Suffix link added from node %d %d to %d %d\n",
//			(*newest)->start,
//			*(*newest)->end,
//			node->start,
//			*node->end);
	*newest = NULL;

}

//Apply skip/count trick to walk down the tree quickly
int walkDown(ActivePoint *const ap, Node *const currentNode)
{
	int edgeLength = getEdgeLen(currentNode);
	if(ap->activeLen >= edgeLength) {
		ap->activeEdge += edgeLength;
		ap->activeLen -= edgeLength;
		ap->activeNode = currentNode;
//		printf("Walkdown\n");
//		printf("New AP: node %d len %d edge %d\n", *ap->activeNode->end,
//												 ap->activeLen,
//												 ap->activeEdge);
		return 1;
	}

	return 0;
}

void updateAP(ActivePoint *const ap,
			  Node *const root,
			  const int phases,
			  const int remainder)
{
	if(ap->activeNode == root && ap->activeLen > 0) {
		ap->activeLen--;
		ap->activeEdge = phases - remainder + 1;
//		printf("Update ap len %d edge %d", ap->activeLen, ap->activeEdge);
	} else if(ap->activeNode != root) {
		ap->activeNode = ap->activeNode->suffixLink;
//		printf("Follow suffix link ap node = %d", *ap->activeNode->end);
	}
}

//DFS traversal to set the suffix indexes of the leaves
void addSuffixIndex(Node *const node,
					const int labelLen,
					unsigned *const input,
					const size_t inputLen)
{
	HashChildren *child;
	int num_children = HASH_COUNT(node->children);

	if(num_children == 0) {
		node->suffixIndex = inputLen - labelLen;
		return;
	}

	for(child=node->children; child!=NULL; child=child->hh.next)
		addSuffixIndex(child->node,
				labelLen + getEdgeLen(child->node), input, inputLen);
}


int getEdgeLen(Node *const node)
{
	return *(node->end) - (node->start) + 1;
}


