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
 * @file 	suffixTree.h
 * @author 	Stefano Valladares, ste.valladares@live.com
 * @date	20/12/2018
 * @version 1.1
 *
 * @brief	Header file implementing the construction of the suffix tree given
 * 			a sequence of bytes.
 *
 * @details This header defines the function needed to build the suffix tree
 * 			of a sequence of bytes. The most relevant functions of the interface
 * 			are ::buildSuffixTree and ::addSuffixIndex. The last one is used to
 * 			add the suffix index to the leaves of the tree. They can be useful
 * 			to extract properties of the sequence in input. @n
 * 			The interface offers also the possibility to delete one node
 * 			(::deleteNode) and to print the tree (::printTree). @n
 * 			The other functions are used for the construction fo the tree. The
 * 			algorithm implements the online construction of suffix tree
 * 			introduced by Ukkonen. The implementation follows the one proposed
 * 			by <http://www.geeksforgeeks.org>.@n
 *
 *
 * @see		<https://www.geeksforgeeks.org/ukkonens-suffix-tree-construction-part-1/>
 * @see		E. Ukkonen. “On-line construction of suffix trees”, Algorithmica, 14(3):249-260, 1995.
 */

#ifndef SUFFIX_TREE_H
#define SUFFIX_TREE_H

#include "../extern/uthash.h"
#include "util.h"

/**
 * The node structure is composed by several members that can be divided in two
 * groups:
 * 		- Three values to store the start and end indexes of the suffix in the
 * 		  edge connecting it to its parent, and its suffixIndex.
 * 		- The links with its children and the suffix link, that is used in the
 * 		  construction of the tree proposed by Ukkonen.
 */
typedef struct Node Node;

/**
 * This structure is used to maintain the list of children of each node.
 * They are maintain in a table, created by using the library offered by Uthash.@n
 * For a node of the suffix tree, each suffix starting from it starts with a
 * different letter. Thanks to this property the hash table that uses as key
 * the firstChar of the edge that connects the child to the current node will
 * have no collision.
 *
 * @see	https://troydhanson.github.io/uthash/
 */
typedef struct HashChildren
{
	unsigned 		firstChar;	/**< Key value. It represents the first char
									 of the suffix starting at the current node
									 and terminating at the corresponding child.*/
    Node 			*node;		/**< Child. Pointer to the corresponding child
    								 node. */
    UT_hash_handle 	hh;			/**< Makes this structure an hash table. */

} HashChildren;

/**
 * In the node structure there is also information about the label of the
 * edge that connects it to its parent. In particular the suffix on each edge
 * is encoded in the child node of the edge by storing the indexes that
 * delimit the suffix in the input string.
 */
struct Node
{
	int 		start;		 /**< Index of the start of the suffix in the
								  edge with its parent. */
	int 		*end;		 /**< Pointer to the index of the end of the
								  suffix in the edge with its parent. */
	int 		suffixIndex; /**< Suffix index of the nodes. (-1 if the
								  node is internal). */
	HashChildren *children;	 /**< Pointer to the hash table containing the
								  children of the node. NULL if it is a leaf.*/
	Node 		*suffixLink; /**< Pointer to another node in a different subtree.*/

};

/**
 * This structure is used to keep track of the current active node, the current
 * active edge and the current active length in order to speed up the
 * construction of the suffix tree in the Ukkonen's version. @n
 * It also stores the pointer to the root of the tree and the current phase
 * of the construction.
 *
 * @see	<https://www.geeksforgeeks.org/ukkonens-suffix-tree-construction-part-3/>
 */
typedef struct ActivePoint
{
	Node 			*activeNode;	///< Current active node.
	int 			activeLen;		///< Current active length.
	int 			activeEdge;		///< Current active edge.

	Node			*root;			///< Root of the suffix tree.
	int				*phase;			///< Current phase.

} ActivePoint;

/**
 * @brief	Returns the root of the suffix tree of the input sequence of bytes.
 *
 * @note	The input sequence is assumed to terminate with a sentinel character,
 * 		  	that is the only one present in the input.
 *
 * @param[in] 		input		The input sequence from where the tree is built.
 * @param[in] 		inputLen 	The length of the input.
 * @return	The root of the syntax tree of the sequence in input.
 */
