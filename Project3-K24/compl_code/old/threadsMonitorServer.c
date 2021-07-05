#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <netinet/in.h>
#include <pthread.h>
#include <netdb.h>


#include "hash.h"
#include "bst.h"
#include "bloom.h"
#include "skip_list.h"

#define INNER_MENU_LINE_LENGTH 200
#define FILE_LINE_LENGTH 200
#define NUM_OF_BUCKETS 50
#define BLOOM_STRING_MAX_LENGTH 50

static int accepts = 0;
static int rejects = 0;
static int numCountries;
static char filesBuffer[1000][70];
static int filesBufferCounter = 0;
static int totalFilesEdited = 0;


typedef struct arguments{
	int filesCounter;
	unsigned int num_threads;
    hash_table record_hash_table;
    Tree virus_tree;
    int num_of_viruses;
    Bloom bloom;
}arguments;

pthread_mutex_t mtx;
pthread_mutex_t mtxBuffer;
pthread_cond_t condBuffer;

void* threadFunction(void* arg);

void add_file_to_buffer(char* file)
{
	pthread_mutex_lock(&mtxBuffer);
	strcpy(filesBuffer[filesBufferCounter],file);
	filesBufferCounter++;
	pthread_mutex_unlock(&mtxBuffer);
	pthread_cond_signal(&condBuffer);
}

// Synarthsh poy diaxeirizetai ta SIGINT/SIGQUIT twn Monitors
void int_quit_children(int argc, char** argv)
{
	// Dhmioyrgia log_file.xxx arxeioy
	char log_file[20];
	sprintf(log_file,"log_file.%d",getpid());
	FILE* fp = fopen(log_file,"w+");

	// Eisagwgh ka8e xwras sto log_file
	for (int i = 11; i < argc; i++)
	{
		int position=0;
		while (position < strlen(argv[i]))
		{
			if (argv[i][position] == '/')
			{
				break;
			}
			position++;
		}

		// Metablhth poy krata th xwra apo to subdirectory (Morfh subdirextory: input_dir/xwra)
		char* country=argv[i]+position+1;
		fputs(country,fp);
		fputs("\n",fp);
	}

	// Eisagwgh statistikwn sto log_file
	fputs("\nTOTAL TRAVEL REQUESTS ",fp);
	fprintf(fp,"%d\n",accepts+rejects);	
	fputs("ACCEPTED ",fp);
	fprintf(fp,"%d\n",accepts);	
	fputs("REJECTS ",fp);
	fprintf(fp,"%d\n",rejects);	
	fclose(fp);
	printf("MonitorServer logfile: %s \n",log_file);
}

const int pipeSize = 512;

