#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>



#include "bloom.h"

#define INNER_MENU_LINE_LENGTH 200
#define FILE_LINE_LENGTH 200
#define BLOOM_STRING_MAX_LENGTH 50
#define BUFFER_SIZE 512
#define NUM_OF_PROCESSES 1

char* fifo1 = "fifo1";
char* fifo2 = "fifo2";

const int pipeSize = 512;


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

    Bloom bloom;
    int bloom_size = 10000;
    BLOOM_init(&bloom, bloom_size);


    int fd1, fd2;
    char msgbuf[BUFFER_SIZE+1];

    if(mkfifo(fifo1,0666) == -1)     {perror("mkfifo");    return -1;}
    if(mkfifo(fifo2,0666) == -1)     {perror("mkfifo");    return -1;}

    // if((fd2 = open(fifo2, O_RDWR | O_NONBLOCK)) < 0 )     {perror("Open fifo2");    return -1;}

    pid_t pid[NUM_OF_PROCESSES];
    for (int i = 0; i < NUM_OF_PROCESSES; i++)
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

	if((fd1 = open(fifo1, O_RDONLY)) < 0 )     {perror("Open fifo1");    return -1;}
    int i=0;
    while(i < NUM_OF_PROCESSES)
    {
        i++;
		int readBytes;
		char* message;
		char str[100];

        if(read(fd1, &readBytes, sizeof(int)) < 0)     {perror("read");    return -1;}
		// printf("readBytes = %d\n", readBytes);
		// bloom.test = (char*)malloc(readBytes);
		if(read(fd1, bloom.filter, bloom.size) < 0)     {perror("read");    return -1;}
		// printf("%s\n", bloom.filter);
		readBytes = strlen(bloom.filter);
		// if(read(fd1, &bloom, sizeof(bloom)) < 0)     {perror("read");    return -1;}


        // printf("Message Received: %s - %d - %s\n", bloom.filter, bloom.size, bloom.test);
        fflush(stdout);
        if(strcmp(msgbuf,"exit") == 0)      break;
    }
	close(fd1);

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
			printf("Here\n");
			// Diavasma orismatwn
			citizen_id=strtok(NULL," \t\n");
			virus_name=strtok(NULL," \t\n");
			
			printf("CitizenID %s, virus_name %s\n", citizen_id, virus_name);

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
	
	BLOOM_destroy(&bloom);
    close(fd2);
    unlink(fifo1);
    unlink(fifo2);

    return 0;
}