#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bloom.h"

// Dosmenos kwdikas

unsigned long djb2(unsigned char *str) 
{
	int c;
	unsigned long hash = 5381;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c; // hash * 33 + c

	return hash;
}

// This algorithm was created for sdbm (a public-domain reimplementation of ndbm) 
// database library. it was found to do well in scrambling bits, causing better 
// distribution of the keys and fewer splits. it also happens to be a good 
// general hashing function with good distribution. The actual function 
// is hash(i) = hash(i - 1) * 65599 + str[i]; what is included below 
// is the faster version used in gawk. There is even a faster, duff-device 
// version. The magic constant 65599 was picked out of thin air while experimenting 
// with different constants, and turns out to be a prime. this is one of the 
// algorithms used in berkeley db (see sleepycat) and elsewhere.

unsigned long sdbm(unsigned char *str)
{
	int c;
	unsigned long hash = 0;

	while ((c = *str++))
		hash = c + (hash << 6) + (hash << 16) - hash;

	return hash;
}

// Return the result of the ith hash function. This function uses djb2 and sdbm.
// None of the functions used here is strong for cryptography purposes but they
// are good enough for the purpose of the class.
//
// The approach in this function is based on the paper:
// https://www.eecs.harvard.edu/~michaelm/postscripts/rsa2008.pdf

unsigned long BLOOM_hash(unsigned char *str, unsigned int i) 
{
	return djb2(str) + i * sdbm(str) + i * i;
}



// H synarthsh arxikopoiei ena bloom filter
void BLOOM_init(Bloom* bloom,int size)	
{
	
	// Desmeysh mnhmhs gia to bloom
	bloom->filter=malloc(size);
	
	// Reset mnhmhs gia to bloom
	memset(bloom->filter,0,size);	
	
	// Orismos megethous
	bloom->size=size;
	
}

// Eisagei monada sth thesh position toy bloom filter 
void BLOOM_insert(Bloom* bloom, unsigned long position)
{

	// Evresh byte
	int byte_num=(position/8)%bloom->size;
		
	// Evresh eswterikhs theshs bit
	int inner_bit_num=(position/8)%8;
	
	// Lhpsh antigrafoy toy byte
	unsigned char byte_copy=bloom->filter[byte_num];
	
	// Bohthitikh metablhth
	unsigned char byte_set;
	
	// To bit poy mas endiaferei metaferetai sth deksioterh thesh toy antigrafoy toy byte
	byte_copy=byte_copy>>inner_bit_num;
	
	// Ean einai mhden prepei na ginei set
	if(byte_copy%2==0)
	{
		
		byte_set=1;
		
		byte_set=byte_set<<inner_bit_num;
		
		bloom->filter[byte_num]=(bloom->filter[byte_num]|byte_set);
				
	}
	
	
}

// Epistrefei to bit ths sygkekrimenhs theshs toy bloom filter
int BLOOM_get(Bloom* bloom, unsigned long position)
{
		
	// Evresh byte
	int byte_num=(position/8)%bloom->size;
	
	// Evresh eswterikhs theshs bit
	int inner_bit_num=(position/8)%8;
	
	// Lhpsh antigrafoy toy byte
	unsigned char byte_copy=bloom->filter[byte_num];
	
	// Bohthitikh metablhth
	unsigned char byte_set;
	
	// To bit poy mas endiaferei metaferetai sth deksioterh thesh toy antigrafoy toy byte
	byte_copy=byte_copy>>inner_bit_num;
	
	// Episterfetai h timh toy bit
	return byte_copy%2;		
	
}

// H synarthsh katastrefei ena bloom filter
void BLOOM_destroy(Bloom* bloom)
{
	
	free(bloom->filter);
	
}