#ifndef _BST_
#define _BST_
	
	#include "skip_list.h"
	
	#include<stdio.h>
	#include<stdlib.h>
	#include<string.h>
	
	typedef struct node{

		char* virus;
		
		skip_node* vacc_list;
		skip_node* not_vacc_list;
		
		struct node* left;
		struct node* right;
		
	}node;
	
	typedef node* Tree;
	
	// H synarthsh arxikopoiei ena dyadiko dentro 
	void BST_init(Tree* addressOfBst);
	
	// Eisagei kai epistrefei neo komvo sto dyadiko dentro 
	node* BST_insert(Tree* addressOfBst, char* virus);
	
	// Anazhta kai epistrefei komvo sto dyadiko dentro 
	node* BST_search(Tree bstTree, char* virus);

	// Anadromikh katastrofh dentroy
	void BST_destroy(Tree bstSubTree);
	
	// Anadromikh emfanish dentroy
	void BST_print(Tree bstSubTree);
	
	// Gemizei enan pinaka me toys ioys poy periexei. Oi ioi den antigrafontai. 
	// Apla ekxwroyntai ston paragomeno pinaka oi antistoixes dieythynseis
	void BST_fill_array(Tree bstSubTree, node** array);


#endif