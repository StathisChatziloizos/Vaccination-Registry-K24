#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "bst.h"

// H synarthsh arxikopoiei ena dyadiko dentro 
void BST_init(Tree* addressOfBst)
{

	*addressOfBst = NULL;

}

// Eisagei kai epistrefei neo komvo sto dyadiko dentro 
node* BST_insert(Tree* addressOfBst, char* virus)
{
	
	node* parent;
	node* current;
	
	// An eisagetai to prwto stoixeio, apla dhmioyrgoyme ena
	// komvo kai 8etoyme th riza ish me ton komvo ayto
	if(*addressOfBst==NULL)
	{
		
		// Desmeysh mnhmhs gia ton komvo
		*addressOfBst = malloc(sizeof(node));
		
		// Desmeysh mnhmhs gia to onoma toy ioy poy tha periexei
		(*addressOfBst)->virus=malloc(strlen(virus)+1);
		
		// Apothikeysh toy ioy poy
		strcpy((*addressOfBst)->virus,virus);
		
		// Arxikopoihsh ths skip list twn emvoliasmenwn
		(*addressOfBst)->vacc_list=NULL;
		
		// Arxikopoihsh ths skip list twn mh emvoliasmenwn
		(*addressOfBst)->not_vacc_list=NULL;
		
		// O aristeros komvos ti8etai NULL
		(*addressOfBst)->left=NULL;
		
		// O deksios komvos ti8etai NULL
		(*addressOfBst)->right=NULL;
		
		// Epistrefetai o komvos
		return *addressOfBst;
		
	}
	else
	{
		
		// Diaforetika, o neos komvos 8a mpei ws fyllo 
		// Epomenws prepei na diasxisoyme to dentro katakoryfa 
		// kai na vroume thn katallhlh 8esh eisagwghs
		
		current = *addressOfBst;
		
		while(current!=NULL)
		{
			
			//Se periptwsh poy yparxei o komvos toy ioy epistrefetai
			if(strcmp(virus,current->virus)==0)
			{

				return current;
				
			}			
			// An to data einai mikrotero toy trexontos komvoy phgaine aristera			
			else if(strcmp(virus,current->virus)<0)
			{

				parent = current;
				current = current->left;
				
			}
			// Alliws phgaine deksia
			else
			{

				parent = current;
				current = current->right;		
			
			}
		
		}
		
		// Ftasame se fyllo, opote dhmioyrgoyme ton neo komvo 
		// Otan bgoume apo to while, kseroume ton patera 
		
		
		// Desmeysh mnhmhs gia ton komvo
		current = malloc(sizeof(node));
		
		// Desmeysh mnhmhs gia to onoma toy ioy poy tha periexei
		current->virus=malloc(strlen(virus)+1);
		
		// Apothikeysh toy ioy poy
		strcpy(current->virus,virus);
		
		// Arxikopoihsh ths skip list twn emvoliasmenwn
		current->vacc_list=NULL;
		
		// Arxikopoihsh ths skip list twn mh emvoliasmenwn
		current->not_vacc_list=NULL;
		
		// O aristeros komvos ti8etai NULL
		current->left=NULL;
		
		// O deksios komvos ti8etai NULL
		current->right=NULL;
		
		if(strcmp(virus,parent->virus)<0)
			// Aristero "kremasma"
			parent->left = current;	
		else
			// Deksio "kremasma"
			parent->right = current;	
		
		// Epistrefetai o komvos
		return current;
		
	}

}

// Anazhta kai epistrefei komvo sto dyadiko dentro 
node* BST_search(Tree bstTree, char* virus){
	
	node* current = bstTree;
	
	while(current!=NULL)
	{
		
		// An brhkame τον ιο epistrefoyme τον κομωο
		if(strcmp(virus,current->virus)==0)
		{
			return current;
		}
		// Diaforetika an to virus einai mikrotero toy trexontos komvoy pame aristera 
		else if(strcmp(virus,current->virus)<0)
		{
			current = current->left;			
		}
		// Alliws pame deksia. 
		else
		{
			current = current->right;		
		
		}
	
	}
	
	// Se periptwsh poy den to broyme epistrefoyme NULL 
	return NULL;
	
}

// Anadromikh katastrofh dentroy
void BST_destroy(Tree bstSubTree)
{
	
	node* current = bstSubTree;
	
	if(current!=NULL)
	{
		
		// Anadromikh katastrofh toy aristeroy ypodentroy
		BST_destroy(current->left);
		
		// Anadromikh katastrofh toy deksioy ypodentroy
		BST_destroy(current->right);
		
		// Katastrofh ths rizas toy dentroy
		free(current->virus);
		
		free(current);

	}

}

// Anadromikh emfanish dentroy
void BST_print(Tree bstSubTree)
{
	
	node* current = bstSubTree;
	
	if(current!=NULL)
	{
		
		// Anadromikh katastrofh toy aristeroy ypodentroy
		BST_print(current->left);
		
		printf("%s\n",current->virus);
		
		// Anadromikh katastrofh toy deksioy ypodentroy
		BST_print(current->right);
		
		

	}

}


// Bohthhtikh synarthsh gia thn epomenh
void BST_fill_array_recursive(Tree bstSubTree, node** array, int* i){
	
	if(bstSubTree!=NULL){
		
		array[*i]=bstSubTree;
		
		(*i)++;
		
		BST_fill_array_recursive(bstSubTree->left,array,i);
		BST_fill_array_recursive(bstSubTree->right,array,i);		
		
	}
	
	
}


// Gemizei enan pinaka me toys ioys poy periexei. Oi ioi den antigrafontai. 
// Apla ekxwroyntai ston paragomeno pinaka oi antistoixes dieythynseis
void BST_fill_array(Tree bstSubTree, node** array){
	
	int i=0;
	
	BST_fill_array_recursive(bstSubTree,array,&i);
	
	
}