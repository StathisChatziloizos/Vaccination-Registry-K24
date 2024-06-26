#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>


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

static int flag = 0;

void int_quit_children()
{
	flag=1;
	printf(" -- INT/QUIT RECEIVED MONITOR!\n");
}

const int pipeSize = 512;

int main(int argc, char** argv)
{
	signal(SIGINT,int_quit_children);
	signal(SIGQUIT,int_quit_children);
    int fd1, fd2, nwrite, readBytes, writeBytes, numCountries;
	unsigned int buffer_size;

	char* directoryName;
    DIR* dir_ptr;
    struct dirent* dir;
    // char msgbuf[BUFFER_SIZE+1];

	// To megethos bloom filtroy
	unsigned long bloom_size;

	char* fifo1 = argv[1];
	char* fifo2 = argv[2];
	// printf("Monitor: fifo1 = %s, fifo2 = %s\n", fifo1, fifo2);
	char** subdirectory;
	char command[20];

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

	char*** files = (char***)malloc(numCountries * sizeof(char**));

	int* counter = (int*)malloc(numCountries * sizeof(int));
	// int max_counter=0;
	for (int i = 0; i < numCountries; i++)
	{
		counter[i]=0;
		files[i] = (char**)malloc(40 * sizeof(char*));
		// printf("Subdirectory[%d]: %s\n",i,subdirectory[i]);
		if((dir_ptr = opendir(subdirectory[i])) == NULL) 		{perror("Open Dir"); 	return -1;}
		while((dir = readdir(dir_ptr)) != NULL )
		{
			if(strcmp(dir->d_name, ".")==0 || strcmp(dir->d_name, "..")==0)
				continue;
			files[i][counter[i]] = (char*)malloc(strlen(dir->d_name)+1);
			strcpy (files[i][counter[i]],dir->d_name);
			counter[i]++;
		}
		// if(counter > max_counter)
		// 	max_counter = counter;
		closedir(dir_ptr);
	}
	// printf("max counter: %d", max_counter);

	// for (int i = 0; i < numCountries; i++)
	// {
	// 	for (int  j = 0; j < counter[i]; j++)
	// 	{
	// 		if(files[i][j] == NULL)
	// 			continue;
	// 		printf("files[%d][%d] = %s/%s\n", i, j, subdirectory[i],files[i][j]);
	// 	}
	// }


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
		

		
	// Arxikopoihsh toy pinaka katakermatismoy twn citizen
	HASH_init(&record_hash_table,num_of_buckets);
	
	// Arxikopoihsh toy dentroy twn iwn filtrou
	BST_init(&virus_tree);
	
	// Arxikopoihsh toy bloom filtrou
	BLOOM_init(&bloom,bloom_size);

	unsigned char file[50];
	for (int j = 0; j < numCountries; j++)
	{
		for (int  z = 0; z < counter[j]; z++)
		{
				if(files[j][z] == NULL)
					continue;
			strcpy(file,subdirectory[j]);
			strcat(file, "/");
			strcat(file,files[j][z]);
			// printf("file: %s\n",file);
			fp=fopen(file,"r");

			// Elegxos anoigmatos arxeioy
			if(fp==NULL)
			{
				
				printf("Open file error\n");
				return 0;
				
			}
			
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
		}
	}

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

	int accepts=0;
	int rejects=0;

	while (!flag)
	{
		if((fd2 = open(fifo2, O_RDONLY)) < 0 )     {perror("Open fifo2-Monitor");    return -1;}
		if(read(fd2, &readBytes, sizeof(int)) < 0)     {perror("read");    return -1;}
		if (readBytes == -1)
		{
			accepts ++;
			close(fd2);
			usleep(1);
			continue;
		}
		else if(readBytes == -2)
		{
			rejects ++;
			usleep(1);
			close(fd2);
			// printf("RejectsM - pid %d\n",getpid());
			continue;
		}
		if(read(fd2, command, readBytes) < 0)     {perror("read");    return -1;}		// Lhpsh subdirectory
	


		


		// printf("here\n");
		// close(fd2);
		// fflush(stdout);


		if (strcmp(command,"softExit")==0)
		{
			// printf("Exit - PID = %d - FIFO %s - ID %d\n",getpid(),fifo2, fd2);
			break;
		}
		else if(strcmp(command,"searchVaccinationStatus")==0)
		{
			// close(fd2);
			// if((fd2 = open(fifo2, O_RDONLY)) < 0 )     {perror("Open fifo2-Monitor");    return -1;}


			// printf("SearchVacc - PID = %d - FIFO %s - ID %d\n",getpid(),fifo2, fd2);
			if(read(fd2, &readBytes, sizeof(int)) < 0)     {perror("read");    return -1;}
			if(read(fd2, citizen_id, readBytes) < 0)     {perror("read");    return -1;}		// Lhpsh subdirectory
			close(fd2);

			// printf("CID %s - PID = %d\n",citizen_id, getpid());
			record* requested_citizen;
			if((requested_citizen=HASH_exists(&record_hash_table, citizen_id))!=NULL)
			{
				// printf("CitizenID %s is here\n",citizen_id);
				



				writeBytes = strlen(requested_citizen->first_name) +1;
				// Apostolh first_name
				if((fd1 = open(fifo1, O_WRONLY)) < 0 )     {perror("Open fifo1exec");    return -1;}
				if((nwrite = write(fd1,&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
				if((nwrite = write(fd1,requested_citizen->first_name, writeBytes)) == -1)    {perror("write");   return -1;}

				// Apostolh last_name
				writeBytes = strlen(requested_citizen->last_name) +1;
				if((nwrite = write(fd1,&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
				if((nwrite = write(fd1,requested_citizen->last_name, writeBytes)) == -1)    {perror("write");   return -1;}

				// Apostolh xwras
				writeBytes = strlen(requested_citizen->country_name) +1;
				if((nwrite = write(fd1,&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
				if((nwrite = write(fd1,requested_citizen->country_name, writeBytes)) == -1)    {perror("write");   return -1;}

				// Apostolh hlikias
				if((nwrite = write(fd1,&requested_citizen->age, sizeof(int))) == -1)    {perror("write");   return -1;}

				// Apostolh ari8moy iwn
				if((nwrite = write(fd1,&num_of_viruses, sizeof(int))) == -1)    {perror("write");   return -1;}

				// // Anazhthsh stis skil lists olwn twn iwn
				for(i=0;i<num_of_viruses;i++)
				{
					unsigned short reply_bool;
									
					skip_lst_node=SKIP_LIST_search(array_of_viruses[i]->vacc_list,atoi(citizen_id));
					
					// Apostolh ioy
					writeBytes = strlen(array_of_viruses[i]->virus) +1;
					if((nwrite = write(fd1,&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
					if((nwrite = write(fd1,array_of_viruses[i]->virus, writeBytes)) == -1)    {perror("write");   return -1;}

					//  Emfanizoyme to apotelesma ths anazhthshs
					if(skip_lst_node)
					{
						// Apostolh 8etikhs apanthshs emvoliasmoy
						reply_bool = 1;
						if((nwrite = write(fd1,&reply_bool, sizeof(short))) == -1)    {perror("write");   return -1;}

						// Apostolh hmeromhnias emvoliasmoy
						writeBytes = strlen(skip_lst_node->date) +1;
						if((nwrite = write(fd1,&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
						if((nwrite = write(fd1,skip_lst_node->date, writeBytes)) == -1)    {perror("write");   return -1;}


						// printf("%s YES %s\n",array_of_viruses[i]->virus,skip_lst_node->date);
						
					}
					else{
						// Apostolh arnhtikhs apanthshs emvoliasmoy
						reply_bool = 0;
						if((nwrite = write(fd1,&reply_bool, sizeof(short))) == -1)    {perror("write");   return -1;}


						// printf("%s NO\n",array_of_viruses[i]->virus);//skip_lst_node->date);
												
					}									
				}				



				close(fd1);

			}



			// printf("citizenID = %s   - PID = %d\n", citizen_id,getpid());


		}
		else if (strcmp(command,"continue")==0)
		{
			// printf("continue ivoked\n");
			continue;
		}
		else if (strcmp(command,"travelRequest")==0)
		{
			char reply[12];
			// printf("M:fd2 %d\n",fd2);
			// if((fd2 = open(fifo2, O_RDONLY)) < 0 )     {perror("Open fifo2");    return -1;}
			// printf("here\n");
			if(read(fd2, &readBytes, sizeof(int)) < 0)     {perror("read");    return -1;}
			// printf("Readbytes %d\n",readBytes);
			if(read(fd2, citizen_id, readBytes) < 0)     {perror("read");    return -1;}		// Lhpsh subdirectory
			// printf("citizenID %s\n", citizen_id);

			if(read(fd2, &readBytes, sizeof(int)) < 0)     {perror("read");    return -1;}
			if(read(fd2, virus_name, readBytes) < 0)     {perror("read");    return -1;}		// Lhpsh subdirectory
			close(fd2);

			// printf("CitizenID %s, Virus Name %s\n", citizen_id, virus_name);

			// Anazhtoyme ton komvo toy ioy sto dentro
			virus_node=BST_search(virus_tree,virus_name);
			
			// An brethei kanoyme anazhthsh sthn antistoixh skip list
			if(virus_node)
			{
				
				skip_lst_node=SKIP_LIST_search(virus_node->vacc_list,atoi(citizen_id));
				
				//  Emfanizoyme to apotelesma ths anazhthshs
				if(skip_lst_node)
				{
					strcpy(reply, "YES");
					writeBytes = strlen(reply) +1;

					if((fd1 = open(fifo1, O_WRONLY)) < 0 )     {perror("Open fifo1exec");    return -1;}
					if((nwrite = write(fd1,&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
					if((nwrite = write(fd1,reply, writeBytes)) == -1)    {perror("write");   return -1;}
					
					strcpy(reply,skip_lst_node->date);
					writeBytes = strlen(reply) +1;
					if((nwrite = write(fd1,&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
					if((nwrite = write(fd1,reply, writeBytes)) == -1)    {perror("write");   return -1;}
					close(fd1);
					
				}
				else
				{
					// Not vaccinated
					
					strcpy(reply, "NO");
					writeBytes = strlen(reply) +1;

					if((fd1 = open(fifo1, O_WRONLY)) < 0 )     {perror("Open fifo1exec");    return -1;}
					if((nwrite = write(fd1,&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
					if((nwrite = write(fd1,reply, writeBytes)) == -1)    {perror("write");   return -1;}
					close(fd1);
				}					
				
			}
			else
			{
			// Edw einai h periptwsh poy den exei brethei o ios
				printf("Can't find virus\n");						
			}
		}
	}




	if(flag)
	{
		char log_file[20];
		sprintf(log_file,"log_file.%d",getpid());
		FILE* fp = fopen(log_file,"w+");


		for (int i = 0; i < numCountries; i++)
		{
			int position=0;
			while (position < strlen(subdirectory[i]))
			{
				if (subdirectory[i][position] == '/')
				{
					break;
				}
				position++;
			}
		
			char* country=subdirectory[i]+position+1;
			fputs(country,fp);
			fputs("\n",fp);

			// printf("%s\n",country);
		}
		// for (int i = 0; i < counter; i++)
		// {
		// 	fputs(countries[i],fp);
		// 	fputs("\n",fp);

		// }
		fputs("\nTOTAL TRAVEL REQUESTS ",fp);
		fprintf(fp,"%d\n",accepts+rejects);	
		fputs("ACCEPTED ",fp);
		fprintf(fp,"%d\n",accepts);	
		fputs("REJECTS ",fp);
		fprintf(fp,"%d\n",rejects);	
		fclose(fp);
		printf("logfile: %s \n",log_file);
	}
	// if((nwrite = write(fd1,bloom.filter, bloom.size)) == -1)    {perror("write");   return -1;}


	

		// if((nwrite = write(fd1,&msgbuf, strlen(msgbuf) +1)) == -1)    {perror("write");   return -1;}
        // if((nwrite = write(fd1,&bloom, sizeof(bloom))) == -1)    {perror("write");   return -1;}
		// sleep(2);
        // if(strcmp(msgbuf,"exit") == 0)      break;
    // }

    close(fd1);
    close(fd2);
	BLOOM_destroy(&bloom);
	// printf("Here\n");
	HASH_clear(&record_hash_table);
	BST_destroy(virus_tree);
	free(array_of_viruses);
	for (int i = 0; i < numCountries; i++)
	{
		free(subdirectory[i]);
		for (int  j = 0; j < counter[i]; j++)
		{
			free(files[i][j]);
		}
		
		free(files[i]);
	}
	
	free(subdirectory);
	free(counter);
	free(files);
	
	printf("Exited Monitor R:%d - A:%d!\n",rejects,accepts);
    return 0;
}