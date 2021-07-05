#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUM_OF_NEXT_NODES 20

#include "skip_list.h"

// Eisagwgi komvou 
void SKIP_LIST_insert(skip_node** sk_node,int citizen_id,record* record_ptr,char* date)
{
	
	int i;	

	int layer;
	
	skip_node* new_node;
	
	skip_node* current_node;	

	// An okombos einai kenos shmainei oti dhmioyrgeitai lista
	if(*sk_node==NULL)
	{

		*sk_node=malloc(sizeof(skip_node));
		
		// Ekxwreitai to id tou citizen
		(*sk_node)->citizen_id=citizen_id;
		
		// Ekxwreitai to deikths pros to rekord toy citizen
		(*sk_node)->record_ptr=record_ptr;
		
		// O prwtos komvos exei panta megisto ypsos
		(*sk_node)->num_of_next_nodes=MAX_NUM_OF_NEXT_NODES;
		
		// Ekxwreitai h hmeromhnia 
		if(date==NULL)			
			strcpy((*sk_node)->date,"");
		else
			strcpy((*sk_node)->date,date);
		
		// Desmeyetai xwros gia toys deiktes pros toyw epomenoys kombous
		(*sk_node)->next_nodes=malloc(MAX_NUM_OF_NEXT_NODES*sizeof(skip_node*));
		
		// Oi deiktes aytoi ginontai NULL
		for(i=0;i<MAX_NUM_OF_NEXT_NODES;i++)
			(*sk_node)->next_nodes[i]=NULL;
		
	}
	// Diaforetika an to citizen_id einai mikrotero toy prwtoy prepei na ginei eisagwgh sthn arxh
	else if(citizen_id<(*sk_node)->citizen_id)
	{
		
		new_node=malloc(sizeof(skip_node));
		
		
		// Ekxwreitai to id tou citizen
		new_node->citizen_id=citizen_id;
		
		// Ekxwreitai to deikths pros to rekord toy citizen
		new_node->record_ptr=record_ptr;
		
		// O prwtos komvos exei panta megisto ypsos
		new_node->num_of_next_nodes=MAX_NUM_OF_NEXT_NODES;
		
		// Ekxwreitai h hmeromhnia 
		if(date==NULL)			
			strcpy(new_node->date,"");
		else
			strcpy(new_node->date,date);
		
		// Desmeyetai xwros gia toys deiktes pros toyw epomenoys kombous
		new_node->next_nodes=malloc(MAX_NUM_OF_NEXT_NODES*sizeof(skip_node*));
		
		// Oi deiktes aytoi deixnoyn oloi ston prwhn prwto (giati o prwtos komvos exei panta megisto ypsos)
		for(i=0;i<MAX_NUM_OF_NEXT_NODES;i++)
			new_node->next_nodes[i]=(*sk_node);
		
		// O neos komvos ginetai tera prwtos
		*sk_node=new_node;
		
	}
	else
	{
		// Anazhtame th thesh eisagwghs 
		current_node=*sk_node;
		
		// Arxikopoihsh epipedoy
		layer=MAX_NUM_OF_NEXT_NODES-1;
		
		while(1)
		{
			
			// An o epomenos toy layer einai NULL...
			if(current_node->next_nodes[layer]==NULL)
			{
				
				// ... an to layer einai to mhden den mporoyme na katevoyme. Sthn oysia exoyme eisagwgh sto telos toy skip list
				if(layer==0)
				{
									
					// Sthn periptwsh ayth apla kanoyme break
					break;			
					
				}
				else
				{
				// ... diaforetika katevainoyme epipedo
					layer--;	
				}
				
			}
			// An o epomenos den einai NULL kai h timh toy einai mikroterh apo aythn poy theloyme na eisagoyme phgainoyme se ayton
			else if(current_node->next_nodes[layer]->citizen_id<citizen_id)
			{
				
				current_node=current_node->next_nodes[layer];
				
				
			}
			// An o epomeno den einai NULL kai h timh toy einai megalyterh h ish apo aythn poy theloyme na eisagoyme...
			else
			{
				
				// ... an to layer einai to mhden den mporoyme na katevoyme. Sthn oysia exoyme ftasei sth swsth thesh
				if(layer==0)
				{
					// Sthn periptwsh ayth apla kanoyme break
					break;			
					
				}
				else
				{
				// ... diaforetika katevainoyme epipedo
					layer--;	
				}
				
				
			}
			
			
		}
		
		new_node=malloc(sizeof(skip_node));
		
		
		// Ekxwreitai to id tou citizen
		new_node->citizen_id=citizen_id;
		
		// Ekxwreitai to deikths pros to rekord toy citizen
		new_node->record_ptr=record_ptr;
		
		// O komvos exei tyxaio ypsos
		new_node->num_of_next_nodes=rand()%MAX_NUM_OF_NEXT_NODES+1;
		
		// Ekxwreitai h hmeromhnia 
		if(date==NULL)			
			strcpy(new_node->date,"");
		else
			strcpy(new_node->date,date);
		
		// Desmeyetai xwros gia toys deiktes pros toyw epomenoys kombous
		new_node->next_nodes=malloc(new_node->num_of_next_nodes*sizeof(skip_node*));
		
		// O deikths vashs deixnei ekei poy edeixne o deikths vashs toy current
		new_node->next_nodes[0]=current_node->next_nodes[0];
		
		// O neos komvos mpainei mprosta apo ton current
		current_node->next_nodes[0]=new_node;
		
		// Prepei na ginoyn update oi sydeseis
		
		// Ksekiname pali apo thn arxh
		current_node=*sk_node;
				
		// Arxikopoihsh epipedoy
		layer=MAX_NUM_OF_NEXT_NODES-1;
		
		while(1)
		{

			// An o epomenos toy layer einai NULL katevainoyme epiepdo
			if(current_node->next_nodes[layer]==NULL)
			{				
				layer--;	
				
				// An ftasame sto epipedo mhden teleisame
				if(layer==0)
					break;
				
			}
			// An o epomenos den einai NULL kai h timh toy einai mikroterh apo aythn poy theloyme na eisagoyme phgainoyme se ayton
			else if(current_node->next_nodes[layer]->citizen_id<citizen_id)
			{
				
				current_node=current_node->next_nodes[layer];
				
				
			}
			// An o epomenos den einai NULL kai h timh toy einai megalyterh h ish apo aythn poy eisagame...
			else
			{
				
				// Kanoyme update th syndesh me ton no komvo
				// Ayto omws ginetai mono sthn periptwsh poy to layer den ypervainei to ypsos toy neoy komboy
				
				if(layer<new_node->num_of_next_nodes)
				{
				
					new_node->next_nodes[layer]=current_node->next_nodes[layer];
					current_node->next_nodes[layer]=new_node;
				}
				
				// Sth synexeia katevainoyme epipedo
				layer--;	
				
				// An ftasame sto epipedo mhden teleisame
				if(layer==0)
					break;
				
				
			}
			
		}
			
	}
			
}

