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
#include <sys/select.h>



#include "bloom.h"
#include "libTM.h"

#define INNER_MENU_LINE_LENGTH 200
#define BLOOM_STRING_MAX_LENGTH 50


static int flag = 0;

// Synarthsh poy diaxeirizetai ta SIGINT/SIGQUIT toy travelMonitor
void int_quit_parent();

int main(int argc, char** argv)
{
	signal(SIGINT,int_quit_parent);
	signal(SIGQUIT,int_quit_parent);

    // Pedia eggrafwn
	char* citizen_id;
	char* first_name;
	char* last_name;
	char* country_name;
	char* country_from;
	char* country_to;
	char* age;
	char* virus_name;
	char* vaccination_condition;
	char* date;
	char* date2;

	// To string poy ginetai hash sto bloom filter
	unsigned char bloom_string[BLOOM_STRING_MAX_LENGTH];

    // Boh8htikh metablhth
	unsigned long bit_position;

    // Entolh sto eswteriko menoy
	char* command_name;

	// Eswterikh grammh entolwn
	char inner_menu_line[INNER_MENU_LINE_LENGTH];	

	// To megethos toy buffer
	unsigned int buffer_size;

	// O ari8mos Monitor diergasiwn
	unsigned int num_Monitors; 

	// Deikths se Bloom struct
    Bloom* bloom;

	// To megethos bloom filtroy
    unsigned long bloom_size;

	// Boh8htikes metablhtes gia prospelash katalogwn
	char* directoryName;
    DIR* dir_ptr;
    struct dirent* dir;


	// Pinakas xwrwn
	char* countries[200];

	// Subdirectory poy analambanoyn ta Monitors
	char* subdirectory;

	// Boh8htikh metablhth
	unsigned int counter = 0;

	// Boh8htikh metablhth gia apostolh/lhpsh megalwn mhnymatwn
	unsigned long offset = 0;

	// Deikths se domh Monitor
	Monitor* monitor;

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

	// Gemisma toy pinaka xwrwn me tis xwres toy directory
    if((dir_ptr = opendir(directoryName)) == NULL) 		{perror("Open Dir"); 	return -1;}
	while((dir = readdir(dir_ptr)) != NULL )
    {
		// Agnohse to current kai to parent directory
		if(strcmp(dir->d_name, ".")==0 || strcmp(dir->d_name, "..")==0)
			continue;

		countries[counter] = (char*) malloc (strlen(dir->d_name)+1);
        strcpy (countries[counter],dir->d_name);
		counter ++;
    }
	
	closedir(dir_ptr);
	
	// Takshnomish toy pinaka xwrwn alfabhtika
	qsort(&countries[0], counter, sizeof(char *), comparator);

	// Ta Monitos mporei na einai to poly oses kai oi xwres
	if(num_Monitors > counter)
		num_Monitors=counter;

	
	// Elegxos megethoys bloom filtroy
	if(bloom_size<=0){
		
		printf("Bloom filter length error\n");
		return 0;
		
	}

	// Desmeysh mnhmhs gia ta Monitors
	monitor = (Monitor*)malloc(num_Monitors * sizeof(Monitor));

	// Desmeysh mnhmhs enos pinaka apo Bloom kai arxikopoihsh toys 
	bloom = (Bloom*)malloc(num_Monitors * sizeof(Bloom));
	for (int i = 0; i < num_Monitors; i++)
	{
		BLOOM_init(&bloom[i], bloom_size);
	}

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
	}
	

	// Fork - Dhmioyrgia Monitors
    pid_t pid[num_Monitors];
    for (int i = 0; i < num_Monitors; i++)
    {
        pid[i] = fork();

        if (pid[i] < 0)     {perror("Fork ");    return -1;}
        if (pid[i] == 0)
        {
            char* args[] = {"./Monitor", fifo1[i], fifo2[i] , NULL};
            execv(args[0], args);   //exec ./Monitor
        }
    }

	// Metrhths
	int i;

	// Desmeysh mnhmhs subdirectory
	subdirectory = (char*)malloc(50 * sizeof(char));
	

    for(i=0; i < num_Monitors; i++)
    {
		// Reset metablhtwn
		offset = 0;
		int nwrite = 0 , nread = 0, readBytes = 0, writeBytes = 0, num_Countries = 0;

		for (int j = i; j <= counter-1; j+=num_Monitors)
		{	
			num_Countries++;
		}
		// Arxikopoihsh monitor
		MONITOR_init(&monitor[i], num_Countries);
		unsigned int position = 0;
		for (int j = i; j <= counter-1; j+=num_Monitors)
		{
			strcpy(monitor[i].countries[position],countries[j]);
			position++;
		}

		// Apostolh buffer size kai ari8moy xwrwn
		if((fd2[i] = open(fifo2[i], O_WRONLY)) < 0 )     {perror("Open fifo2-TravelMonitor");    return -1;}
		if((nwrite = write(fd2[i],&buffer_size, sizeof(int))) == -1)    {perror("write");   return -1;}
		if((nwrite = write(fd2[i],&monitor[i].numCountries, sizeof(int))) == -1)    {perror("write");   return -1;}
		for(int j=0; j < monitor[i].numCountries; j++)
		{
			strcpy(subdirectory,directoryName);

			// Pros8hkh '/' sto telos toy directory poy do8hke efoson leipei
			if(subdirectory[strlen(subdirectory)-1] !='/')
				strcat(subdirectory, "/");

			strcat(subdirectory,monitor[i].countries[j]);
			writeBytes = strlen(subdirectory) + 1;

			// Apostolh subdirectory
			if((nwrite = write(fd2[i],&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
			if((nwrite = write(fd2[i],subdirectory, writeBytes)) == -1)    {perror("write");   return -1;}
		}

		// Apostolh bloom_size
		if((nwrite = write(fd2[i],&bloom_size, sizeof(unsigned long))) == -1)    {perror("write");   return -1;}
		close(fd2[i]);

		// Lhpsh bloom filter
		if((fd1[i] = open(fifo1[i], O_RDONLY)) < 0 )     {perror("Open fifo1");    return -1;}
        if(read(fd1[i], &readBytes, sizeof(int)) < 0)     {perror("read");    return -1;}
		offset = 0;

		// Lhpsh toy bloom filter apo to Monitor
		// H lhpsh ginetai se tmhmata ta opoia ka8orizei to buffer_size
		// H metablhth offset krataei ka8e fora ton ari8mo twn bytes poy exoun hdh paralhf8ei
		while(offset < bloom[i].size)
		{
			if(offset + buffer_size > bloom[i].size)
			{
				if((nread = read(fd1[i], bloom[i].filter + offset, bloom[i].size - offset)) <= 0)     {perror("read");    return -1;}
				break;
			}
			if((nread = read(fd1[i], bloom[i].filter + offset, buffer_size)) <= 0)     {perror("read");    return -1;}
			offset += nread;
		}

		close(fd1[i]);

		readBytes = strlen(bloom[i].filter);

        fflush(stdout);
    }

	int requestNum=0;

	// Desmeysh mnhmhs gia ta requests
	Request* requests = (Request*)malloc(sizeof(Request));
	

	// Efoson den exei ypsw8ei h shmaia apo SIGINT/SIGQUIT h apo exit command
    while(!flag)
	{
		int nwrite = 0 , nread = 0, readBytes = 0, writeBytes = 0;
		printf("/");		
	
		// Diavasma entolhs xrhsth
		fgets(inner_menu_line,INNER_MENU_LINE_LENGTH-1,stdin);
		if (strcmp(inner_menu_line,"\n")==0)
		{
			continue;
		}
	
		// Anagnwrish entolhs
		command_name = strtok(inner_menu_line," \t\n");
		

		//  Voi8htiko command poy ka8arizei thn o8onh
		if(strcmp(command_name,"clear")==0)
			system("clear");
		
		// travelRequest //
		else if (strcmp(command_name,"travelRequest")==0)
		{
			// Diavasma orismatwn
			citizen_id=strtok(NULL," \t\n");
			date=strtok(NULL," \t\n");
			country_from=strtok(NULL," \t\n");
			country_to=strtok(NULL," \t\n");
			virus_name=strtok(NULL," \t\n");

			if(virus_name==NULL)
			{
				
				printf("Missing argument\n");
				
			}
			else
			{
				// Briskei th 8esh thw xwras ston pinaka ton Monitors
				int countryFromIndex = MONITOR_search_country(monitor, country_from, num_Monitors);
				int countryToIndex = MONITOR_search_country(monitor, country_to, num_Monitors);

				if (countryFromIndex == -1)
				{
					printf("This country(countryFrom) doesn't exist: %s \n", country_from);
					continue;
				}
				if (countryToIndex == -1)
				{
					printf("This country(countryTo) doesn't exist: %s \n", country_to);
					continue;
				}
			
				requestNum++;

				// Desmeysh mnhmhs request
				requests = (Request*)realloc(requests,requestNum * sizeof(Request));

				// Gemisma aparaithtwn stoixeiwn
				strcpy(requests[requestNum-1].countryTo,country_to);
				strcpy(requests[requestNum-1].virus_name,virus_name);
				strcpy(requests[requestNum-1].date,date);



				for(i=0;i<16;i++)
				{
					// Kataskeyh string poy tha ginei hash
					sprintf(bloom_string,"%s%s",citizen_id,virus_name);
					
					// Evresh theshs prepei na eksetastei an einai set					
					bit_position=BLOOM_hash(bloom_string,i);
					// Ean h thesh den einai set o citizen den einai emvoliasmenos

					if(BLOOM_get(&bloom[countryFromIndex],bit_position)==0)
					{
						
						printf("REQUEST REJECTED – YOU ARE NOT VACCINATED\n");
						int rejection=-2;
						if((fd2[countryToIndex] = open(fifo2[countryToIndex], O_WRONLY)) < 0 )     {perror("Open fifo2-TravelMonitor");    return -1;}
						if((nwrite = write(fd2[countryToIndex],&rejection, sizeof(int))) == -2)    {perror("write");   return -1;}
						close(fd2[countryToIndex]);

						requests[requestNum-1].outcome=0;
					
						// To i ginetai 0 kai ginetai break
						i=0;
						break;
						
					}
	
				
				}
				
				// An to i den einai 0 o citizen einai pithanws emvoliasmenos
				if(i!=0)
				{
				
					char reply[12];


					// Apostolh travelRequest command sto Monitor poy diaxeirizetai to countryFrom
					if((fd2[countryFromIndex] = open(fifo2[countryFromIndex], O_WRONLY)) < 0 )     {perror("Open fifo2-TravelMonitor");    return -1;}
					writeBytes = strlen(command_name) +1;
					if((nwrite = write(fd2[countryFromIndex],&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
					if((nwrite = write(fd2[countryFromIndex],command_name, writeBytes)) == -1)    {perror("write");   return -1;}

					// Apostolh citizen_id
					writeBytes = strlen(citizen_id) +1;
					if((nwrite = write(fd2[countryFromIndex],&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
					if((nwrite = write(fd2[countryFromIndex],citizen_id, writeBytes)) == -1)    {perror("write");   return -1;}

					// Aposolh onomatos ioy
					writeBytes = strlen(virus_name) +1;
					if((nwrite = write(fd2[countryFromIndex],&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
					if((nwrite = write(fd2[countryFromIndex],virus_name, writeBytes)) == -1)    {perror("write");   return -1;}
					close(fd2[countryFromIndex]);

					// Lhpsh apanthshs
					if((fd1[countryFromIndex] = open(fifo1[countryFromIndex], O_RDONLY)) < 0 )     {perror("Open fifo1-TravelMonitor");    return -1;}
					if(read(fd1[countryFromIndex],&readBytes, sizeof(int)) < 0)     {perror("read");    return -1;}
					if(read(fd1[countryFromIndex],reply, readBytes) < 0)     {perror("read");    return -1;}		// Lhpsh apanthshs

					if(strcmp(reply,"NO")==0)
					{
						// An h apanthsh einai arnhtikh
						close(fd1[countryFromIndex]);
						printf("REQUEST REJECTED – YOU ARE NOT VACCINATED\n");

						// Stelnete -2 sto Monitor poy diaxeirizetai to countryTo
						int rejection=-2;
						if((fd2[countryToIndex] = open(fifo2[countryToIndex], O_WRONLY)) < 0 )     {perror("Open fifo2-TravelMonitor");    return -1;}
						if((nwrite = write(fd2[countryToIndex],&rejection, sizeof(int))) == -2)    {perror("write");   return -1;}
						close(fd2[countryToIndex]);

						requests[requestNum-1].outcome=0;
					}
					else if(strcmp(reply,"YES")==0)
					{
						// An h apanthsh einai 8etikh
						int req_year, req_month, req_day;
						sscanf(date,"%d-%d-%d",&req_day,&req_month,&req_year);

						int vacc_day, vacc_month, vacc_year;

						// Lhpsh hmeromhnias
						if(read(fd1[countryFromIndex],&readBytes, sizeof(int)) < 0)     {perror("read");    return -1;}
						if(read(fd1[countryFromIndex],reply, readBytes) < 0)     {perror("read");    return -1;}		// Lhpsh subdirectory
						close(fd1[countryFromIndex]);
						sscanf(reply,"%d-%d-%d",&vacc_day,&vacc_month,&vacc_year);
						
						// Elegxos gia ton xrono toy emvoliasmoy
						if (req_day < vacc_day)
						{
							req_day += 30;
							req_month --;
						}
						if (req_month < vacc_month)
						{
							req_month += 12;
							req_year --;
						}
						
						vacc_day=req_day-vacc_day;
						vacc_month=req_month-vacc_month;
						vacc_year=req_year-vacc_year;

						if (vacc_year >0)
						{
							// Den emvoliasthke ton teleytaio xrono
							printf("REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE\n");
							int rejection=-2;
							if((fd2[countryToIndex] = open(fifo2[countryToIndex], O_WRONLY)) < 0 )     {perror("Open fifo2-TravelMonitor");    return -1;}
							if((nwrite = write(fd2[countryToIndex],&rejection, sizeof(int))) == -2)    {perror("write");   return -1;}
							close(fd2[countryToIndex]);

							requests[requestNum-1].outcome=0;

						}
						else if(vacc_year == 0)
						{
							if (vacc_month > 6 ||(vacc_month == 6 && vacc_day > 0))
							{
								// Den emvoliasthke edw kai 6 mhnes
								printf("REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE\n");


								// Stelnete -2 sto Monitor poy diaxeirizetai to countryTo
								int rejection=-2;
								if((fd2[countryToIndex] = open(fifo2[countryToIndex], O_WRONLY)) < 0 )     {perror("Open fifo2-TravelMonitor");    return -1;}
								if((nwrite = write(fd2[countryToIndex],&rejection, sizeof(int))) == -2)    {perror("write");   return -1;}
								close(fd2[countryToIndex]);

								requests[requestNum-1].outcome=0;

							}
							else if (vacc_month < 6 ||(vacc_month == 6 && vacc_day == 0))
							{
								// Emvoliasthke toys teleytaioys 6 mhnes
								printf("REQUEST ACCEPTED – HAPPY TRAVELS\n");

								// Stelnete -1 sto Monitor poy diaxeirizetai to countryTo
								int acceptance=-1;
								if((fd2[countryToIndex] = open(fifo2[countryToIndex], O_WRONLY)) < 0 )     {perror("Open fifo2-TravelMonitor");    return -1;}
								if((nwrite = write(fd2[countryToIndex],&acceptance, sizeof(int))) == -2)    {perror("write");   return -1;}
								close(fd2[countryToIndex]);

								requests[requestNum-1].outcome=1;

							}
						}
						else
						{
							printf("Person vaccinated on a date after the intended trip!!! Program calculates 6 months in the past from the date given\n");

							// Stelnete -2 sto Monitor poy diaxeirizetai to countryTo
							int rejection=-2;
							if((fd2[countryToIndex] = open(fifo2[countryToIndex], O_WRONLY)) < 0 )     {perror("Open fifo2-TravelMonitor");    return -1;}
							if((nwrite = write(fd2[countryToIndex],&rejection, sizeof(int))) == -2)    {perror("write");   return -1;}
							close(fd2[countryToIndex]);
							requests[requestNum-1].outcome=0;
						}
						
								
					}
				}

			}

		}
		// travelStats //
		else if(strcmp(command_name,"travelStats")==0)
		{
			// Diavasma orismatwn
			virus_name=strtok(NULL," \t\n");
			date=strtok(NULL," \t\n");
			date2=strtok(NULL," \t\n");
			country_to=strtok(NULL," \t\n");

			// Elegxos gia elliph orismata
			if(date2==NULL)
			{
				
				printf("Missing argument\n");
				continue;

			}

			// An den exoyn ginei akoma requests
			if (requestNum == 0)
			{
				printf("No requests yet!\n");
				continue;
			}

			int accepts=0, rejects=0;			

			// Gia oles tis xwres
			if (country_to == NULL)
			{

				// Gia ola ta requests
				for (int i = 0; i < requestNum; i++)
				{
					// An o ios einai o swstos
					if(strcmp(requests[i].virus_name,virus_name) == 0)
					{
						// An h hmeromhnia toy request einai anamesa sta dates poy do8hkan
						if(is_between_date(requests[i].date,date,date2))
						{
							// An to request egine accept
							if(requests[i].outcome == 1)
							{
								accepts ++;
							}
							// An to request egine reject
							else
							{
								rejects ++;
							}
							
						}
					}
				}
				
				printf("TOTAL REQUESTS %d\n",accepts+rejects);
				printf("ACCEPTED %d\n",accepts);
				printf("REJECTED %d\n",rejects);
				
			}
			else
			{
				// Gia oles tis xwres


				for (int i = 0; i < requestNum; i++)
				{
					// An h xwra einai h swsth
					if(strcmp(requests[i].countryTo,country_to) == 0)
					{
						// An o ios einai o swstos
						if(strcmp(requests[i].virus_name,virus_name) == 0)
						{
							// An h hmeromhnia toy request einai anamesa stis hmeromhnies poy do8hkan
							if(is_between_date(requests[i].date,date,date2))
							{
								// An to request egine accept
								if(requests[i].outcome == 1)
								{
									accepts ++;
								}
								// An to request egine reject
								else
								{
									rejects ++;
								}
								
							}
						}
					}
				}
				
				printf("TOTAL REQUESTS %d\n",accepts+rejects);
				printf("ACCEPTED %d\n",accepts);
				printf("REJECTED %d\n",rejects);


			}

		}

		// searchVaccinationStatus //
		else if(strcmp(command_name,"searchVaccinationStatus")==0)
		{
			// Diavasma orismatos
			citizen_id=strtok(NULL," \t\n");

			if(citizen_id == NULL)
			{
				printf("Missing citizen_id argument\n");
				continue;
			}

			writeBytes = strlen(command_name) +1;
			int citizen_id_length = strlen(citizen_id) +1;
			for(i=0; i < num_Monitors; i++)
			{
				// Apostolh searchVaccinationStatus command sta Monitors
				if((fd2[i] = open(fifo2[i], O_WRONLY)) < 0 )     {perror("Open fifo2-TravelMonitor");    return -1;}
				if((nwrite = write(fd2[i],&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
				if((nwrite = write(fd2[i],command_name, writeBytes)) == -1)    {perror("write");   return -1;}
				
				// Apostolh citizen_id sta Monitors
				if((nwrite = write(fd2[i],&citizen_id_length, sizeof(int))) == -1)    {perror("write");   return -1;}
				if((nwrite = write(fd2[i],citizen_id, writeBytes)) == -1)    {perror("write");   return -1;}
				close(fd2[i]);

				
			}

			int age_of_citizen;
			fd_set fds;
			struct timeval tv;

			// timeout gia thn select
			tv.tv_usec = 50000;  
			tv.tv_sec = 0;
			int select_success_flag = 0;
			int num_of_viruses = 0;

			FD_ZERO(&fds);

			for (int i = 0; i < num_Monitors; i++)
			{
				if((fd1[i] = open(fifo1[i], O_RDONLY | O_NONBLOCK)) < 0 )     {perror("Open fifo1TravelMonitor");    return -1;}
				FD_SET(fd1[i],&fds);
			}
			select(fd1[num_Monitors-1]+1, &fds, NULL, NULL, &tv);
			for (int i = 0; i < num_Monitors; i++)
			{
				if (FD_ISSET(fd1[i], &fds))
				{
					// Lhpsh first_name
					if(read(fd1[i], &readBytes, sizeof(int)) < 0)     {perror("read");    return -1;}
					first_name=(char*)malloc(readBytes*sizeof(char));
					if(read(fd1[i], first_name, readBytes) < 0)     {perror("read");    return -1;}	
					
					//Lhpsh last_name 
					if(read(fd1[i], &readBytes, sizeof(int)) < 0)     {perror("read");    return -1;}
					last_name=(char*)malloc(readBytes*sizeof(char));
					if(read(fd1[i], last_name, readBytes) < 0)     {perror("read");    return -1;}	

					//Lhpsh xwras 
					if(read(fd1[i], &readBytes, sizeof(int)) < 0)     {perror("read");    return -1;}
					country_from=(char*)malloc(readBytes*sizeof(char));
					if(read(fd1[i], country_from, readBytes) < 0)     {perror("read");    return -1;}	

					// Lhpsh hlikias
					if(read(fd1[i], &age_of_citizen, sizeof(int)) < 0)     {perror("read");    return -1;}

					// Lhpsh ari8moy iwn
					if(read(fd1[i], &num_of_viruses, sizeof(int)) < 0)     {perror("read");    return -1;}

					// Shmaia epityxias select ginetai 1
					select_success_flag = 1;


					printf("%s %s %s %s\n",citizen_id, first_name, last_name, country_from);
					printf("AGE %d\n",age_of_citizen);

					// Afairesh O_NONBLOCK flag apo to fd
					int flags = fcntl(fd1[i], F_GETFL);
					flags &= ~O_NONBLOCK;
					fcntl(fd1[i], F_SETFL, flags);

					for(int j=0;j<num_of_viruses;j++)
					{
						unsigned short reply_bool;
						//Lhpsh ioy 
						if(read(fd1[i], &readBytes, sizeof(int)) < 0)     {perror("read");    return -1;}
						virus_name=(char*)malloc(readBytes*sizeof(char));
						if(read(fd1[i], virus_name, readBytes) < 0)     {perror("read");    return -1;}	


						// lhpsh apanthshs emvoliasmoy
						if(read(fd1[i], &reply_bool, sizeof(short)) < 0)     {perror("read");    return -1;}
						if (reply_bool)
						{
							//Lhpsh hmeromhnias emvoliasmoy 
							if(read(fd1[i], &readBytes, sizeof(int)) < 0)     {perror("read");    return -1;}
							date=(char*)malloc(readBytes*sizeof(char));
							if(read(fd1[i], date, readBytes) < 0)     {perror("read");    return -1;}	

							printf("%s  VACCINATED ON  %s\n",virus_name , date);

							free(date);
						}
						else
						{
							printf("%s  NOT YET VACCINATED\n",virus_name);
						}
						
						
						free(virus_name);
					}

					break;
				}
				
			}
			for (int i = 0; i < num_Monitors; i++)
			{
				close(fd1[i]);
			}

			// Eleftherwsh ths mnhmhs an exei desmef8ei
			if(select_success_flag)
			{
				free(first_name);
				free(last_name);
				free(country_from);
			}


		}
		// exit //
		else if(strcmp(command_name,"exit")==0)
		{
			// apostolh SIGKILL sta Monitors
			for (int i = 0; i < num_Monitors; i++)
			{
				kill(pid[i],SIGKILL);
			}
			// Ypswsh shmaias
			flag=1;
		}

        	
		// softExit // Boh8htiko command gia omalo exit
		else if(strcmp(command_name,"softExit")==0)
		{
			writeBytes = strlen(command_name) +1;
			for(i=0; i < num_Monitors; i++)
			{
				// Apostolh exit command
				if((fd2[i] = open(fifo2[i], O_WRONLY)) < 0 )     {perror("Open fifo2-TravelMonitor");    return -1;}
				if((nwrite = write(fd2[i],&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
				if((nwrite = write(fd2[i],command_name, writeBytes)) == -1)    {perror("write");   return -1;}
				close(fd2[i]);
				
			}

			break;
		}
		// printMonitors // Boh8htiko command
		else if(strcmp(command_name,"printMonitors")==0)
		{
			MONITOR_print_all(monitor,num_Monitors);
		}

		// continue // Boh8htiko command
		else if(strcmp(command_name,"continue")==0)
		{
			writeBytes = strlen(command_name) +1;
			for(i=0; i < num_Monitors; i++)
			{
				// Apostolh continue command
				if((fd2[i] = open(fifo2[i], O_WRONLY)) < 0 )     {perror("Open fifo2-TravelMonitor");    return -1;}
				if((nwrite = write(fd2[i],&writeBytes, sizeof(int))) == -1)    {perror("write");   return -1;}
				if((nwrite = write(fd2[i],command_name, writeBytes)) == -1)    {perror("write");   return -1;}
				close(fd2[i]);
			}
			printf("CONTINUE\n");
			continue;
		}
    }



	if(flag)
	{
		// To travel Monitor perimenei ta paidia toy na oloklhrwsoyn
		for (int i = 0; i < num_Monitors; i++)
		{
			kill(pid[i],SIGKILL);
			wait(NULL);
		}
		int accepts=0;
		int rejects=0;

		// Metra ta accepts/rejects
		for (int i = 0; i < requestNum; i++)
		{
			if (requests[i].outcome ==1 )
			{
				accepts ++;
			}
			else
			{
				rejects ++;
			}
		
		}

		// Dhmioyrgia log_file.xxx arxeioy
		char log_file[20];
		sprintf(log_file,"log_file.%d",getpid());
		FILE* fp = fopen(log_file,"w+");

		// Eisagwgh ka8e xwras sto log_file
		for (int i = 0; i < counter; i++)
		{
			fputs(countries[i],fp);
			fputs("\n",fp);

		}

		// Eisagwgh statistikwn sto log_file
		fputs("\nTOTAL TRAVEL REQUESTS ",fp);
		fprintf(fp,"%d\n",requestNum);	
		fputs("ACCEPTED ",fp);
		fprintf(fp,"%d\n",accepts);	
		fputs("REJECTS ",fp);
		fprintf(fp,"%d\n",rejects);	
		fclose(fp);
		printf("logfile: %s \n",log_file);
	}
	else
	{
		for (int i = 0; i < num_Monitors; i++)
		{
			wait(NULL);
		}
	}

	

	// Elef8erwsh mnhmhs kai desmeymenwn domwn
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
		BLOOM_destroy(&bloom[i]);
		MONITOR_destroy(&monitor[i]);
	}
	free(requests);

	free(bloom);
	free(monitor);
	free(fifo1);
	free(fifo2);
	free(subdirectory);
	
	free(fd1);
	free(fd2);

    return 0;
}

void int_quit_parent()
{
	flag=1;
}