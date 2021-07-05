#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

// H synarthsh arxikopoiei enan pinaka katakermatismou 
int HASH_function(hash_table* h_table,char* citizen_id)
{
	
	// A8roisma katakermatismoy
	int S=0;
	
	// Ari8modeikths
	int i;
	
	int length=strlen(citizen_id);
	
	for(i=0;i<length;i++)
		S+=citizen_id[i];
	
	return S%h_table->num_of_buckets;

}

// H synarthsh arxikopoiei enan komvo country_node kai ton epistrefei
country_node* COUNTRIES_init(char* country_name)
{
	country_node* newNode;
	newNode=(country_node*)malloc(sizeof(country_node));
	strcpy(newNode->country_name,country_name);
	newNode->age0=0;

	// Plh8ismos 20-40
	newNode->age20=0;
	
	// Plh8ismos 20-40
	newNode->age40=0;
	
	// Plh8ismos 40-60
	newNode->age60=0;

	// Epomenos komvos NULL
	newNode->next_node=NULL;

	return newNode;
}

// Ayksanei ton metrhth enos komvoy country_node
void COUNTRIES_add(country_node* node, int age)
{

	if(age<=20)
		node->age0++;
	else if(age<=40)
		node->age20++;
	else if(age<=60)
		node->age40++;
	else if(age<=120)
		node->age60++;

}

// Psaxnei se mia lista apo komvoys country node kai epistrefei ayton me to swsto
// onoma ths xwras. An den bre8ei epistrefei ton teleftaio kai enhmerwnei thn shmaia is_last
country_node* COUNTRIES_search(country_node* head,char* country_name, int* is_last)
{
	*is_last=0;
	country_node *current, *previous;
	current=head;
	while(current!=NULL)
	{
		if(strcmp(current->country_name,country_name)==0)
			return current;
		else
		{
			previous=current;
			current=current->next_node;
		}

	}
	*is_last=1;
	return previous;
}

// Psaxnei ton komvo me th swsth xwra kai ayksanei ton metrhth me bash thn hlikia
// An den brethei ftiaxnei enan kainoyrgio komvo kai ton prosarta sto telos ths listas
void COUNTRIES_list_insertLast(country_node** head, char* country_name, int age)
{
	if(*head==NULL)
	{
	// An h lista einai adeia
		*head=COUNTRIES_init(country_name);
		COUNTRIES_add(*head, age);
	}
	else
	{
		int is_last;
		country_node* current;
		current=COUNTRIES_search(*head, country_name, &is_last);
		if(!is_last)
			COUNTRIES_add(current, age);
		else
		{
			current->next_node=COUNTRIES_init(country_name);
			COUNTRIES_add(current->next_node, age);
		}
	}
}

// Voi8htikh synarthsh poy typwnei thn lista apo xwres
void COUNTRIES_print_list(country_node* head)
{
	if(head==NULL)
		return;
	country_node* current=head;
	while(current!=NULL)
	{
		int population=current->age0+current->age20+current->age40+current->age60;
		printf("%s (0-20: %d)(20-40: %d)(40-60: %d)(60+: %d)\tPOPULATION: %d\n", current->country_name,current->age0,current->age20,current->age40,current->age60, population);
		current=current->next_node;
	}
}

// Diagrafei toys komvoys ths listas kai eleftherwnei thn mnhmh toys
void COUNTRIES_delete_list(country_node* head)
{
	if(head==NULL)
		return;
	country_node* current;
	while(current!=NULL)
	{
		country_node* temp=current;
		current=current->next_node;
		free(temp);
	}
}



// H synarthsh dhmiourgei kai epistrefei enan pinaka katakermatismou 
void HASH_init(hash_table* h_table, int num_of_buckets)
{
	
	// Ari8modeikths
	int i;
	
	h_table->num_of_buckets=num_of_buckets;
	
	h_table->buckets=malloc(num_of_buckets*sizeof(bucket));
	
	// Arxikopoihsh twn buckets
	for(i=0;i<num_of_buckets;i++)
	{
		
		h_table->buckets[i].num_of_records=0;
		h_table->buckets[i].first_record=NULL;
		
	}		
	
}


// Eisagei enan asthenh ston pinaka katakermatismoy
record* HASH_add_patient(hash_table* h_table,char* patient)
{

	// Proswrinos pinakas
	char patient_full_data[128];
	
	// Proswrinos pinakas
	char citizen_id[32];
	
	// Proswrinos pinakas
	char first_name[32];
	
	// Proswrinos pinakas
	char last_name[32];
	
