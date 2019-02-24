/******************************************************************************
 * Copyright (C) 2018 by Stefano Valladares                                   *
 *                                                                            *
 * This file is part of ParallelBWTzip.                                       *
 *                                                                            *
 *   ParallelBWTzip is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation, either version 3 of the       *
 *   License, or (at your option) any later version.                          *
 *                                                                            *
 *   ParallelBWTzip is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU Lesser General Public License for more details.                      *
 *                                                                            *
 *   You should have received a copy of the GNU Lesser General Public         *
 *   License along with ParallelBWTzip. 									  *
 *   If not, see <http://www.gnu.org/licenses/>.     						  *
 ******************************************************************************/

/**
 * @file 	suffixTree.c
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief	Source file implementing the construction of the suffix tree given
 * 			a sequence of bytes.
 */

#include "../headers/suffixTree.h"

/**
 * The function firstly initializes the active point and the root of the tree. @n
 * After that it performs n extensions of the suffix tree, where n is the length
 * of the input string. Finally it adds the suffix indexes to the leaves of the
 * tree and returns the root. @n
 * Basically the main function implementing the Ukkonen's algorithm is
 * ::applyExtensions and is called a number of times equal to the length of
 * the input string. Each time a new byte is processed.
 */
Node *buildSuffixTree(unsigned *const input, const size_t inputLen)
{
	ActivePoint ap;
	int remainder = 0;	//Counts how many suffixes we still need to insert
	int phases;
	int *endRoot = (int *) malloc(sizeof(int));

	*endRoot = -1;

	//Root -> special node with suffixLink NULL, start = end = -1
	ap.root 	  = createNode(-1, endRoot, NULL);
	ap.phase 	  = &phases;
	ap.activeNode = ap.root;
	ap.activeEdge = -1;
	ap.activeLen  = 0;

	for(phases=0; phases < inputLen; phases++) {
		remainder++;
		applyExtensions(input, &remainder, &ap);
	}
	(*ap.phase)--;

	addSuffixIndex(ap.root, 0, inputLen);

	return ap.root;
}

/**
 * Depth-first search traversal of the suffix tree to set the suffix
 * indexes of the leaves. The one of the internal nodes are set to -1.
 * The function traverses the tree in a recursive way adding the
 * suffix indixes to the leaves.
 */
void addSuffixIndex(Node *const  node,
					const int 	 labelLen,
					const size_t inputLen)
{
	HashChildren *child;
	int num_children = HASH_COUNT(node->children);

	//Base case
	if(num_children == 0) {
		node->suffixIndex = inputLen - labelLen;
		return;
	}

	//Recursion
	for(child=node->children; child!=NULL; child=child->hh.next)
		addSuffixIndex(child->node,
				labelLen + getEdgeLen(child->node), inputLen);
}

/**
 * This function is applied once for each input byte and it performs several
 * operation in loop until remainder is 0:
 * 	- If there is not an edge from the current active node such that the
 * 	  label on the edge starts with the same character of the one in input,
 * 	  it creates a new leaf starting from the current node.
 * 	- Otherwise it walks down the tree until either:
 * 		- The current char being processed is already on the edge, in that
 * 		   case the function returns. The remainder is not zero so the
 * 		   algorithm can keep track of the suffix the are missing in the tree.
 * 		   They will be generated in the next phases.
 * 		- Otherwise an internal node is created because the input character is
 * 		   found in the middle of an edge.
 */
void applyExtensions(unsigned 	 *const input,
					 int 		 *const remainder,
					 ActivePoint *const ap)
{
	Node 		 *child;
	Node 		 *newest = NULL;
	HashChildren *hashChild;

	while(*remainder) {

		if(ap->activeLen == 0)
			ap->activeEdge = *(ap->phase);

		HASH_FIND_INT(ap->activeNode->children,
					  &input[ap->activeEdge],
					  hashChild);

		if(hashChild == NULL) {
			createLeaf(ap, ap->activeNode, input);
			if(newest != NULL)
				setSuffixLink(ap->activeNode, &newest);

		} else {

			child = hashChild->node;

			if(walkDown(ap, child))
				continue;	//Restart the loop with the child as activeNode

			//Current char being processed is already on the edge
			if(input[child->start + ap->activeLen] == input[*(ap->phase)]) {

				ap->activeLen++;
				if(newest != NULL && ap->activeNode != ap->root)
					setSuffixLink(ap->activeNode, &newest);

				break;	//Move on to next phase
			}

			Node *internalNode = createInternalNode(ap, &hashChild, input);
			if(newest != NULL)
				setSuffixLink(internalNode, &newest);
			newest = internalNode;
		}

		(*remainder)--;
		updateAP(ap, *remainder);
	}
}

