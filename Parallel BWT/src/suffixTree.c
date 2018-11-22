#include "suffixTree.h"

int main(int argc, char *argv[]) {

	char text[MAX_LENGTH] = "BANANA";

	buildSuffixTree(text);

	return 0;
}

void buildSuffixTree(String text)
{
	int phases;
	int remainingExt = 0;

	//Initialization of the root (special node with suffixLink NULL)
	int *endRoot = (int*) malloc(sizeof(int));
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

	for(phases=0; phases<strlen(text); phases++) {

		//Apply rule 2 and 3
		remainingExt++;
		applyExtensions(text, remainingExt, phases, ap, newest, root);

	}

}

Node *createNode(int start, int *end, Node *root)
{
	Node *node = (Node*) malloc(sizeof(Node));

	//Default initialization
	node->children = NULL;
	node->suffixLink = root;
	node->suffixIndex = -1;
	node->start = start;
	node->end = end;

	return node;
}

void applyExtensions(String text,
					 int remainingExt,
					 int position,
					 ActivePoint *ap,
					 Node *newest,
					 Node *root)
{
	while(remainingExt) {

		if(ap->activeLen == 0)
			ap->activeEdge = position;

		HashNode *hashChild;
		HASH_FIND_INT(ap->activeNode->children, &ap->activeEdge, hashChild);

		if(hashChild == NULL) {

			//Apply rule 2 and create a new leaf
			HashNode *newChild = (HashNode*) malloc(sizeof(HashNode));
			newChild->id = position;
			newChild->node = createNode(position, &position, root);;
			HASH_ADD_INT(ap->activeNode->children, id, newChild);

			checkSuffixLinkNeeded(ap, newest);

		} else {

			Node *child = hashChild->node;

			//Walk down the tree
			if(walkDown(ap, child))
				continue;	//restart the loop with the child as activeNode

			//Rule 3
			if(text[child->start + ap->activeLen] == text[position]) {

				checkSuffixLinkNeeded(ap, newest);

				ap->activeLen++;

				break;	//Move on to next phase
			}

			/*
			 * Rule 2 apply to an internal node,
			 * activePoint in the middle of an edge.
			 */


			remainingExt--;
			updateAP(ap, root, position, remainingExt);
		}
	}
}

void checkSuffixLinkNeeded(ActivePoint *ap, Node *newest)
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
	if(newest != NULL && ap->activeNode->suffixLink != NULL) {
		newest->suffixLink = ap->activeNode;
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

int getEdgeLen(Node *node)
{
	return *(node->end) - (node->start) + 1;
}

//Add a sentinel at the end of the text for the transformation
void initText(String text)
{
	strcat((text), "$");
}