int main(int argc, char** argv)
{
	if(argc <= 10)
	{
		printf("Usage: ./monitorServer -p port -t numThreads -b socketBufferSize -c cyclicBufferSize -s sizeOfBloom path1 path2 ... pathn");
		return -1;
	}
	if(strcmp(argv[1],"-p")!=0 || strcmp(argv[3],"-t")!=0 || strcmp(argv[5],"-b")!=0
		 || strcmp(argv[7],"-c")!=0 || strcmp(argv[9],"-s")!=0)
	{
		printf("Usage: ./monitorServer -p port -t numThreads -b socketBufferSize -c cyclicBufferSize -s sizeOfBloom path1 path2 ... pathn");
		return 0;		
	}

	arguments* thread_args = (arguments*) malloc(sizeof(arguments));

	int port = atoi(argv[2]);
	unsigned int num_threads = (unsigned int)atoi(argv[4]);
	thread_args->num_threads = num_threads;

	// ka8orismos mege8ous toy buffer twn sockets
	unsigned int socket_buff_size = (unsigned int)atoi(argv[6]);

	// ka8orismos mege8ous toy buffer
	unsigned int cyclic_buff_size = (unsigned int)(atoi(argv[8]));

	// Ka8orismos mege8ous bloom filtroy
	unsigned long bloom_size = (unsigned int)(atoi(argv[10]));


	// for (int i = 1; i < argc; i++)	// apo to 11 ksekinane ta paths
    // {
    //     printf("argv[%d] %s\n",i,argv[i]);
    // }

	// Diaxeirismos SIGINT kai SIGQUIT
	// signal(SIGINT,int_quit_children);
	// signal(SIGQUIT,int_quit_children);

    int nwrite, readBytes, writeBytes;

	// Boh8htikes metablhtes gia prospelash katalogwn
	char* directoryName;
    DIR* dir_ptr;
    struct dirent* dir;

	pthread_mutex_init(&mtx, NULL);
	pthread_mutex_init(&mtxBuffer, NULL);
    pthread_cond_init(&condBuffer, NULL);


	// Sockets --------------------------------------------------------------------------
	int sock;

	struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*)&server;



	// Boh8htikh metablhth, krataei thn trexoysa entolh apo to travelMonitor
	char command[20];

	numCountries = argc - 11;

		// printf("argc %d numCountries %d\n", argc-11, numCountries);


	// Desmeysh mnhmhs gia ta subdirectories/xwres
	// subdirectory = (char**)malloc(numCountries * sizeof(char*));

	// // Lhpsh bloom_size
	// if(read(sock, &bloom_size,sizeof(unsigned long)) <0)     {perror("read");    return -1;}
	// close(fd2);

	// Desmeysh mnhmhs enos pinaka arxeiwn kai enos metrhth arxeiwn ana xwra
	// O pinakas arxeiwn exei th morfh:
	// files[ari8mos xwrwn][ari8mos arxeiwn ths xwras]
	// char*** files = (char***)malloc(numCountries * sizeof(char**));
	char* filePaths[1000];
	int filesCounter = 0;
	// thread_args->counter = (int*)malloc(numCountries * sizeof(int));

	// Gemisma toy pinaka arxeiwn me ta arxeia toy ka8e subdirectory 
	for (int i = 0; i < numCountries; i++)
	{
		// thread_args->counter[i]=0;
		// files[i] = (char**)malloc(40 * sizeof(char*));
		if((dir_ptr = opendir(argv[i+11])) == NULL) 		{perror("Open Dir"); 	return -1;}
		while((dir = readdir(dir_ptr)) != NULL )
		{
			// Agnohse to current kai to parent directory
			if(strcmp(dir->d_name, ".")==0 || strcmp(dir->d_name, "..")==0)
				continue;

			// files[i][thread_args->counter[i]] = (char*)malloc(strlen(dir->d_name)+1);
			filePaths[filesCounter] = (char*)malloc(strlen(dir->d_name) + strlen(argv[i+11]) +2);
			// strcpy (files[i][thread_args->counter[i]],dir->d_name);
			strcpy (filePaths[filesCounter],argv[i+11]);
			strcat (filePaths[filesCounter],"/");
			strcat (filePaths[filesCounter],dir->d_name);
			filesCounter++;
			// thread_args->counter[i]++;
		}

		closedir(dir_ptr);
	}
	thread_args->filesCounter = filesCounter;
	for (int i = 0; i < filesCounter; i++)
	{
		// printf("%s\n", filePaths[i]);
	}
	

    // Pedia eggrafwn
	char citizen_id[10];
	char first_name[20];
	char last_name[20];
	char country_name[25];
	char age[4];
	char virus_name[20];
	char vaccination_condition[5];
	char date[15];
	
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
	// hash_table record_hash_table;
	
	// To dentro twn iwn
	// Tree virus_tree;
	
	// Deikths se skip list node
	skip_node* skip_lst_node;
	
	// Deikths se io
	node* virus_node;
	
	// Deikths se eggrafh citizen
	record* citizen_record;

	// Deiktis se country node
	country_node* head=NULL;
	
	// To bloom filtro 
	// Bloom bloom;
	
	// Boh8htikh metablhth
	unsigned int i;
	
	// Boh8htikh metablhth
	unsigned long bit_position;
	
	// Pinakas iwn (kataskeyasmenos apo to antistoixo dentro)
	node** array_of_viruses;
		
	// Metrhths iwn
	thread_args->num_of_viruses=0;
	
	int num_of_buckets;
		
	num_of_buckets = NUM_OF_BUCKETS;

	// Boh8htikh metablhth gia apostolh/lhpsh megalwn mhnymatwn
	unsigned long offset = 0;		
		
	// Arxikopoihsh toy pinaka katakermatismoy twn citizen
	HASH_init(&(thread_args->record_hash_table),num_of_buckets);
	
	// Arxikopoihsh toy dentroy twn iwn filtrou
	BST_init(&(thread_args->virus_tree));
	
	// Arxikopoihsh toy bloom filtrou
	BLOOM_init(&(thread_args->bloom),bloom_size);

	// Metablhth poy krataei to onoma toy trexontos arxeioy  
	// thread_args->file = (char*)malloc(50 * sizeof(char));


	// printf("num_threads %d\n", num_threads);
	if(num_threads > filesCounter)
	{
		// thread_args->num_threads = thread_args->num_threads - thread_args->filesCounter; 
		thread_args->num_threads = filesCounter; 
		num_threads = filesCounter;
		// printf("numThread is more than total files of MonitorServer. numThread adjusted to %d\n", num_threads);
	}

	pthread_t tid[num_threads];

	// Eisagwgh ka8e eggrafhs, olwn twn arxeiwn, ka8e subdirectory
	// stis aparaithtes domes gia th fylaksi twn dedomenwn twn politwn
	for (int i = 0; i < num_threads; i++)
	{
		pthread_create(&tid[i], NULL, threadFunction, thread_args);
	}



	// int threadCounter = 0;
	for (int  i = 0; i < filesCounter; i++)
	{
		// strcpy(thread_args->file,filePaths[i]);
		add_file_to_buffer(filePaths[i]);
	}
	for (int i = 0; i < num_threads; i++)
	{
		pthread_join(tid[i], NULL);
	}

	
	// for (int j = 0; j < filesCounter; j++)
	// {
	// 	// pthread_mutex_lock(&mtx);
	// 	strcpy(thread_args->file,filePaths[j]);
	// 	// pthread_mutex_unlock(&mtx);
		
	// 	// printf("(%d)file: %s\n",fileCount,thread_args->file);

	// 	pthread_create(&tid[threadCounter], NULL, threadFunction, thread_args);
	// 	// printf("ThreadCounter %d\n", threadCounter);
	// 	pthread_join(tid[threadCounter], NULL);
	// 	threadCounter++;
	// 	// usleep(2000);


	// 	// for (int  z = 0; z < thread_args->counter[j]; z++)
	// 	// {
	// 	// 	// pthread_join(tid[z], (void*)&thread_args);
	// 	// }

	// }
	// for (int j = 0; j < filesCounter; j++)
	// {
	// 	pthread_join(tid[j], NULL);
	// }


	// HASH_print(&(thread_args->record_hash_table));

	// Exontas diavasei olous toys ioys, apo to dentro paragoyme antistoixo pinaka deiktwn sta onomata toys
	// Boliko gia seiriakh prospelash
	array_of_viruses=malloc(thread_args->num_of_viruses*sizeof(node*));
	
	BST_fill_array(thread_args->virus_tree,array_of_viruses);

	if (( sock = socket ( AF_INET , SOCK_STREAM , 0) ) < 0)     {perror("socket client"); return -1;}

	struct hostent *rem = gethostbyname("localhost");

	server.sin_family = AF_INET;    // Internet Domain
    memcpy (&server.sin_addr,rem->h_addr, rem->h_length);
    server.sin_port = htons(port);  // Server port

	// Initiate connection
    if ( connect ( sock , serverptr , sizeof ( server ) ) < 0)   {perror("connect client"); return -1;}
    // printf ( "Connecting to port % d \n ", port ) ;

	// Apostolh mege8oys bloom filter toy Monitor 
	writeBytes = strlen(thread_args->bloom.filter) +1;
    // if((fd1 = open(fifo1, O_WRONLY)) < 0 )     {perror("Open fifo1exec");    return -1;}
	if((nwrite = write(sock,&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}


	// Apostolh toy bloom filter toy Monitor sto travelMonitor
	// H apostolh ginetai se tmhmata ta opoia ka8orizei to socket_buff_size
	// H metablhth offset krataei ka8e fora ton ari8mo twn bytes poy exoun hdh stal8ei
	while(offset < bloom_size)
	{
		if((nwrite = write(sock,thread_args->bloom.filter + offset, socket_buff_size)) == -1)	{perror("write");   return -1;}
		offset += nwrite;
	}

	// close(fd1);

	// Main loop poy ektelei tis entoles toy travelMonitor
	while (1)
	{
		// Lhpsh mege8oys entolhs
		// if((fd2 = open(fifo2, O_RDONLY)) < 0 )     {perror("Open fifo2-Monitor");    return -1;}
		if(read(sock, &readBytes, sizeof(int)) < 0)     {perror("read-comm-size");    return -1;}

		// Ean diavasei arnhtiko ari8mo tote to travelMonitor ektelese request
		// poy afora thn eisagwgh se kapoia xwra poy exei analabei to Monitor
		// Meta thn afksisi twn aparaithtwn metrhtwn to Monitor perimenei na ektelesei nea entolh
		if (readBytes == -1)
		{
			// To -1 ypodeiknyei oti to aithma egkri8hke
			accepts ++;
			continue;
		}
		else if(readBytes == -2)
		{
			// To -2 ypodeiknyei oti to aithma aporif8hke
			rejects ++;

			continue;
		}


		// Lhpsh entolhs
		if(read(sock, command, readBytes) < 0)     {perror("read");    return -1;}
	
		// Boh8htiko command
		if (strcmp(command,"exit")==0)
		{
			int_quit_children(argc, argv);
			break;
		}

		// searchVaccinationStatus //
		else if(strcmp(command,"searchVaccinationStatus")==0)
		{
			// Lhpsh citizen_id
			if(read(sock, &readBytes, sizeof(int)) < 0)     {perror("read-mon(1)");    return -1;}
			if(read(sock, citizen_id, readBytes) < 0)     {perror("read");    return -1;}

			record* requested_citizen;

			// Periptwsh poy o poliths yparxei stis domes toy Monitor
			if((requested_citizen=HASH_exists(&(thread_args->record_hash_table), citizen_id))!=NULL)
			{

				writeBytes = strlen(requested_citizen->first_name) +1;

				// Apostolh first_name
				// if((fd1 = open(fifo1, O_WRONLY)) < 0 )     {perror("Open fifo1exec");    return -1;}
				if((nwrite = write(sock,&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
				if((nwrite = write(sock,requested_citizen->first_name, writeBytes)) == -1)    {perror("write");   return -1;}

				// Apostolh last_name
				writeBytes = strlen(requested_citizen->last_name) +1;
				if((nwrite = write(sock,&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
				if((nwrite = write(sock,requested_citizen->last_name, writeBytes)) == -1)    {perror("write");   return -1;}

				// Apostolh xwras
				writeBytes = strlen(requested_citizen->country_name) +1;
				if((nwrite = write(sock,&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
				if((nwrite = write(sock,requested_citizen->country_name, writeBytes)) == -1)    {perror("write");   return -1;}

				// Apostolh hlikias
				if((nwrite = write(sock,&requested_citizen->age, sizeof(int))) == -1)    {perror("write");   return -1;}

				// Apostolh ari8moy iwn
				if((nwrite = write(sock,&thread_args->num_of_viruses, sizeof(int))) == -1)    {perror("write");   return -1;}

				// // Anazhthsh stis skip lists olwn twn iwn
				for(i=0;i<thread_args->num_of_viruses;i++)
				{
					// Boh8htikh metablhth gia thn apanthsh sto travelMonitor 
					unsigned short reply_bool;
									
					skip_lst_node=SKIP_LIST_search(array_of_viruses[i]->vacc_list,atoi(citizen_id));
					
					// Apostolh ioy
					writeBytes = strlen(array_of_viruses[i]->virus) +1;
					if((nwrite = write(sock,&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
					if((nwrite = write(sock,array_of_viruses[i]->virus, writeBytes)) == -1)    {perror("write");   return -1;}

					//  Emfanizoyme to apotelesma ths anazhthshs
					if(skip_lst_node)
					{
						// Apostolh 8etikhs apanthshs emvoliasmoy gia to sygkekrimeno io
						reply_bool = 1;
						if((nwrite = write(sock,&reply_bool, sizeof(short))) == -1)    {perror("write");   return -1;}

						// Apostolh hmeromhnias emvoliasmoy
						writeBytes = strlen(skip_lst_node->date) +1;
						if((nwrite = write(sock,&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
						if((nwrite = write(sock,skip_lst_node->date, writeBytes)) == -1)    {perror("write");   return -1;}

					}
					else
					{
						// Apostolh arnhtikhs apanthshs emvoliasmoy gia to sygkekrimeno io
						reply_bool = 0;
						if((nwrite = write(sock,&reply_bool, sizeof(short))) == -1)    {perror("write");   return -1;}
												
					}									
				}				


				// close(fd1);

			}

			// Efoson o poliths den yparxei stis domes toy Monitor
			// to Monitor de xreiazetai na apanthsei sto travelMonitor

		}
		// Boh8htiko command
		else if (strcmp(command,"continue")==0)
		{
			printf("Monitor continues\n");
			continue;
		}

		// travelRequest //
		else if (strcmp(command,"travelRequest")==0)
		{
			char reply[12];

			// Lhpsh citizen_id
			if(read(sock, &readBytes, sizeof(int)) < 0)     {perror("read");    return -1;}
			if(read(sock, citizen_id, readBytes) < 0)     {perror("read");    return -1;}

			// Lhpsh onomatos ioy
			if(read(sock, &readBytes, sizeof(int)) < 0)     {perror("read");    return -1;}
			if(read(sock, virus_name, readBytes) < 0)     {perror("read");    return -1;}
			// close(fd2);

			// Anazhtoyme ton komvo toy ioy sto dentro
			virus_node=BST_search(thread_args->virus_tree,virus_name);
			
			// An brethei kanoyme anazhthsh sthn antistoixh skip list
			if(virus_node)
			{
				
				skip_lst_node=SKIP_LIST_search(virus_node->vacc_list,atoi(citizen_id));
				
				//  Periptwsh poy o poliths exei emvoliastei
				if(skip_lst_node)
				{
					strcpy(reply, "YES");
					writeBytes = strlen(reply) +1;

					// Apostolh 8etikhs apanthshs
					// if((fd1 = open(fifo1, O_WRONLY)) < 0 )     {perror("Open fifo1exec");    return -1;}
					if((nwrite = write(sock,&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
					if((nwrite = write(sock,reply, writeBytes)) == -1)    {perror("write");   return -1;}
					
					strcpy(reply,skip_lst_node->date);
					writeBytes = strlen(reply) +1;

					// Apostolh hmeromhnias emvoliasmou
					if((nwrite = write(sock,&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
					if((nwrite = write(sock,reply, writeBytes)) == -1)    {perror("write");   return -1;}
					// close(fd1);
					
				}
				// Periptwsh poy den exei emvoliastei
				else
				{
					
					strcpy(reply, "NO");
					writeBytes = strlen(reply) +1;

					// Apostolh arnhtikhs apanthshs
					// if((fd1 = open(fifo1, O_WRONLY)) < 0 )     {perror("Open fifo1exec");    return -1;}
					if((nwrite = write(sock,&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
					if((nwrite = write(sock,reply, writeBytes)) == -1)    {perror("write");   return -1;}
					// close(fd1);
				}					
				
			}
			else
			{
				// Edw einai h periptwsh poy den exei vre8ei o ios
				printf("Can't find virus\n");

				// To -1 deixnei oti o ios de vre8hke
				writeBytes=-1;
				if((nwrite = write(sock,&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
						
			}
		}
	}

	// close(sock);

	// Kleisimo twn named pipes
    // close(fd1);
    // close(fd2);

	// Eleftherwsh domwn kai desmeymenhs mnhmhs
	BLOOM_destroy(&(thread_args->bloom));
	HASH_clear(&(thread_args->record_hash_table));
	BST_destroy(thread_args->virus_tree);
	free(array_of_viruses);


	// for (int i = 0; i < numCountries; i++)
	// {
	// 	free(subdirectory[i]);
	// 	for (int  j = 0; j < thread_args->counter[i]; j++)
	// 	{
	// 		free(files[i][j]);
	// 	}
		
	// 	free(files[i]);
	// }
	
	// free(subdirectory);
	// free(thread_args->counter);
	// free(thread_args->file);
	// free(files);
	free(thread_args);

	pthread_mutex_destroy(&mtxBuffer);
	pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&condBuffer);

    return 0;
}

void* threadFunction(void* arg)
{
	while(1)
	{
		char file[70];
		pthread_mutex_lock(&mtxBuffer);
		while(filesBufferCounter == 0)
		{
			pthread_cond_wait(&condBuffer, &mtxBuffer);
		}
		strcpy(file,filesBuffer[0]);
		// printf("threadFunction: %s\n", file);
		for (int i = 0; i < filesBufferCounter-1; i++)
		{
			strcpy(filesBuffer[i],filesBuffer[i+1]);
		}
		filesBufferCounter--;
		pthread_mutex_unlock(&mtxBuffer);
		
		// usleep(500000);
		pthread_mutex_lock(&mtx);
		arguments *thread_args = (arguments*)arg;
		pthread_mutex_unlock(&mtx);


		// Grammh arxeioy
		char file_line[FILE_LINE_LENGTH];

		// Pedia eggrafwn
		char* citizen_id;
		char* first_name;
		char* last_name;
		char* country_name;
		char* age;
		char* virus_name;
		char* vaccination_condition;
		char* date;

		// Deikths se eggrafh citizen
		record* citizen_record;

		// Deikths se io
		node* virus_node;

		// To string poy ginetai hash sto bloom filter
		unsigned char bloom_string[BLOOM_STRING_MAX_LENGTH];

		// Boh8htikh metablhth
		unsigned long bit_position;

		
		FILE* fp=fopen(file,"r");
		// printf("%s\n", file);

		// Elegxos anoigmatos arxeioy
		if(fp==NULL)
		{
			
			printf("Open file error\n");
			return 0;
			
		}
		pthread_mutex_lock(&mtx);
		int brazilCounter = 0;
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

			citizen_record=HASH_add_patient(&(thread_args->record_hash_table),current_line);

			
			virus_node=BST_search(thread_args->virus_tree,virus_name);			
			
			if(virus_node==NULL)
			{
				virus_node=BST_insert(&(thread_args->virus_tree),virus_name);	
				thread_args->num_of_viruses++;		
			}
			
			// Ean prokeitai gia eggrafh emvoliasmenoy eisagetai sto bloom filter
			//  kai sthn antistoixh lista emvoliasmenwn
			if(!strcmp(vaccination_condition,"YES"))
			{	
				
				for(int i=0;i<16;i++)
				{
					// Kataskeyh string poy tha ginei hash
					sprintf(bloom_string,"%s%s",citizen_id,virus_name);
						
					// Evresh theshs poy tha ginei set
					bit_position=BLOOM_hash(bloom_string,i);
					
					BLOOM_insert(&(thread_args->bloom),bit_position);
				}
								
				SKIP_LIST_insert(&(virus_node->vacc_list),atoi(citizen_id),citizen_record,date);
							
			}
			// Diaforetika exoyme eisagwgh monaxa sth non vaccinated list
			else
				SKIP_LIST_insert(&(virus_node->not_vacc_list),atoi(citizen_id),citizen_record,NULL);	
				
		}
		pthread_mutex_unlock(&mtx);
		totalFilesEdited ++;
		

		fclose(fp);
		if (totalFilesEdited >= thread_args->filesCounter - thread_args->num_threads +1)
		{
			// printf("TotalFilesEdited %d , FilesCounter %d, FilesBufferCounter %d\n",totalFilesEdited, thread_args->filesCounter, filesBufferCounter);
			break;
		}

	}

	pthread_exit(NULL);
}