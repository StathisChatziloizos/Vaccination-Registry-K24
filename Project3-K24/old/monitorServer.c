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
// #include <netbd.h>


#include "hash.h"
#include "bst.h"
#include "bloom.h"
#include "skip_list.h"

#define INNER_MENU_LINE_LENGTH 200
#define FILE_LINE_LENGTH 200
#define NUM_OF_BUCKETS 50
#define BLOOM_STRING_MAX_LENGTH 50

static char** subdirectory;
static int accepts = 0;
static int rejects = 0;
static int numCountries;



// Synarthsh poy diaxeirizetai ta SIGINT/SIGQUIT twn Monitors
void int_quit_children()
{
	// Dhmioyrgia log_file.xxx arxeioy
	char log_file[20];
	sprintf(log_file,"log_file.%d",getpid());
	FILE* fp = fopen(log_file,"w+");

	// Eisagwgh ka8e xwras sto log_file
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

		// Metablhth poy krata th xwra apo to subdirectory (Morfh subdirextory: input_dir/xwra)
		char* country=subdirectory[i]+position+1;
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

	int port = atoi(argv[2]);
	unsigned int num_threads = (unsigned int)atoi(argv[4]);

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
	signal(SIGINT,int_quit_children);
	// signal(SIGQUIT,int_quit_children);

    int nwrite, readBytes, writeBytes;

	// Boh8htikes metablhtes gia prospelash katalogwn
	char* directoryName;
    DIR* dir_ptr;
    struct dirent* dir;


	// Sockets --------------------------------------------------------------------------
	int sock;

	struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*)&server;

	if (( sock = socket ( AF_INET , SOCK_STREAM , 0) ) < 0)     {perror("socket client"); return -1;}

	server.sin_family = AF_INET;    // Internet Domain
    // memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("127.0.0.1");
    server.sin_port = htons(port);  // Server port

	// Initiate connection
    if ( connect ( sock , serverptr , sizeof ( server ) ) < 0)   {perror("connect client"); return -1;}
    // printf ( "Connecting to port % d \n ", port ) ;

	// Boh8htikh metablhth, krataei thn trexoysa entolh apo to travelMonitor
	char command[20];

	numCountries = argc - 11;

		// printf("argc %d numCountries %d\n", argc-11, numCountries);


	// Desmeysh mnhmhs gia ta subdirectories/xwres
	subdirectory = (char**)malloc(numCountries * sizeof(char*));

	// Lhpsh bloom_size
	if(read(sock, &bloom_size,sizeof(unsigned long)) <0)     {perror("read");    return -1;}
	// close(fd2);

	// Desmeysh mnhmhs enos pinaka arxeiwn kai enos metrhth arxeiwn ana xwra
	// O pinakas arxeiwn exei th morfh:
	// files[ari8mos xwrwn][ari8mos arxeiwn ths xwras]
	char*** files = (char***)malloc(numCountries * sizeof(char**));
	int* counter = (int*)malloc(numCountries * sizeof(int));

	// Gemisma toy pinaka arxeiwn me ta arxeia toy ka8e subdirectory 
	for (int i = 0; i < numCountries; i++)
	{
		counter[i]=0;
		files[i] = (char**)malloc(40 * sizeof(char*));
		if((dir_ptr = opendir(argv[i+11])) == NULL) 		{perror("Open Dir"); 	return -1;}
		while((dir = readdir(dir_ptr)) != NULL )
		{
			// Agnohse to current kai to parent directory
			if(strcmp(dir->d_name, ".")==0 || strcmp(dir->d_name, "..")==0)
				continue;

			files[i][counter[i]] = (char*)malloc(strlen(dir->d_name)+1);
			strcpy (files[i][counter[i]],dir->d_name);
			counter[i]++;
		}

		closedir(dir_ptr);
	}

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

	// Boh8htikh metablhth gia apostolh/lhpsh megalwn mhnymatwn
	unsigned long offset = 0;		
		
	// Arxikopoihsh toy pinaka katakermatismoy twn citizen
	HASH_init(&record_hash_table,num_of_buckets);
	
	// Arxikopoihsh toy dentroy twn iwn filtrou
	BST_init(&virus_tree);
	
	// Arxikopoihsh toy bloom filtrou
	BLOOM_init(&bloom,bloom_size);

	// Metablhth poy krataei to onoma toy trexontos arxeioy  
	unsigned char file[50];

	// Eisagwgh ka8e eggrafhs, olwn twn arxeiwn, ka8e subdirectory
	// stis aparaithtes domes gia th fylaksi twn dedomenwn twn politwn
	for (int j = 0; j < numCountries; j++)
	{
		for (int  z = 0; z < counter[j]; z++)
		{
			// An den yparxei to arxeio synexise
			if(files[j][z] == NULL)
				continue;
			strcpy(file,argv[j+11]);
			strcat(file, "/");
			strcat(file,files[j][z]);

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


				citizen_record=HASH_add_patient(&record_hash_table,current_line);

				
				virus_node=BST_search(virus_tree,virus_name);			
				
				if(virus_node==NULL)
				{
					virus_node=BST_insert(&virus_tree,virus_name);	
					num_of_viruses++;		
				}
				
				// Ean prokeitai gia eggrafh emvoliasmenoy eisagetai sto bloom filter
				//  kai sthn antistoixh lista emvoliasmenwn
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


	// Apostolh mege8oys bloom filter toy Monitor 
	writeBytes = strlen(bloom.filter) +1;
    // if((fd1 = open(fifo1, O_WRONLY)) < 0 )     {perror("Open fifo1exec");    return -1;}
	if((nwrite = write(sock,&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}


	// Apostolh toy bloom filter toy Monitor sto travelMonitor
	// H apostolh ginetai se tmhmata ta opoia ka8orizei to socket_buff_size
	// H metablhth offset krataei ka8e fora ton ari8mo twn bytes poy exoun hdh stal8ei
	while(offset < bloom_size)
	{
		if((nwrite = write(sock,bloom.filter + offset, socket_buff_size)) == -1)	{perror("write");   return -1;}
		offset += nwrite;
	}

	// close(fd1);

	// Main loop poy ektelei tis entoles toy travelMonitor
	while (1)
	{
		// Lhpsh mege8oys entolhs
		// if((fd2 = open(fifo2, O_RDONLY)) < 0 )     {perror("Open fifo2-Monitor");    return -1;}
		if(read(sock, &readBytes, sizeof(int)) < 0)     {perror("read");    return -1;}

		// Ean diavasei arnhtiko ari8mo tote to travelMonitor ektelese request
		// poy afora thn eisagwgh se kapoia xwra poy exei analabei to Monitor
		// Meta thn afksisi twn aparaithtwn metrhtwn to Monitor perimenei na ektelesei nea entolh
		if (readBytes == -1)
		{
			// To -1 ypodeiknyei oti to aithma egkri8hke
			accepts ++;
			// close(fd2);
			usleep(1);

			continue;
		}
		else if(readBytes == -2)
		{
			// To -2 ypodeiknyei oti to aithma aporif8hke
			rejects ++;
			usleep(1);
			// close(fd2);

			continue;
		}


		// Lhpsh entolhs
		if(read(sock, command, readBytes) < 0)     {perror("read");    return -1;}
	
		// Boh8htiko command
		if (strcmp(command,"softExit")==0)
		{
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
			if((requested_citizen=HASH_exists(&record_hash_table, citizen_id))!=NULL)
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
				if((nwrite = write(sock,&num_of_viruses, sizeof(int))) == -1)    {perror("write");   return -1;}

				// // Anazhthsh stis skip lists olwn twn iwn
				for(i=0;i<num_of_viruses;i++)
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
			virus_node=BST_search(virus_tree,virus_name);
			
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
				// Edw einai h periptwsh poy den exei brethei o ios
				printf("Can't find virus\n");						
			}
		}
	}

	close(sock);

	// Kleisimo twn named pipes
    // close(fd1);
    // close(fd2);

	// Eleftherwsh domwn kai desmeymenhs mnhmhs
	BLOOM_destroy(&bloom);
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
	

    return 0;
}