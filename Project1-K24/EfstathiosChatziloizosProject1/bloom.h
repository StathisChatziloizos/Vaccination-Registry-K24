#ifndef _BLOOM_
#define _BLOOM_
	
	typedef struct Bloom{

		unsigned char* filter;
		
		int size;
		
	}Bloom;
	

	// H synarthsh arxikopoiei ena bloom filter
	void BLOOM_init(Bloom* bloom,int size);
	
	// Synarthsh katakermatismou tou bloom
	unsigned long BLOOM_hash(unsigned char *str, unsigned int i);
	
	// Eisagei monada sth thesh position toy bloom filter 
	void BLOOM_insert(Bloom* bloom, unsigned long position);
	
	// Epistrefei to bit ths sygkekrimenhs theshs toy bloom filter
	int BLOOM_get(Bloom* bloom, unsigned long position);

	// H synarthsh katastrefei ena bloom filter
	void BLOOM_destroy(Bloom* bloom);

#endif
