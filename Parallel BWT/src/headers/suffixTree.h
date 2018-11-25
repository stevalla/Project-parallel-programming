#ifndef SUFFIX_TREE_H
#define SUFFIX_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../extern/uthash.h"

typedef char* String;
typedef unsigned char* Ascii;
typedef struct Node Node;

//To maintain the list of children of each node.
typedef struct HashChildren
{
    int firstChar[1];	    /* key */
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


Node *buildSuffixTree(Ascii, int *);

void applyExtensions(Ascii, int *, int *, ActivePoint *, Node *);

int getEdgeLen(Node *);

void initText(Ascii);

Node *createNode(int, int *, Node *);

void updateAP(ActivePoint *, Node *, int, int);

int walkDown(ActivePoint *, Node *);

void checkSuffixLinkNeeded(Node *, Node *, Node *);

Node *createInternalNode(ActivePoint *, Node *, HashChildren *, int *, Ascii);

void createLeaf(int *, Node *, Node *, Ascii);

void addSuffixIndex(Node *, int, Ascii);

void addNewChild(Node *, Node *, Ascii);

void deleteChildren(HashChildren *);

void deleteNode(Node *);

HashChildren *findChildren(ActivePoint *, Ascii);


#endif
