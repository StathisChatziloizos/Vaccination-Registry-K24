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

// #define BUFFER_SIZE 2

// char* fifo1 = "simpleFifo";
// char* fifo2 = "simpleFifo2";

const int pipeSize = 512;

int main(int argc, char** argv)
{
    int fd1, fd2, nwrite, readBytes, writeBytes, numCountries;
	unsigned int buffer_size;
    // char msgbuf[BUFFER_SIZE+1];

	// To megethos bloom filtroy
	unsigned long bloom_size;

	char* fifo1 = argv[1];
	char* fifo2 = argv[2];
	// printf("Monitor: fifo1 = %s, fifo2 = %s\n", fifo1, fifo2);
	char** subdirectory;

	if((fd2 = open(fifo2, O_RDONLY)) < 0 )     {perror("Open fifo2");    return -1;}
	if(read(fd2, &buffer_size, sizeof(int)) < 0)     {perror("read");    return -1;}
	if(read(fd2, &numCountries, sizeof(int)) < 0)     {perror("read");    return -1;}
	subdirectory = (char**)malloc(numCountries * sizeof(char*));
	for (int i = 0; i < numCountries; i++)
	{
		if(read(fd2, &readBytes, sizeof(int)) < 0)     {perror("read");    return -1;}
		subdirectory[i] = (char*)malloc(readBytes);
		if(read(fd2, subdirectory[i], readBytes) < 0)     {perror("read");    return -1;}		// Lhpsh subdirectory
		// printf("Subdirectory[%d]: %s\n",i,subdirectory[i]);
	}
	if(read(fd2, &bloom_size,sizeof(unsigned long)) <0)     {perror("read");    return -1;}		// Lhpsh bloom_size
	close(fd2);

	// printf("Buffer Size: %d, numCountries: %d\n",buffer_size, numCountries);

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

	unsigned long offset = 0;

	// Anoigma arxeioy ka8orismos mege8ous bloom filtroy
		
		unsigned char file[50];
		strcpy(file,subdirectory[0]);
		strcat(file, "/inputFile");
		fp=fopen(file,"r");

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

	fclose(fp);

	// Exontas diavasei olous toys ioys, apo to dentro paragoyme antistoixo pinaka deiktwn sta onomata toys
	// Boliko gia seiriakh prospelash
	array_of_viruses=malloc(num_of_viruses*sizeof(node*));
	
	BST_fill_array(virus_tree,array_of_viruses);
    // ----------------------------------------------------------------------


    // if((fd2 = open(fifo2, O_RDWR)) < 0 )     {perror("Open fifo2exec");    return -1;}

    // while(1)
    // {
	char* message;
        // scanf("%s",msgbuf);
        //strcpy(msgbuf, argv[1]);
	writeBytes = strlen(bloom.filter) +1;

		//printf("%s", msgbuf);
		//int writeBytes = strlen(msgbuf +1);

        // printf("Write strlen = %d\n", writeBytes);
    if((fd1 = open(fifo1, O_WRONLY)) < 0 )     {perror("Open fifo1exec");    return -1;}

	// printf("CHILD fd: %d\n", fd1);
	if((nwrite = write(fd1,&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}

	// printf("CHILD fd: %d\n", fd1);

	while(offset < bloom_size)
	{
		if((nwrite = write(fd1,bloom.filter + offset, buffer_size)) == -1)	{perror("write");   return -1;}
		offset += nwrite;
		// printf("nwrite = %d, offset = %d\n", nwrite, offset);

	}
	close(fd1);



	// if((nwrite = write(fd1,bloom.filter, bloom.size)) == -1)    {perror("write");   return -1;}

	


		// if((nwrite = write(fd1,&msgbuf, strlen(msgbuf) +1)) == -1)    {perror("write");   return -1;}
        // if((nwrite = write(fd1,&bloom, sizeof(bloom))) == -1)    {perror("write");   return -1;}
		// sleep(2);
        // if(strcmp(msgbuf,"exit") == 0)      break;
    // }

    close(fd1);
    close(fd2);
	BLOOM_destroy(&bloom);
	free(array_of_viruses);
	for (int i = 0; i < numCountries; i++)
	{
		free(subdirectory[i]);
	}
	
	free(subdirectory);

	
	printf("Exiting child!\n");
    return 0;
}