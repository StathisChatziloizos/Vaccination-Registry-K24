#ifndef _SKIP_LIST_
#define _SKIP_LIST_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUM_OF_NEXT_NODES 20

#include "hash.h"

// Komvos skip listas
typedef struct skip_node{
	
	// To citizen id toy citizen
	int citizen_id;
	
	// Deikths se record
	record* record_ptr;
	
	// Hmeromhnia
	char date[11];
	
	// To plithos twn epomenwn kombwn
	int num_of_next_nodes;
	
	// Pinakas deiktwn stoys epomenoys
	struct skip_node** next_nodes;
	
}skip_node;

// Eisagwgi komvou 
void SKIP_LIST_insert(skip_node** sk_node,int citizen_id,record* record_ptr,char* date);

// Anazitisi komvoy
skip_node* SKIP_LIST_search(skip_node* sk_node, int citizen_id);

// Emfanish oloklhrhs listas
void SKIP_LIST_print(skip_node* sk_node);

#endif
