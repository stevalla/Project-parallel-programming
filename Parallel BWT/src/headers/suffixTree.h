#ifndef SUFFIX_TREE_H
#define SUFFIX_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../extern/uthash.h"

typedef char* String;
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


Node *buildSuffixTree(short *, int *, size_t);

void applyExtensions(short *, int *, int *, ActivePoint *, Node *);

int getEdgeLen(Node *);

void initText(short *, size_t);

Node *createNode(int, int *, Node *);

void updateAP(ActivePoint *, Node *, int, int);

int walkDown(ActivePoint *, Node *);

void checkSuffixLinkNeeded(Node *, Node *, Node *);

Node *createInternalNode(ActivePoint *, Node *, HashChildren *, int *, short *);

void createLeaf(int *, Node *, Node *, short *);

void addSuffixIndex(Node *, int, short *, size_t);

void addNewChild(Node *, Node *, short *);

void deleteChildren(HashChildren *);

void deleteNode(Node *);

HashChildren *findChildren(ActivePoint *, short *);


#endif
