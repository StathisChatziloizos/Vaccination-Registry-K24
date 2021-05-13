#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


#include "hash.h"
#include "bst.h"
#include "bloom.h"
#include "skip_list.h"

#define INNER_MENU_LINE_LENGTH 200
#define FILE_LINE_LENGTH 200
#define NUM_OF_BUCKETS 50
#define BLOOM_STRING_MAX_LENGTH 50

#define BUFFER_SIZE 512 // For named PIPE

// char* fifo1 = "simpleFifo";
// char* fifo2 = "simpleFifo2";

const int pipeSize = 512;

int main(int argc, char** argv)
{
	char* fifo1 = argv[1];
	char* fifo2 = argv[2];
    // ----------------------------------------------------------------------
    // Pedia eggrafwn
	char* citizen_id;
	char* first_name;
	char* last_name;
	char* country_name;
	char* age;
	char* virus_name;
	char* vaccination_condition;
	char* date;
	
	// To string poy ginetai hash sto bloom filter
	unsigned char bloom_string[BLOOM_STRING_MAX_LENGTH];
	
	// Hmeromhnies anazhthshs
	char* start_date;
	char* end_date;
	
	// Deikths arxeioy eggrafwn
	FILE* fp;
	
	// To megethos bloom filtroy
	unsigned long bloom_size;
		
	// Entolh sto eswteriko menoy
	char* command_name;

	// Eswterikh grammh entolwn
	char inner_menu_line[INNER_MENU_LINE_LENGTH];	
	
	// Grammh arxeioy
	char file_line[FILE_LINE_LENGTH];		
	
	// Pinakas katakermatismoy twn citizen
	hash_table record_hash_table;
	
	// To dentro twn iwn
	Tree virus_tree;
	
	// Deikths se skip list node
	skip_node* skip_lst_node;
	
	// Deikths se io
	node* virus_node;
	
	// Deikths se eggrafh citizen
	record* citizen_record;

	// Deiktis se country node
	country_node* head=NULL;
	
	// To bloom filtro 
	Bloom bloom;
	
	// Boh8htikh metablhth
	unsigned int i;
	
	// Boh8htikh metablhth
	unsigned long bit_position;
	
	// Pinakas iwn (kataskeyasmenos apo to antistoixo dentro)
	node** array_of_viruses;
		
	// Metrhths iwn
	int num_of_viruses=0;
	
	int num_of_buckets;
		
	num_of_buckets = NUM_OF_BUCKETS;

	// Anoigma arxeioy ka8orismos mege8ous bloom filtroy
		
		fp=fopen("input_dir/Greece/inputFile","r");
		bloom_size=10000;
	
	// Elegxos anoigmatos arxeioy
	if(fp==NULL)
	{
		
		printf("Open file error\n");
		return 0;
		
	}
		
	// Arxikopoihsh toy pinaka katakermatismoy twn citizen
	HASH_init(&record_hash_table,num_of_buckets);
	
	// Arxikopoihsh toy dentroy twn iwn filtrou
	BST_init(&virus_tree);
	
	// Arxikopoihsh toy bloom filtrou
	BLOOM_init(&bloom,bloom_size);
	
	while(fgets(file_line,FILE_LINE_LENGTH-1,fp)!=NULL)
	{
		char current_line[200];
		strcpy(current_line,file_line);		
		
		citizen_id=strtok(file_line," \t\n");
		first_name=strtok(NULL," \t\n");
		last_name=strtok(NULL," \t\n");
		country_name=strtok(NULL," \t\n"); 
		age=strtok(NULL," \t\n"); 
		virus_name=strtok(NULL," \t\n");
		vaccination_condition=strtok(NULL," \t\n"); 
		date=strtok(NULL," \t\n");

		// An to record den iparxei tote prosthetei ton asthenh sto country list
		if(HASH_exists(&record_hash_table, citizen_id)==NULL)
			COUNTRIES_list_insertLast(&head,country_name, atoi(age));

		citizen_record=HASH_add_patient(&record_hash_table,current_line);

		
		virus_node=BST_search(virus_tree,virus_name);			
		
		if(virus_node==NULL)
		{
			virus_node=BST_insert(&virus_tree,virus_name);	
			num_of_viruses++;		
		}
		
		// Ean prokeitai gia eggrafh emvoliasmenoy eisagetai sto bloom filter kai sthn antistoixh lista emvoliasmenwn
		if(!strcmp(vaccination_condition,"YES"))
		{	
			
			for(i=0;i<16;i++)
			{
				// Kataskeyh string poy tha ginei hash
				sprintf(bloom_string,"%s%s",citizen_id,virus_name);
					
				// Evresh theshs poy tha ginei set
				bit_position=BLOOM_hash(bloom_string,i);
				
				BLOOM_insert(&bloom,bit_position);
			}
							
			SKIP_LIST_insert(&(virus_node->vacc_list),atoi(citizen_id),citizen_record,date);
						
		}
		// Diaforetika exoyme eisagwgh monaxa sth non vaccinated list
		else
			SKIP_LIST_insert(&(virus_node->not_vacc_list),atoi(citizen_id),citizen_record,NULL);	
			
	}

	// Exontas diavasei olous toys ioys, apo to dentro paragoyme antistoixo pinaka deiktwn sta onomata toys
	// Boliko gia seiriakh prospelash
	array_of_viruses=malloc(num_of_viruses*sizeof(node*));
	
	BST_fill_array(virus_tree,array_of_viruses);
    // ----------------------------------------------------------------------


    int fd1, fd2, nwrite;
    char msgbuf[BUFFER_SIZE+1];

    //if(mkfifo(fifo,0666) == -1)     {perror("mkfifo");    return -1;}

    if((fd1 = open(fifo1, O_WRONLY)) < 0 )     {perror("Open fifo1exec");    return -1;}

    // if((fd2 = open(fifo2, O_RDWR)) < 0 )     {perror("Open fifo2exec");    return -1;}

    // while(1)
    // {
		char* message;
        // scanf("%s",msgbuf);
        //strcpy(msgbuf, argv[1]);
		int writeBytes = strlen(bloom.filter) +1;

		//printf("%s", msgbuf);
		//int writeBytes = strlen(msgbuf +1);

        // printf("Write strlen = %d\n", writeBytes);
		if((nwrite = write(fd1,&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
		if((nwrite = write(fd1,bloom.filter, bloom.size)) == -1)    {perror("write");   return -1;}
		// if((nwrite = write(fd1,&msgbuf, strlen(msgbuf) +1)) == -1)    {perror("write");   return -1;}
        // if((nwrite = write(fd1,&bloom, sizeof(bloom))) == -1)    {perror("write");   return -1;}
		// sleep(2);
        // if(strcmp(msgbuf,"exit") == 0)      break;
    // }

    close(fd1);
    close(fd2);
	BLOOM_destroy(&bloom);

	printf("Exiting child!\n");
    return 0;
}