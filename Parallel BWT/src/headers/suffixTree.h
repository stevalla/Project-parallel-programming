#ifndef SUFFIX_TREE_H
#define SUFFIX_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../extern/uthash.h"

typedef struct Node Node;

//To maintain the list of children of each node.
typedef struct HashChildren
{
	unsigned firstChar;	    			/* key */
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


Node *buildSuffixTree(unsigned *const, const size_t);

void applyExtensions(unsigned *const , int *const , int *const,
					 ActivePoint *const , Node *const);

int getEdgeLen(Node *const);

Node *createNode(const int, int *const, Node *const);

void updateAP(ActivePoint *const, Node *const, const int, const int);

int walkDown(ActivePoint *const, Node *const);

void addSuffixLink(Node *const, Node **, Node *const);

Node *createInternalNode(ActivePoint *const, Node *const,
						 HashChildren **const, int *const, unsigned *const);

void createLeaf(int *const, Node *const, Node *const, unsigned *const);

void addSuffixIndex(Node *const, const int, unsigned *const, const size_t);

void addNewChild(Node *const, Node *const, unsigned *const);

void deleteChildren(HashChildren **);

void deleteNode(Node *);

HashChildren *findChildren(ActivePoint *const, unsigned *const);


#endif
