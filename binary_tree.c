#include "binary_tree.h"

/**
 * @brief Inserts a new node to the binary search tree.
   @param root: The root pointer of the binary search tree.
   @param name: The name (aka key) of the tree node.
   @param data_ptr: A pointer to the data that will be copied to the new node.
   @param found: A tree node, in case of not NULL the value of it points
   to the new allocated node.
   @return: The new binary search tree.
 */ 
tree_node * tree_insert_node(tree_node * root,const char * name, void * data_ptr, tree_node ** found){
	tree_node * new_node = create_tree_node(name,data_ptr);
	tree_node * visitor = root;
	tree_node * previous = NULL;
	char is_left_child = 0;
	short int comparison;

	if(!new_node)
		return root;

	while(visitor){
		previous = visitor;
		comparison = strcmp(name,visitor->name);
		if(comparison==0){
			if(found)
				*found = visitor;
			return root;
		}
		else if(comparison>0){
			visitor = visitor->rc;
			is_left_child = 0;
		}
		else{
			visitor = visitor->lc;
			is_left_child = 1;
		}
	}

	if(root==NULL)
		root = new_node;
	else {
		if(is_left_child)
			previous->lc = new_node;
		else
			previous->rc = new_node;
	}

	return root;
}

/**
 * @brief Looks up for a node in the binary search tree that has a specific name.
   @param root: The pointer to the root the binary search tree.
   @param name: The name (aka key) of the node we are looking for.
   @return: The pointer to the founded node. In case of not existing, returns NULL.
 */ 
tree_node * tree_lookup(tree_node * root,const char * name){
	tree_node * visitor = root;
	short int comparison;

	while(visitor){
		comparison = strcmp(name,visitor->name);
		if(comparison==0)
			break;
		else if(comparison>0)
			visitor = visitor->rc;
		else
			visitor = visitor->lc;
	}	
	return visitor;
}

/**
 * @brief Creates a new tree node (by doing the required allocations).
   @param name: The name of the node.
   @param ptr: The pointer to the data that will be copied to the node.
   @param data_ptr: A pointer to the data that will be copied to the new node.
   @return: The new allocated tree node.
 */ 
tree_node * create_tree_node(const char * name, void * data_ptr){
	tree_node * new_node = (tree_node *)malloc(sizeof(tree_node));

	if(memerror(new_node,"new tree node"))
		return NULL;

	new_node->name = malloc(strlen(name)+1);
	if(memerror(new_node->name,"new tree node name")){
		free(new_node);
		return NULL;
	}

	strcpy(new_node->name,name);
	new_node->data_ptr = data_ptr;
	new_node->lc = NULL;
	new_node->rc = NULL;

	return new_node;
}