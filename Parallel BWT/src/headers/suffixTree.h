#ifndef SUFFIX_TREE_H
#define SUFFIX_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../extern/uthash.h"

#define SENTINEL 256;

typedef struct Node Node;

//To maintain the list of children of each node.
typedef struct HashChildren
{
	short firstChar;	    			/* key */
    Node *node;				   			/* value */
    UT_hash_handle hh;         			/* makes this structure hash table */

} HashChildren;

struct Node
{
	Node *suffixLink;

	int start;
	int *end;

	int suffixIndex;

	HashChildren *children;
};

//These three variables define the current active point
typedef struct ActivePoint
{
	Node *activeNode;
	int activeLen;
	int activeEdge;

} ActivePoint;


Node *buildSuffixTree(short *const , int *const , const size_t);

void applyExtensions(short *const , int *const , int *const,
					 ActivePoint *const , Node *const);

int getEdgeLen(Node *const);

Node *createNode(const int, int *const, Node *const);

void updateAP(ActivePoint *const, Node *const, const int, const int);

int walkDown(ActivePoint *const, Node *const);

void checkSuffixLinkNeeded(Node *const, Node *, Node *const);

Node *createInternalNode(ActivePoint *const, Node *const,
						 HashChildren *const, int *const, short *const);

void createLeaf(int *const, Node *const, Node *const, short *const);

void addSuffixIndex(Node *const, const int, short *const, const size_t);

void addNewChild(Node *const, Node *const, short *const);

void deleteChildren(HashChildren *);

void deleteNode(Node *);

HashChildren *findChildren(ActivePoint *const, short *const);


#endif