	// Proswrinos pinakas
	char country_name[32];
	
	// Proswrinh metablhth
	int age;
				
	// Eyresh kadoy poy prepei na mpei h nea eggrafh
	int num_of_bucket;
	
	// O deikths poy 8a epistrafei
	record* ret_record;
	
	// Antloyme to citizen_id
	sscanf(patient,"%s",citizen_id);
	
	// Anazhtoyme thn eggrafh toy sygkekrimenoy citizen_id
	ret_record = HASH_exists(h_table,citizen_id);
	
	// Efoson yparxei epistrefetai
	if(ret_record)
		return ret_record;
	
	// Efoson den yparxei ypologizetai o kados poy 8a mpei
	num_of_bucket=HASH_function(h_table,citizen_id);
	
	// Antigrafh stoixeiwn asthenoys
	strncpy(patient_full_data,patient,127);
	
	// Antloyme ola ta stoixeia ths eggrafhs 
	sscanf(patient_full_data,"%s %s %s %s %d",citizen_id,first_name,last_name,country_name,&age);
	
	// Ean h hlikia einai akyrh, epistrefetai NULL
	if(age<=0)
		return NULL;
	
	if(age>120)
		return NULL;
		
	// Desmeysh mnhmhs gia neo bucket_record
	bucket_record* new_record=malloc(sizeof(bucket_record));
	
	// Desmeysh mnhmhs gia neo record
	new_record->patient_record=malloc(sizeof(record));
	
	
	// Desmeysh mnhmhs gia to citizen_id
	new_record->patient_record->citizen_id=malloc(strlen(citizen_id)+1);
	
	// Desmeysh mnhmhs gia to first_name
	new_record->patient_record->first_name=malloc(strlen(first_name)+1);
	
	// Desmeysh mnhmhs gia to last_name
	new_record->patient_record->last_name=malloc(strlen(last_name)+1);
	
	// Desmeysh mnhmhs gia to country_name
	new_record->patient_record->country_name=malloc(strlen(country_name)+1);
	
	// Antigrafh citizen_id
	strcpy(new_record->patient_record->citizen_id,citizen_id);
	
	// Antigrafh first_name
	strcpy(new_record->patient_record->first_name,first_name);
	
	// Antigrafh last_name
	strcpy(new_record->patient_record->last_name,last_name);
	
	// Antigrafh country_name
	strcpy(new_record->patient_record->country_name,country_name);
	
	// Antigrafh age
	new_record->patient_record->age=age;
		
	// O next toy neoy record deixnei ston ews twra prwto record toy bucket
	new_record->next=h_table->buckets[num_of_bucket].first_record;
	
	// To neo record mpainei prwto sto bucket
	h_table->buckets[num_of_bucket].first_record=new_record;
	
	h_table->buckets[num_of_bucket].num_of_records++;
	
	return new_record->patient_record;
		
}

// Epistrefei to record enos asthenh an yparxei ston pinaka katakermatismoy, diaforetika epistrefei NULL
record* HASH_exists(hash_table* h_table,char* citizen_id)
{
		
	// Eyresh kadoy poy 8a prepei na vrisketai to page
	int num_of_bucket=HASH_function(h_table,citizen_id);
	
	// Ari8mos records toy bucket
	int num_of_records=h_table->buckets[num_of_bucket].num_of_records;
	
	// Ari8modeikths
	int i;
	
	// Deikths eggrafhs (arxikopoihsh sthn prwth eggrafh toy bucket)
	bucket_record* rcd;
	
	if(num_of_records==0)
		return NULL;
	
	rcd=h_table->buckets[num_of_bucket].first_record;
	
	// Sarwnontai oi eggrafes  toy bucket kai an vre8ei to page epistrefetai 1
	for(i=0;i<num_of_records;i++)
	{
		
		if(!strcmp(rcd->patient_record->citizen_id,citizen_id))
			return rcd->patient_record;
		
		rcd=rcd->next;
		
	}
	
	// Epistrefetai NULL
	
	return NULL;
		
}


