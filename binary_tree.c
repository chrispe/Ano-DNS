#include "binary_tree.h"
 
void print_tree(tree_node * root){
	if(root->lc)
		print_tree(root->lc);	
	printf("%s\n",root->name);
	if(root->rc)
		print_tree(root->rc);
}

tree_node * tree_insert_node(tree_node * root, char * name, void * data_ptr, tree_node ** found){
	tree_node * new_node = create_tree_node(name,data_ptr);
	tree_node * visitor = root;
	tree_node * previous = NULL;
	char is_left_child = 0;
	short int comparison;

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

tree_node * tree_lookup(tree_node * root, char * name){
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

tree_node * create_tree_node(char * name, void * data_ptr){
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