/**
 * The default initialization of the node is performed by setting:
 * 		-	The end, start passed as arguments.
 * 		-	The root as suffix link.
 * 		-	The set of children is NULL.
 * 		-	The suffix index is set to -1.
 */
Node *createNode(const int start, int *const end, Node *const root)
{
	Node *node = (Node *) malloc(sizeof(Node));

	node->children    = NULL;
	node->suffixLink  = root;
	node->suffixIndex = -1;
	node->start 	  = start;
	node->end 		  = end;

	return node;
}

/**
 * The functions needs to update/insert various part of the tree around
 * the edge that must be split and the. This is done in different steps:
 */
Node *createInternalNode(ActivePoint   *const ap,
						 HashChildren **const child,
						 unsigned 	   *const input)
{
	Node *oldChild 		 = (*child)->node;
	int *endInternalNode = (int *) malloc(sizeof(int));
	Node *internalNode;

	/// - Delete the old child from the children of the current node.
	HASH_DEL(ap->activeNode->children, *child);

	/// - Create internal node
	*endInternalNode = oldChild->start + ap->activeLen - 1;
	internalNode = createNode(oldChild->start, endInternalNode, ap->root);

	/**
	 * - Attach the old child as new child of the new internal node, then
	 * 	 update the old child label, so that the new start of the node is
	 * 	 the actual plus the active length.
	 */
	(*child)->node->start += ap->activeLen;
	(*child)->firstChar    = input[(*child)->node->start];
	addNewChild(internalNode, ap->activeNode, input);
	HASH_ADD_INT(internalNode->children, firstChar, *child);

	/// - Create new leaf from the internal node.
	createLeaf(ap, internalNode, input);

	return internalNode;
}


void createLeaf(ActivePoint *const ap,
				Node 	 	*const parent,
				unsigned 	*const input)
{
	Node *child = createNode(*(ap->phase), ap->phase, ap->root);
	addNewChild(child, parent, input);
}

/**
 * The input sequence is used to retrieve the key value of the hash table.
 */
void addNewChild(Node 	  *const child,
				 Node 	  *const parent,
				 unsigned *const input)
{
	HashChildren *hash = (HashChildren *) malloc(sizeof(HashChildren));
	hash->node 		   = child;
	hash->firstChar    = input[child->start];
	HASH_ADD_INT(parent->children, firstChar, hash);
}

/**
 * Update suffix link if newest is pointing to a node,
 * then reset newest for next iterations. @n
 * If newest is not null means that in the last iteration
 * an internal node was created and now we set its suffix link. @n
 * The only node with suffixLink null is the root and all the other
 * nodes have the root as default suffixLink.
 */
void setSuffixLink(Node *const node, Node **newest)
{
	(*newest)->suffixLink = node;
	*newest = NULL;
}

/**
 * Update the active point depending on whether the active node is root or not.
 */
void updateAP(ActivePoint *const ap, const int remainder)
{
	if(ap->activeNode == ap->root && ap->activeLen > 0) {

		ap->activeLen--;
		ap->activeEdge = (*ap->phase) - remainder + 1;

	} else if(ap->activeNode != ap->root)
		ap->activeNode = ap->activeNode->suffixLink;
}

/**
 * This function allows to walk down the tree quickly. In particulare if the
 * active lenght is greater then the label length of the selected node, it
 * can directly skip to that node by updating the active point.
 */
int walkDown(ActivePoint *const ap, Node *const currentNode)
{
	int edgeLength = getEdgeLen(currentNode);
	if(ap->activeLen   >= edgeLength) {
		ap->activeEdge += edgeLength;
		ap->activeLen  -= edgeLength;
		ap->activeNode  = currentNode;
		return 1;
	}

	return 0;
}


int getEdgeLen(Node *const node)
{
	return *(node->end) - (node->start) + 1;
}


void deleteNode(Node *node)
{
	if(node->suffixIndex == -1) {
		deleteChildren(&node->children);
		free(node->end);
	}
	free(node);
}

void deleteChildren(HashChildren **children)
{
	HashChildren *current, *tmp;

	HASH_ITER(hh, *children, current, tmp) {
		HASH_DEL(*children, current);
		free(current);
	}
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

	for(child=node->children; child!=NULL; child=child->hh.next)
		printTree(child->node, input);
}

