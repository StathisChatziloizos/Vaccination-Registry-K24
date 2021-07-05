#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct arguments{
    char* name;
    unsigned long long vlaue;
}arguments;

pthread_mutex_t mtx;

void* threadFunction(void* arg)
{
	pthread_mutex_lock(&mtx);
    arguments *thread_args = (arguments*)arg;

	// Grammh arxeioy
	char file_line[200];

	// Pedia eggrafwn
	char* citizen_id;
	char* first_name;
	char* last_name;
	char* country_name;
	char* age;
	char* virus_name;
	char* vaccination_condition;
	char* date;

	
	FILE* fp=fopen("inputDir/Brazil/Brazil-0","r");

	// Elegxos anoigmatos arxeioy
	if(fp==NULL)
	{
		
		printf("Open file error\n");
		return 0;
		
	}
	
	int brazilCounter = 0;
	while(fgets(file_line,200-1,fp)!=NULL)
	{
        brazilCounter++;
        // printf("Here\n");
		char current_line[200];
		strcpy(current_line,file_line);		
        printf("(%d) %s",brazilCounter, current_line);


	}

	fclose(fp);
	pthread_mutex_unlock(&mtx);
    // pthread_exit((void*)thread_args);
    pthread_exit(NULL);

}

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        printf("\nCorrect format is: ./ program {int}\n\n");
        return -1;
    }
    
    int num_threads = atoi(argv[1]);
    // int *var;

    pthread_mutex_init(&mtx, NULL);


    arguments* thread_args = (arguments*) malloc(sizeof(arguments));
    thread_args->name = (char*)malloc(20*sizeof(char));
    strcpy(thread_args->name,"Stathis");
    thread_args->vlaue = 0;
    // printf("Initialization: *var = %d\n", *var);

    pthread_t tid[num_threads];

    for (int i = 0; i < num_threads; i++)
    {
        // Create child thread
        pthread_create(&tid[i], NULL, threadFunction, thread_args);
    }
    
    for (int i = 0; i < num_threads; i++)
    {
        //Join child thread
        pthread_join(tid[i], (void*)&thread_args);
        // pthread_join(tid[i], (void*)&thread_args);
    }

    //Parent thread is printing
    // printf("Parent thread: name = %s\n", thread_args->name);
    // printf("Parent thread: value = %lld\n", thread_args->vlaue);

    pthread_mutex_destroy(&mtx);
    // free(thread_args->name);
    // free(thread_args);

    return 0;
}


// typedef struct argum{
//     int* counter;
//     char*** files;
//     hash_table record_hash_table;
//     Tree virus_tree;
//     int num_of_viruses;
//     Bloom bloom;

// }argum;