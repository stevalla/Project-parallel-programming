#ifndef SUFFIX_TREE_H
#define SUFFIX_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uthash.h"

typedef char* String;
typedef struct Node Node;

//To maintain the list of children of each node.
typedef struct HashChildren
{
    char firstLetter[1];	   /* key */
    Node *node;				   /* value */
    UT_hash_handle hh;         /* makes this structure hash table */

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


Node *buildSuffixTree(String, int *);

void applyExtensions(String, int *, int *, ActivePoint *, Node *, Node *);

int getEdgeLen(Node *);

void initText(String);

Node *createNode(int, int *, Node *);

void updateAP(ActivePoint *, Node *, int, int);

int walkDown(ActivePoint *, Node *);

void checkSuffixLinkNeeded(Node *, Node *, Node *);

Node *createInternalNode(ActivePoint *, Node *, HashChildren *, int *, String);

void createLeaf(int *, Node *, Node *, String);

void addSuffixIndex(Node *, int, String);

void addNewChild(Node *, Node *, String);

void deleteChildren(HashChildren *);

void deleteNode(Node *);

HashChildren *findChildren(ActivePoint *, String);

//void printfEdgeLength(Node *, String);

#endif
