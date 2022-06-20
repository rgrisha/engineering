
void shift_bit_array_left( char* lcba, int ibalen, int inum ) {
	int i,j;
	int im,imc;
	if( ibalen <= 0 ) return;
	for( i = 0 ; i < inum; i++ ) {
		im = 0;
		imc = 0;
		for( j = 0 ; j < ibalen; j++ ) {
			//printf("before shift lcba: %x im %x: %x\n",lcba[j],im);
			im = lcba[j] & 0x80;
			lcba[j] = lcba[j] << 1;
			if( imc > 0 ) lcba[j] |= 0x01;
			//printf("lcba after shift %d, byte %d: %x and prev im result: %x\n",i,j,lcba[j],imc);
			imc = im;
		}
	}
}

void shift_bit_array_right( char* lcba, int ibalen, int inum ) {
	int i,j;
	int im,imc;
	if( ibalen <= 0 ) return;
	for( i = 0 ; i < inum; i++ ) {
		im = 0;
		imc = 0;
		for( j = ibalen-1 ; j >= 0; j-- ) {
			//printf("before shift lcba: %x im %x: imc: %x\n",lcba[j],im,imc);
			im = lcba[j] & 0x01;
			lcba[j] = lcba[j] >> 1;
			lcba[j] &= 0x7F;
			//printf("just here after shift: %x\n", lcba[j] & 0xFF );
			if( imc > 0 ) lcba[j] |= 0x80;
			//printf("lcba after shift %d, byte %d: %x and prev im result: %x\n",i,j,lcba[j],imc);
			imc = im;
		}
	}
}