Node *buildSuffixTree(unsigned *const input, const size_t inputLen);

/**
 * @brief	Add the suffix indexes to the leaves of the suffix tree.
 *
 * @param[in,out]	node		The root of the current subtree of the recursion.
 * @param[in] 		labelLen	Length of the current suffix under consideration.
 * @param[in] 		inputLen	Length of the input sequence.
 */
void addSuffixIndex(Node *const  node,
					const int	 labelLen,
					const size_t inputLen);

/**
 * @brief	Apply the extensions to the tree.
 *
 * @param[in] 		input		The input sequence.
 * @param[in,out] 	remainder	Counts the number of suffixes to be added.
 * @param[in,out]	ap			Current active point.
 */
void applyExtensions(unsigned 	 *const input,
					 int 	  	 *const remainder,
					 ActivePoint *const	ap);

/**
 * @brief	Creates a node with default initialization.
 *
 * @param[in] 		start	The start of the new node.
 * @param[in]		end		Pointer to the end of the new node.
 * @param[in] 		root	The root of the suffix tree.
 * @return	The new node created.
 */
Node *createNode(const int start, int *const end, Node *const root);

/**
 * @brief	Splits an edge by creating a new internal and a new leaf.
 *
 * @param[in]		ap		The current active point.
 * @param[in,out] 	child	The child connected to the edge to be split.
 * @param[in] 		input	The input sequence.
 * @return	The internal node created.
 */
Node *createInternalNode(ActivePoint   *const ap,
						 HashChildren **const child,
						 unsigned 	   *const input);

/**
 * @brief	Creates a leaf of the tree and attach it to the parent passed as arg.
 *
 * @param[in] 		ap		The current acrive point.
 * @param[in,out] 	parent	The parent of the new leaf.
 * @param[in] 		input	The input sequence.
 */
void createLeaf(ActivePoint *const ap,
				Node 	 	*const parent,
				unsigned 	*const input);

/**
 * @brief	Attachs a new child to a node.
 *
 * @param[in]		child	The child to attach
 * @param[in,out] 	parent	The parent of the child.
 * @param[in] 		input	The input sequence.
 */
void addNewChild(Node 	  *const child,
				 Node 	  *const parent,
				 unsigned *const input);

/**
 * @brief	Sets a suffix link to a node.
 *
 * @param[in] 		node	The node at which the suffix link will point.
 * @param[in,out] 	newest	Pointer to the node which suffix link must be set.
 */
void setSuffixLink(Node *const node, Node **newest);

/**
 * @brief	Update the active point.
 *
 * @param[in,out]	ap			The current pointer to the active point to update.
 * @param[in] 		remainder	The remainder of the suffixes to be added.
 */
void updateAP(ActivePoint *const ap, const int remainder);

/**
 * @brief	Walk down the tree onw level following one edge.
 *
 * @param[in,out] 	ap			The current pointer to the acrive point.
 * @param[in] 		currentNode	The current node from where to start walking.
 * @return	1 if it has performed walkdown, 0 otherwise.
 */
int walkDown(ActivePoint *const ap, Node *const currentNode);

/**
 * @brief	Returns the length of the label on the edge terminating in
 * 			the input node.
 *
 * @param[in] 		node		Node where the edge terminates.
 * @return	The length of the label.
 */
int getEdgeLen(Node *const node);

/**
 * @brief	Delete a node by free the memory for it and all of its children.
 *
 * @param[in] 		node		The node to be deleted.
 */
void deleteNode(Node *node);

/**
 * @brief	Frees the hash table that contains the children of a node.
 *
 * @param[in] 		children	The hash table to be freed.
 */
void deleteChildren(HashChildren **children);

/**
 * @brief	Depth-first traversal of the tree to print it.
 *
 * @param[in] 		node		The root of the current subtree to be printed out.
 * @param[in]	 	input		The sequence in put.
 */
void printTree(Node *node, unsigned *input);

#endif
