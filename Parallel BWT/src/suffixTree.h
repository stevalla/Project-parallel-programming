#ifndef SUFFIX_TREE_H
#define SUFFIX_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uthash.h"

#define MAX_LENGTH 2000

typedef char* String;
typedef struct Node Node;

//To maintain the list of children of each node.
typedef struct HashNode
{
    int id;                    /* key is the index of the first letter of the label*/
    Node *node;
    UT_hash_handle hh;         /* makes this structure hash table */

} HashNode;

struct Node
{
	Node *suffixLink;

	int start;
	int *end;

	int suffixIndex;

	HashNode *children;
};

//These three variables define the current active point
typedef struct ActivePoint
{
	Node *activeNode;
	int activeLen;
	int activeEdge;

} ActivePoint;

void buildSuffixTree(String);

void applyExtensions(String, int, int, ActivePoint *, Node *, Node *);

int getEdgeLen(Node *);

void initText(String);

Node *createNode(int, int *, Node *);

void updateAP(ActivePoint *, Node *, int, int);

int walkDown(ActivePoint *, Node *);

void checkSuffixLinkNeeded(ActivePoint *, Node *);



#endif
