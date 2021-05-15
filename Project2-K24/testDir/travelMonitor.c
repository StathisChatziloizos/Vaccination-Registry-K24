#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <math.h>



#include "bloom.h"

#define INNER_MENU_LINE_LENGTH 200
#define FILE_LINE_LENGTH 200
#define BLOOM_STRING_MAX_LENGTH 50
#define BUFFER_SIZE 512

const int pipeSize = 512;

int cmpstringp(const void *p1, const void *p2)
{
    return strcmp(*(const char **) p1, *(const char **) p2);
}

int main(int argc, char** argv)
{

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

    // Boh8htikh metablhth
	unsigned long bit_position;

    // Entolh sto eswteriko menoy
	char* command_name;

	// Eswterikh grammh entolwn
	char inner_menu_line[INNER_MENU_LINE_LENGTH];	
	
	// Grammh arxeioy
	char file_line[FILE_LINE_LENGTH];

	// To megethos toy buffer
	unsigned int buffer_size;

	// O ari8mos Monitor diergasiwn
	unsigned int num_Monitors; 

    Bloom bloom;
	// To megethos bloom filtroy
    unsigned long bloom_size;

	char* directoryName;
    DIR *dir_ptr;
    struct dirent *dir;

    // int fd1, fd2;
    char msgbuf[BUFFER_SIZE+1];

	// Pinakas xwrwn
	char* countries[20];

	// Subdirectory poy analambanoyn ta Monitors
	char* subdirectory;

	// Boh8htikh metablhth
	unsigned int counter = 0;

	unsigned int countries_per_Monitor;


	// Elegxoi orismatwn ekteleshs programmatos
	if(argc!=9)
	{
		printf("Usage: travelMonitor -m numMonitors –b bufferSize -s sizeOfBloom -i input_dir\n");
		return 0;	
	}
	
	if(strcmp(argv[1],"-m")!=0&&strcmp(argv[3],"-b")!=0
		&&strcmp(argv[5],"-s")!=0&&strcmp(argv[7],"-i")!=0)
	{
		printf("Usage: travelMonitor -m numMonitors –b bufferSize -s sizeOfBloom -i input_dir\n");
		return 0;		
	}
	
	// Ka8orismos ari8moy Monitor diergasiwn
	num_Monitors=(unsigned int)(atoi(argv[2]));

	// ka8orismos mege8ous toy buffer
	buffer_size=(unsigned int)(atoi(argv[4]));

	// ka8orismos mege8ous bloom filtroy
	bloom_size=(unsigned long)(atol(argv[6]));

	// Ka8orismos toy input_dir
	directoryName = argv[8];

	// Gemisma toy pinaka xwrwn me tis xwres toy in
    if((dir_ptr = opendir(directoryName)) == NULL) 		{perror("Open Dir"); 	return -1;}
	while((dir = readdir(dir_ptr)) != NULL )
    {
		// Agnohse to current kai to parent directory
		if(strcmp(dir->d_name, ".")==0 || strcmp(dir->d_name, "..")==0)
			continue;
		// countries[counter] = dir->d_name;
		countries[counter] = (char*) malloc (strlen(dir->d_name)+1);
        strcpy (countries[counter],dir->d_name);
		counter ++;
    }
	
	closedir(dir_ptr);
	
	// Takshnomish toy pinaka xwrwn alfabhtika
	qsort(&countries[0], counter, sizeof(char *), cmpstringp);

	// for (int i = 0; i < counter; i++)
	// {
	// 	printf("countries[%d] = %s\n",i, countries[i]);
	// }
	

	// printf("countries[3] = %s\n",countries[3]);
	
	// Elegxos megethoys bloom filtroy
	if(bloom_size<=0){
		
		printf("Bloom filter length error\n");
		return 0;
		
	}

	BLOOM_init(&bloom, bloom_size);

	// char* fifo1 = "fifo1";
	// char* fifo2 = "fifo2";

	// Dhmiourgia array gia apo8hkeysh twn file descriptors, 2 ana Monitor (1Read/1Write)
	unsigned int* fd1 = (unsigned int*)malloc(num_Monitors * sizeof(int));
	unsigned int* fd2 = (unsigned int*)malloc(num_Monitors * sizeof(int));

	// Dhmiourgia array gia apo8hkeysh twn onomatwn twn named pipes, 2 ana Monitor (1Read/1Write)
	char** fifo1 = (char**)malloc(num_Monitors * sizeof(char*));
	char** fifo2 = (char**)malloc(num_Monitors * sizeof(char*));


	// Dhmioyrgia named pipes, 2 ana Monitor (1Read/1Write)
	for (int i = 0; i < num_Monitors; i++)
	{
		fifo1[i] = (char*)malloc(12 * sizeof(char));
		fifo2[i] = (char*)malloc(12 * sizeof(char));
		sprintf(fifo1[i],"fifo1-%d",i);
		sprintf(fifo2[i],"fifo2-%d",i);

		if(mkfifo(fifo1[i],0666) == -1)     {perror("mkfifo");    return -1;}
		if(mkfifo(fifo2[i],0666) == -1)     {perror("mkfifo");    return -1;}
		// printf("fifo1[%d] = %s, fifo2[%d] = %s\n", i, fifo1[i], i, fifo2[i]);
	}
	



    // if((fd2 = open(fifo2, O_RDWR | O_NONBLOCK)) < 0 )     {perror("Open fifo2");    return -1;}

	// printf("Counter = %d\n", counter);
	countries_per_Monitor = ceil((float)counter / num_Monitors);
	// printf("countriesPerMonitor = %d\n", countries_per_Monitor);


    pid_t pid[num_Monitors];
    for (int i = 0; i < num_Monitors; i++)
    {
        pid[i] = fork();

        if (pid[i] < 0)     {perror("Fork ");    return -1;}
        if (pid[i] == 0)
        {
			// printf("pid==0 HERE\n");
            char* arg="hey" ;
            // sprintf(arg,"%d-Message", i);
            char* args[] = {"./Monitor", "fifo1", "fifo2" , NULL};
            execv(args[0], args);   //exec ./Monitor
        }
    }

	int i;
    for(i=0; i < num_Monitors; i++)
    {
		int nwrite, readBytes, writeBytes;
		// char* subdirectory;
		subdirectory = (char*)malloc(50 * sizeof(char));

		strcpy(subdirectory,directoryName);

		// Pros8hkh '/' sto telos toy directory poy do8hke efoson leipei
		if(subdirectory[strlen(subdirectory)-1] !='/')
			strcat(subdirectory, "/");

		strcat(subdirectory,countries[3]);


		writeBytes = strlen(subdirectory) + 1;
		char str[100];

		if((fd2[i] = open(fifo2[i], O_WRONLY)) < 0 )     {perror("Open fifo2-TravelMonitor");    return -1;}
		if((nwrite = write(fd2[i],&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
		if((nwrite = write(fd2[i],subdirectory, writeBytes)) == -1)    {perror("write");   return -1;}		//Apostolh subdirectory
		if((nwrite = write(fd2[i],&bloom_size, sizeof(unsigned long))) == -1)    {perror("write");   return -1;}		//Apostolh bloom_size
		close(fd2[i]);

		printf("Here\n");

		if((fd1[i] = open(fifo1[i], O_RDONLY)) < 0 )     {perror("Open fifo1");    return -1;}
        if(read(fd1[i], &readBytes, sizeof(int)) < 0)     {perror("read");    return -1;}
		// printf("readBytes = %d\n", readBytes);
		// bloom.test = (char*)malloc(readBytes);
		if(read(fd1[i], bloom.filter, bloom.size) < 0)     {perror("read");    return -1;}		// Apostolh Bloom Filter sto Monitor
		close(fd1[i]);
		// printf("%s\n", bloom.filter);
		readBytes = strlen(bloom.filter);
		// if(read(fd1, &bloom, sizeof(bloom)) < 0)     {perror("read");    return -1;}


        // printf("Message Received: %s - %d - %s\n", bloom.filter, bloom.size, bloom.test);
        fflush(stdout);
    }

    // for (int i = 0; i < NUM_OF_PROCESSES; i++)
    // {
    //     wait(NULL);
    // }

 


    while(1)
	{
		
		printf("/");		
	
		// Diavasma entolhs xrhsth
		fgets(inner_menu_line,INNER_MENU_LINE_LENGTH-1,stdin);
	
		// Anagnwrish entolhs
		command_name = strtok(inner_menu_line," \t\n");
		


		//  Voi8htiko command poy ka8arizei thn o8onh
		if(strcmp(command_name,"clear")==0)
			system("clear");
		
		// vaccineStatusBloom //
		else if(strcmp(command_name,"vaccineStatusBloom")==0)
		{			
			// Diavasma orismatwn
			citizen_id=strtok(NULL," \t\n");
			virus_name=strtok(NULL," \t\n");
			
			// Elegxos gia elliph orismata
			if(virus_name==NULL)
			{
				
				printf("Missing argument\n");
				
			}
			else
			{
							
				for(i=0;i<16;i++)
				{
					// Kataskeyh string poy tha ginei hash
					sprintf(bloom_string,"%s%s",citizen_id,virus_name);
					
					// Evresh theshs prepei na eksetastei an einai set					
					bit_position=BLOOM_hash(bloom_string,i);
					// Ean h thesh den einai set o citizen den einai emvoliasmenos
					if(BLOOM_get(&bloom,bit_position)==0)
					{
						
						printf("NOT VACCINATED\n");												
					
						// To i ginetai 0 kai ginetai break
						i=0;
						break;
						
					}
	
				
				}
				
				// An to i den einai 0 o citizen einai pithanws emvoliasmenos
				if(i!=0)
				{
					
					printf("MAYBE\n");	
					
				}
				
			
			}
					
		}
        	
		// exit //
		else if(strcmp(command_name,"exit")==0)
			break;
		
    }


	// Elef8erwsh mnhmhs
	for(int i=0; i < counter; i++)
	{
		free(countries[i]);
	}

	for (int i = 0; i < num_Monitors; i++)
	{
		unlink(fifo1[i]);
		unlink(fifo2[i]);
		free(fifo1[i]);
		free(fifo2[i]);
	}
	free(fifo1);
	free(fifo2);
	free(subdirectory);
	
	BLOOM_destroy(&bloom);
	free(fd1);
	free(fd2);

    return 0;
}