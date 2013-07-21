#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

typedef struct tree_node{
	char * name;
	struct tree_node * lc;
	struct tree_node * rc;
	void * data_ptr;
}tree_node;

void print_tree(tree_node * root);

tree_node * tree_insert_node(tree_node * root, char * name, void * data_ptr, tree_node ** found);

tree_node * create_tree_node(char * name, void * ptr);

tree_node * tree_lookup(tree_node * root, char * name);