// Anazitisi komvoy
skip_node* SKIP_LIST_search(skip_node* sk_node, int citizen_id)
{
	
	
	// Arxikopoihsh epipedoy
	int layer=MAX_NUM_OF_NEXT_NODES-1;
	
	// Anazhtame th thesh eisagwghs 
	skip_node* current_node=sk_node;
	
	// An h skip list einai adeia epistrefetai NULL
	if(current_node==NULL)
		return NULL;
	
	while(1)
	{
		
		// An o komvos brethike epistrefetai
		if(current_node->citizen_id==citizen_id)
			return current_node;
		
		// An o epomenos toy layer einai NULL...
		if(current_node->next_nodes[layer]==NULL)
		{
			
			// ... an to layer einai to mhden den mporoyme na katevoyme. H anazhthsh apotygxanei kai epistefoyme NULL
			if(layer==0)
			{
				return NULL;
				
			}
			else
			{
			// ... diaforetika katevainoyme epipedo
				layer--;	
			}
			
		}
		// An o epomenos den einai NULL kai h timh toy einai mikroterh apo aythn poy psaxnoyme phgainoyme se ayton
		else if(current_node->next_nodes[layer]->citizen_id<citizen_id)
		{
			
			current_node=current_node->next_nodes[layer];
			
			
		}
		// An o epomenos den einai NULL kai h timh toy einai ish me aythn poy psaxnoyme ton epistrefoyme
		else if(current_node->next_nodes[layer]->citizen_id==citizen_id)
		{
			
			return current_node->next_nodes[layer];
			
			
		}
		// An o epomenos den einai NULL kai h timh toy einai megalyterh aythn poy theloyme na eisagoyme...
		else
		{
			
			// ... an to layer einai to mhden den mporoyme na katevoyme. H anazhthsh apotygxanei kai epistefoyme NULL
			if(layer==0)
			{
				return NULL;	
				
			}
			else
			{
			// ... diaforetika katevainoyme epipedo
				layer--;	
			}
			
			
		}
		
		
	}
	
}

// Emfanish oloklhrhs listas
void SKIP_LIST_print(skip_node* sk_node)
{
		
	record* rec;
	
	// Vroxos ektypwshs
	while(sk_node!=NULL)
	{
		rec=sk_node->record_ptr;
		
		printf("%d %s %s %s %d\n",sk_node->citizen_id,rec->first_name,rec->last_name,rec->country_name,rec->age);
 
		// H diasxish ginetai sto epipedo vashs
		sk_node=sk_node->next_nodes[0];
		
	}
	printf("\n");		
	
}
