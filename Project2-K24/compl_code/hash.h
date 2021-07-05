#ifndef _HASH_
#define _HASH_

typedef struct record{
	
	// Pedia eggrafwn
	char* citizen_id;
	char* first_name;
	char* last_name;
	char* country_name;
	int age;	
	
}record;

typedef struct bucket_record{
	
	// Eggrafh asthenoys
	record* patient_record;
		
	// Epomenh eggrafh
	struct bucket_record* next;	
	
}bucket_record;

typedef struct bucket{
	
	// Plh8os eggrafwn kadoy
	int num_of_records;
	
	// Deikths sthn prwth eggrafh
	bucket_record* first_record;	
	
}bucket;

typedef struct hash_table
{
	
	// Plh8os kadwn
	int num_of_buckets;
	
	// O pinakas katakermatismoy
	bucket* buckets;	
	
}hash_table;

typedef struct country_node
{
	char country_name[32];
	// Plh8ismos ews 20
	int age0;

	// Plh8ismos 20-40
	int age20;

	// Plh8ismos 20-40
	int age40;

	// Plh8ismos 40-60
	int age60;

	// Epomenos komvos
	struct country_node* next_node;
}country_node;

// H synarthsh arxikopoiei enan komvo country_node kai ton epistrefei
country_node* COUNTRIES_init(char* country_name);

// Ayksanei ton metrhth enos komvoy country_node
void COUNTRIES_add(country_node* node, int age);

// Psaxnei se mia lista apo komvoys country node kai epistrefei ayton me to swsto
// onoma ths xwras. An den bre8ei epistrefei ton teleftaio kai enhmerwnei thn shmaia is_last
country_node* COUNTRIES_search(country_node* head,char* country_name, int* is_last);

// Psaxnei ton komvo me th swsth xwra kai ayksanei ton metrhth me bash thn hlikia
// An den brethei ftiaxnei enan kainoyrgio komvo kai ton prosarta sto telos ths listas
void COUNTRIES_list_insertLast(country_node** head, char* country_name, int age);

// Voi8htikh synarthsh poy typwnei thn lista apo xwres
void COUNTRIES_print_list(country_node* head);

// Diagrafei toys komvoys ths listas kai eleftherwnei thn mnhmh toys
void COUNTRIES_delete_list(country_node* head);

// H synarthsh arxikopoiei enan pinaka katakermatismou 
void HASH_init(hash_table* h_table, int num_of_buckets);

// Eisagei kai epistrefei enan asthenh ston pinaka katakermatismoy
record* HASH_add_patient(hash_table* h_table,char* patient);

// Epistrefei to record enos asthenh an yparxei ston pinaka katakermatismoy, diaforetika epistrefei NULL
record* HASH_exists(hash_table* h_table,char* citizen_id);

// Diagrafei enan asthenh apo ton pinaka katakermatismoy
void HASH_delete(hash_table* h_table,char* citizen_id);

// Apeley8erwsh mnhmhs hash table
void HASH_clear(hash_table* h_table);

// Emfnish table
int HASH_print(hash_table* h_table);

#endif
