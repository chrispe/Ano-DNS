/*
* * * * * * * * * * * * * * * * * * * * * *
*                                         *
*                 Ano-DNS                 *
*               version 0.1               *
*                                         *
*    Developed by Christos Petropoulos    *
*                                         *
* * * * * * * * * * * * * * * * * * * * * *

A smiple implementation of a binary search tree.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

typedef struct tree_node{
	char * name;
	struct tree_node * lc;
	struct tree_node * rc;
	void * data_ptr;
}tree_node;

/**
 * @brief Inserts a new node to the binary search tree.
   @param root: The root pointer of the binary search tree.
   @param name: The name (aka key) of the tree node.
   @param data_ptr: A pointer to the data that will be copied to the new node.
   @param found: A tree node, in case of not NULL the value of it points
   to the new allocated node.
   @return: The new binary search tree.
 */ 
tree_node * tree_insert_node(tree_node * root,const char * name, void * data_ptr, tree_node ** found);

/**
 * @brief Creates a new tree node (by doing the required allocations).
   @param name: The name of the node.
   @param ptr: The pointer to the data that will be copied to the node.
   @param data_ptr: A pointer to the data that will be copied to the new node.
   @return: The new allocated tree node.
 */ 
tree_node * create_tree_node(const char * name, void * ptr);

/**
 * @brief Looks up for a node in the binary search tree that has a specific name.
   @param root: The pointer to the root the binary search tree.
   @param name: The name (aka key) of the node we are looking for.
   @return: The pointer to the founded node. In case of not existing, returns NULL.
 */ 
tree_node * tree_lookup(tree_node * root,const char * name);
