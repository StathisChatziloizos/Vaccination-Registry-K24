#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#include "hash.h"
#include "bst.h"
#include "bloom.h"
#include "skip_list.h"

#define INNER_MENU_LINE_LENGTH 200
#define FILE_LINE_LENGTH 200
#define NUM_OF_BUCKETS 50
#define BLOOM_STRING_MAX_LENGTH 50

int main(int argc,char* argv[])
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

	
	// Elegxoi orismatwn ekteleshs programmatos
	if(argc!=5)
	{
		printf("Usage: vaccineMonitor -c citizenRecordsFile –b bloomSize\n");
		return 0;	
	}
	
	if(strcmp(argv[1],"-c")!=0&&strcmp(argv[3],"-b")!=0)
	{
		if(strcmp(argv[1],"-b")!=0&&strcmp(argv[3],"-c")!=0)
		{
			printf("Usage: vaccineMonitor -c citizenRecordsFile –b bloomSize\n");
			return 0;
		}		
	}
	
	// Anoigma arxeioy ka8orismos mege8ous bloom filtroy
	if(strcmp(argv[1],"-c")==0)
	{
		
		fp=fopen(argv[2],"r");
		bloom_size=(unsigned long)(atol(argv[4]));
				
	}
	else
	{		
		fp=fopen(argv[4],"r");
		bloom_size=(unsigned long)(atol(argv[2]));
	}
	
	// Elegxos anoigmatos arxeioy
	if(fp==NULL)
	{
		
		printf("Open file error\n");
		return 0;
		
	}
	
	// Elegxos megethoys bloom filtroy
	if(bloom_size<=0){
		
		printf("Bloom filter length error\n");
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
		// citizen_record=HASH_add_patient(&record_hash_table,file_line);
		
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
	

	// Menou lhpshs ths epomenhs entolhs apo ton xrhsth
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
		// vaccineStatus //
		else if(strcmp(command_name,"vaccineStatus")==0)
		{			
			
			citizen_id=strtok(NULL," \t\n");
			virus_name=strtok(NULL," \t\n");
			
			// Elegxos gia elliph orismata
			if(citizen_id==NULL){
				
				printf("Missing argument\n");
				
			}
			else{
				
				// Periptwsh poy exei dothei onoma ioy
				if(virus_name)
				{
				
					// Anazhtoyme ton komvo toy ioy sto dentro
					virus_node=BST_search(virus_tree,virus_name);
					
					// An brethei kanoyme anazhthsh sthn antistoixh skip list
					if(virus_node)
					{
						
						skip_lst_node=SKIP_LIST_search(virus_node->vacc_list,atoi(citizen_id));
						
						//  Emfanizoyme to apotelesma ths anazhthshs
						if(skip_lst_node)
						{
							
							printf("VACCINATED ON %s\n",skip_lst_node->date);
							
						}
						else
						{
							
							printf("NOT VACCINATED\n");							
							
						}					
						
					}
					else
					{
					// Edw einai h periptwsh poy den exei brethei o ios
						printf("Can't find virus\n");						
					}
								
				}
				else{
					
					// Periptwsh poy den exei dothei onoma ioy
					
					
					// Anazhthsh stis skil lists olwn twn iwn
					for(i=0;i<num_of_viruses;i++)
					{
										
						skip_lst_node=SKIP_LIST_search(array_of_viruses[i]->vacc_list,atoi(citizen_id));
						
						//  Emfanizoyme to apotelesma ths anazhthshs
						if(skip_lst_node)
						{
							
							printf("%s YES %s\n",array_of_viruses[i]->virus,skip_lst_node->date);
							
						}
						else{
							
							printf("%s NO\n",array_of_viruses[i]->virus);//skip_lst_node->date);
													
						}									
					}					
				}
			}			
		}
		// populationStatus //
		else if(strcmp(command_name,"populationStatus")==0)
		{
			
			country_name=strtok(NULL," \t\n"); 
			virus_name=strtok(NULL," \t\n"); 
			start_date=strtok(NULL," \t\n"); 
			end_date=strtok(NULL," \t\n");
	
			// Elegxos gia elliph orismata
			if(start_date==NULL)
			{
				
				printf("Missing argument\n");
				
			}
			else
			{
				
				// An to end_date einai NULL sthn pragmatikothta shmainei ot den do8hke country
				// Sthn periptwsh ayth apoka8istoyme thn or8h ekxwrhsh
				if(end_date==NULL)
				{
					end_date=start_date;
					start_date=virus_name;
					virus_name=country_name;
					country_name=NULL;
				}
					
				// Anazhtoyme ton komvo toy ioy sto dentro
				virus_node=BST_search(virus_tree,virus_name);

				// An brethei kanoyme anazhthsh sthn antistoixh skip list
				if(virus_node)
				{
					// An dw8hke country
					if(country_name)
					{
						// Komvos sthn skip list
						skip_node* current;

						record* rec;

						// Komvos ths country list
						country_node* country;
						int countryPopulation;
						int vaccinatedPopulation=0;
						
						// Metablhth poy deixnei an to search htan epityxes h an apla gyrise to teleytaio country node
						int is_last=0;

						// Psaxnoyme ton komvo ths xwras me to swsto onoma
						country=COUNTRIES_search(head, country_name, &is_last);
						if(is_last)
						{
						// An htan apotyxhmeno to search
							printf("Country not found\n");
							continue;
						}
						// Synolikos emvoliasmenos ply8hsmos
						countryPopulation=country->age0+country->age20+country->age40+country->age60;

						current=virus_node->vacc_list;
						while(current!=NULL)
						{
							rec=current->record_ptr;
							if(strcmp(country_name,rec->country_name)==0)
							{
							// An einai h swsth xwra ayksanoyme ton counter twn emvoliasmenwn	
								vaccinatedPopulation++;
							}
							// H diasxish ginetai sto epipedo vashs
							current=current->next_nodes[0];
						}
						printf("%s %d %.2f%%\n", country_name, vaccinatedPopulation, 100*(float)((float)vaccinatedPopulation/countryPopulation));
					}
					else
					{
					// Edw einai h periptwsh poy den exei dwthei country

						// Komvos sthn skip list 
						skip_node* current;
						record* rec;

						// Komvos sthn country list
						country_node* country;
						int countryPopulation;
						int vaccinatedPopulation;

						// O komvos country list deixnei sthn kefalh ths listas
						country=head;

						while(country!=NULL)
						{
						// Diasxish ths country listas ews to to telos ths
						
							vaccinatedPopulation=0;
							countryPopulation=country->age0+country->age20+country->age40+country->age60;

							// Komvos deixnei sthn kefalh ths swsths skip list
							current=virus_node->vacc_list;

							while(current!=NULL)
							{
								rec=current->record_ptr;

								// An einai h swsth xwra ayksanoyme ton counter twn emvoliasmenwn	
								if(strcmp(country->country_name,rec->country_name)==0)
								{
									vaccinatedPopulation++;
								}
								// H diasxish ginetai sto epipedo vashs
								current=current->next_nodes[0];
							}

							printf("%s %d %.2f%%\n", country->country_name, vaccinatedPopulation, 100*(float)((float)vaccinatedPopulation/countryPopulation));

							// Enhmerwnoyme ton komvo listas na deixnei ston epomeno komvo
							country=country->next_node;
						}
					}
				}
				else
				{
				// Edw einai h periptwsh poy den exei brethei o ios
					printf("Can't find virus\n");
					
				}								
			}
			
		}
		// popStatusByAge // 
		else if(strcmp(command_name,"popStatusByAge")==0)
		{
						
			country_name=strtok(NULL," \t\n"); 
			virus_name=strtok(NULL," \t\n"); 
			start_date=strtok(NULL," \t\n"); 
			end_date=strtok(NULL," \t\n");
	
			// Elegxos gia elliph orismata
			if(start_date==NULL)
			{
				
				printf("Missing argument\n");
				
			}
			else
			{
				
				// An to end_date einai NULL sthn pragmatikothta shmainei ot den do8hke caountry
				// Sthn periptwsh ayth apoka8istoyme thn or8h ekxwrhsh
				if(end_date==NULL)
				{
					end_date=start_date;
					start_date=virus_name;
					virus_name=country_name;
					country_name=NULL;
					
				}
				
									
				// Anazhtoyme ton komvo toy ioy sto dentro
				virus_node=BST_search(virus_tree,virus_name);

				// An brethei kanoyme anazhthsh sthn antistoixh skip list
				if(virus_node)
				{
					// An dw8hke country
					if(country_name)
					{
						// Komvos sthn skip list
						skip_node* current;

						record* rec;

						// Komvos ths country list
						country_node* country;

						// Metrhtes emvoliasmenwn ana hlikiako group (0-20,21-40,41-60,61-120)
						int vaccinatedPopulation0=0;
						int vaccinatedPopulation20=0;
						int vaccinatedPopulation40=0;
						int vaccinatedPopulation60=0;

						// Metablhth poy deixnei an to search htan epityxes h an apla gyrise to teleytaio country node
						int is_last=0;

						// Psaxnoyme ton komvo ths xwras me to swsto onoma
						country=COUNTRIES_search(head, country_name, &is_last);
						if(is_last)
						{
						// An htan apotyxhmeno to search
							printf("Country not found\n");
							continue;
						}

						current=virus_node->vacc_list;
						while(current!=NULL)
						{
							rec=current->record_ptr;
							if(strcmp(country_name,rec->country_name)==0)
							{
							// An einai h swsth xwra ayksanoyme ton swsto counter twn emvoliasmenwn
							// analoga me thn hlikia 

								if(rec->age<=20)
									vaccinatedPopulation0++;
								else if(rec->age<=40)
									vaccinatedPopulation20++;
								else if(rec->age<=60)
									vaccinatedPopulation40++;
								else if(rec->age<=120)
									vaccinatedPopulation60++;
							}
							// H diasxish ginetai sto epipedo vashs
							current=current->next_nodes[0];
						}
						printf("%s\n",country_name);
						printf("0-20 %d %.2f%%\n", vaccinatedPopulation0, 100.0*(float)((float)vaccinatedPopulation0/country->age0));
						printf("20-40 %d %.2f%%\n", vaccinatedPopulation20, 100.0*(float)((float)vaccinatedPopulation20/country->age20));
						printf("40-60 %d %.2f%%\n", vaccinatedPopulation40, 100.0*(float)((float)vaccinatedPopulation40/country->age40));
						printf("60+ %d %.2f%%\n", vaccinatedPopulation60, 100.0*(float)((float)vaccinatedPopulation60/country->age60));
					}
					else
					{
					// Edw einai h periptwsh poy den exei dwthei country

						// Komvos sthn skip list 
						skip_node* current;
						record* rec;

						// Komvos sthn country list
						country_node* country;

						// Metrhtes emvoliasmenwn ana hlikiako group (0-20,21-40,41-60,61-120)
						int vaccinatedPopulation0=0;
						int vaccinatedPopulation20=0;
						int vaccinatedPopulation40=0;
						int vaccinatedPopulation60=0;

						// O komvos country list deixnei sthn kefalh ths listas
						country=head;

						while(country!=NULL)
						{
						// Diasxish ths country listas ews to to telos ths

						vaccinatedPopulation0=0;
						vaccinatedPopulation20=0;
						vaccinatedPopulation40=0;
						vaccinatedPopulation60=0;

							// Komvos deixnei sthn kefalh ths swsths skip list
							current=virus_node->vacc_list;

							while(current!=NULL)
							{
								rec=current->record_ptr;

								// An einai h swsth xwra ayksanoyme ton swsto counter twn emvoliasmenwn
								// analoga me thn hlikia

								if(strcmp(country->country_name,rec->country_name)==0)
								{
									if(rec->age<=20)
										vaccinatedPopulation0++;
									else if(rec->age<=40)
										vaccinatedPopulation20++;
									else if(rec->age<=60)
										vaccinatedPopulation40++;
									else if(rec->age<=120)
										vaccinatedPopulation60++;
								}
								// H diasxish ginetai sto epipedo vashs
								current=current->next_nodes[0];
							}

							printf("%s\n",country->country_name);
							printf("0-20 %d %.2f%%\n", vaccinatedPopulation0, 100.0*(float)((float)vaccinatedPopulation0/country->age0));
							printf("20-40 %d %.2f%%\n", vaccinatedPopulation20, 100.0*(float)((float)vaccinatedPopulation20/country->age20));
							printf("40-60 %d %.2f%%\n", vaccinatedPopulation40, 100.0*(float)((float)vaccinatedPopulation40/country->age40));
							printf("60+ %d %.2f%%\n\n", vaccinatedPopulation60, 100.0*(float)((float)vaccinatedPopulation60/country->age60));

							// Enhmerwnoyme ton komvo listas na deixnei ston epomeno komvo
							country=country->next_node;
						}

					}
				}
				else
				{
				// Edw einai h periptwsh poy den exei brethei o ios
					printf("Can't find virus\n");
					
				}
				
				
			}
			
		}
		// insertCitizenRecord //
		else if(strcmp(command_name,"insertCitizenRecord")==0)
		{
			

			citizen_id=strtok(NULL," \t\n"); 
			first_name=strtok(NULL," \t\n"); 
			last_name=strtok(NULL," \t\n"); 
			country_name=strtok(NULL," \t\n"); 
			age=strtok(NULL," \t\n"); 
			virus_name=strtok(NULL," \t\n"); 
			vaccination_condition=strtok(NULL," \t\n"); 
			date=strtok(NULL," \t\n");

			// string me tis plhrofories toy citizen
			char citizen_string[200];

			if (date)
			{
				// An dothike hmeromhnia
				sprintf(citizen_string,"%s %s %s %s %s %s %s %s\n",citizen_id, first_name, last_name, country_name, age, virus_name, vaccination_condition, date);
			}
			else
			{
				// An den dothike hmeromhnia
				sprintf(citizen_string,"%s %s %s %s %s %s %s\n",citizen_id, first_name, last_name, country_name, age, virus_name, vaccination_condition);
			}
			
			// An to record den iparxei tote pros8etoyme ton asthenh sto country list
			if(HASH_exists(&record_hash_table, citizen_id)==NULL)
				COUNTRIES_list_insertLast(&head,country_name, atoi(age));

			citizen_record=HASH_add_patient(&record_hash_table,citizen_string);	
						
			// Elegxos gia elliph orismata
			if(vaccination_condition==NULL)
			{
				
				printf("Missing argument\n");
				
			}
			else
			{
					
				virus_node=BST_search(virus_tree,virus_name);			
				
				if(virus_node==NULL)
				{
					virus_node=BST_insert(&virus_tree,virus_name);	
					num_of_viruses++;	

					// Efoson exei eisaxthei neos ios prepei na ginei anakataskeyh toy pinaka				
						
					free(array_of_viruses);
					
					array_of_viruses=malloc(num_of_viruses*sizeof(node*));
	
					BST_fill_array(virus_tree,array_of_viruses);
		
					
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
			
		}	
		// vaccinateNow //		
		else if(strcmp(command_name,"vaccinateNow")==0){
			
						
			citizen_id=strtok(NULL," \t\n"); 
			first_name=strtok(NULL," \t\n"); 
			last_name=strtok(NULL," \t\n"); 
			country_name=strtok(NULL," \t\n"); 
			age=strtok(NULL," \t\n"); 
			virus_name=strtok(NULL," \t\n"); 
			
			// Elegxos gia elliph orismata
			if(virus_name==NULL)
			{
				
				printf("Missing argument\n");
				
			}
			else
			{
				
				printf("Den ylopoih8hke....\n");
				
			}			
		
		}
		// list-nonVaccinated-Persons //
		else if(strcmp(command_name,"list-nonVaccinated-Persons")==0)
		{
			virus_name=strtok(NULL," \t\n");

			if(virus_name==NULL)		
			{	
				printf("Missing argument\n");	
			}
			else
			{
				// Anazhtoyme ton komvo toy ioy sto dentro
				virus_node=BST_search(virus_tree,virus_name);
				
				// An brethei kanoyme anazhthsh sthn antistoixh skip list
				if(virus_node)
				{
					
					SKIP_LIST_print(virus_node->not_vacc_list);
					
				}
				else
				{
				// Edw einai h periptwsh poy den exei brethei o ios
					printf("Can't find virus\n");
					
				}
				printf("Tested for virus: %s\n", virus_name);
			}
		}
		else if(strcmp(command_name,"printCountries")==0)
		{
		// Voi8htiko command, typwnei ka8e xwra, toys ply8hsmoys ana hlikiako
		// group kai ton synoliko ply8hsmo ths xwras 
			COUNTRIES_print_list(head);
		}
		
		// exit //
		else if(strcmp(command_name,"exit")==0)
			break;
		
	}
	

	// Eley8erwsh mnhmhs
	HASH_clear(&record_hash_table);
	BST_destroy(virus_tree);
	BLOOM_destroy(&bloom);
	COUNTRIES_delete_list(head);
	
	return 0;
	
}
