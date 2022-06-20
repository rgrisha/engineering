
#include <stdio.h>
#include "shift_chr.h"


int main( int argc, char** argv ) {

	if( argc < 2 ) {
		printf("usage: %s <cnt to shift>\n",argv[0]);
		return 0;
	}

	typedef struct {
		char char1;
		char char2;
	} char2;


	char szA[3] = {0,0};


	typedef union {
		char2 char_pair;
		unsigned short intv;
	} uci;

	uci uci_test;
	uci_test.intv = 0xc000;

	shift_bit_array_right( (char*)&uci_test, sizeof( uci_test ), atoi( argv[1] ));

	printf( "shifted bit array: %x\n", uci_test.intv );
	
}