// Diagrafei ena page apo ton pinaka katakermatismoy
void HASH_delete(hash_table* h_table,char* citizen_id)
{
	
	// Eyresh kadoy poy 8a prepei na vrisketai to page
	int num_of_bucket=HASH_function(h_table,citizen_id);
	
	// Ari8mos records toy bucket
	int num_of_records;
	
	// Ari8modeikths
	int i;
	
	// Deikths eggrafhs (arxikopoihsh sthn prwth eggrafh toy bucket)
	bucket_record* rcd=h_table->buckets[num_of_bucket].first_record;
	
	// Boh8htikos deikths
	bucket_record* del_rcd;
	
	// An prepei na diagraftei h prwth eggrafh apla diagrafetai
	if(!strcmp(rcd->patient_record->citizen_id,citizen_id))
	{
		
		h_table->buckets[num_of_bucket].first_record=rcd->next;
		
		free(rcd->patient_record->citizen_id);
		free(rcd->patient_record->first_name);
		free(rcd->patient_record->last_name);
		free(rcd->patient_record->country_name);
		
		free(rcd->patient_record);
		
		free(rcd);
		
	}
	else
	{
		
		num_of_records=h_table->buckets[num_of_bucket].num_of_records;
		
		// Diaforetika sarwnontai oi ypoloipes eggrafes oi eggrafes kai diagrafetai ayth poy prepei
		for(i=0;i<num_of_records-1;i++)
		{
			
			//  O deikths rcs deixnei sthn prohgoymenh ths eggrafhs poy prepei na diagraftei
			if(!strcmp(rcd->next->patient_record->citizen_id,citizen_id))
			{
				
				// Apo8hkeyoyme proswrina th diey8ysnh ths egrrafhs poy prepei na diagraftei ston deikth del_rcd
				del_rcd=rcd->next;
				
				// "Denoyme" thn prohgoymenh ths eggrafhs poy prepei na diagraftei, me thn epomenh				
				rcd->next=rcd->next->next;
				
				// Diagrafoyme thn eggrafh
				free(del_rcd->patient_record->citizen_id);
				free(del_rcd->patient_record->first_name);
				free(del_rcd->patient_record->last_name);
				free(del_rcd->patient_record->country_name);
				
				free(del_rcd->patient_record);
				
				free(del_rcd);	
				
				break;
				
			}						
		}
	}
	
	// To plh8os eggrafwn toy kadoy meiwnetai kata 1
	h_table->buckets[num_of_bucket].num_of_records--;
	
}

// Apeley8erwsh mnhmhs hash table
void HASH_clear(hash_table* h_table)
{
		
	// Ari8mos kadwn
	int num_of_buckets=h_table->num_of_buckets;
		
	// Deiktes ka8arismoy eggrafwn
	bucket_record* current;
	bucket_record* next;
	
	// Ari8mos records toy bucket
	int num_of_records;	
	
	// Arithmodeiktes
	int i,j;
		
	// Gia ka8e kado
	for(i=0;i<num_of_buckets;i++)
	{
			
		// Ypologizetai to plh8os eggrafwn			
		num_of_records=h_table->buckets[i].num_of_records;
		
		// Arxikopoioyntai oi deiktes current kai next
		current=h_table->buckets[i].first_record;
		
		if(current!=NULL)
			next=current->next;
		
		// Kai telos sarwnetai o kados kai diagrafontai oi eggrafes
		for(j=0;j<num_of_records;j++)
		{
			
			next=current->next;
			
			free(current->patient_record->citizen_id);
			free(current->patient_record->first_name);
			free(current->patient_record->last_name);
			free(current->patient_record->country_name);
			
			free(current->patient_record);
			
			free(current);
					
			current=next;
			
		}		
	}			
}

// Emfnish table
int HASH_print(hash_table* h_table)
{
		
	// Ari8mos kadwn
	int num_of_buckets=h_table->num_of_buckets;
		
	// Deikths emfanishs eggrafhs
	bucket_record* current;
	
	// Ari8mos records toy bucket
	int num_of_records;	
	
	// Arithmodeiktes
	int i,j;
		
	// Metrhths
	int count=0;
		
	// Gia ka8e kado
	for(i=0;i<num_of_buckets;i++)
	{
			
		// Ypologizetai to plh8os eggrafwn			
		num_of_records=h_table->buckets[i].num_of_records;
		
		// Arxikopoieitai o deikths current
		current=h_table->buckets[i].first_record;
		
		printf("Bucket-%d: \n",i);
		
		// Kai telos sarwnetai o kados kai diagrafontai oi eggrafes
		for(j=0;j<num_of_records;j++)
		{
			
			// Diaforetika typwnoyme to kleidi kai proxwrame sthn epomenh eggrafh
			printf("%s ",current->patient_record->citizen_id);
			printf("%s ",current->patient_record->first_name);
			printf("%s ",current->patient_record->last_name);
			printf("%s ",current->patient_record->country_name);
			printf("%d\n",current->patient_record->age);
						
			count++;
			
			current=current->next;
			
		}	

	}
	
	printf("Number of records: %d\n",count);
		
}
