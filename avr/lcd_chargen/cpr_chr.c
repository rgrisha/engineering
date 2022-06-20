
#include "lcd_chars_courier_test.xpm"
#include <stdio.h>
#include "shift_chr.h"

#define iCharWidth 5
#define iCharHeight 9
// Limit2Stat denotes how much biggest matches between characters we will seek.
// match array is sorted desc and only first <Limit2Stat> items are printed
#define Limit2Stat 4

int idebug = 0;
int ia_frqs[1<<(iCharWidth+1)][2];
int ilasta = 0;

int ia_frqs_2c[ 1<<(iCharWidth+1)][1<<(iCharWidth+1)][1];

int ifrq_prev;

char szbinstr[16];
void conv_bin_str(int ichar ) {
	int i = 0;
	int imsk = 0x80;
	memset( szbinstr, 0 , sizeof( szbinstr ) );
	for( imsk = 0x80; imsk; imsk = imsk >> 1 ) {
		if( ichar & imsk ) {
			szbinstr[i] = '%';
		} else {
			szbinstr[i] = '.';
		}
		i++;
	}
}

char sz_char_compressed[iCharHeight];
void put_bits_into_array( char* lpa, int ilpalen, int ichar, int ibitcnt ) {
	//int imsk = ( 1 << ibitcnt ) - 1;
	lpa[ilpalen-1] = ichar;
	shift_bit_array_right( lpa, ilpalen, ibitcnt );
	//int imasked = imsk & ichar;
	//lpa[iCharHeight-1] = lpa[iCharHeight-1] | (char) imasked;

}

int compress_base[1<<(iCharWidth)+1];

int do_compress( int icharline ) {
	// method of compression of char line:
	// 00 - repeat
	// 01 - base
	// 1XX - 0-3 shift from the base
	//
	int imethod;
	int i,j;
	int ibitlen = 0;
	conv_bin_str( icharline );
	do {
		if( ifrq_prev < 0 ) { // first char line
			ibitlen = iCharWidth;	
			put_bits_into_array( sz_char_compressed, sizeof(sz_char_compressed),
					icharline, iCharWidth );
			if( idebug) printf( "Method: FI offset 0 %s", szbinstr );
		} else { // if not first, then compress
			if( icharline == ifrq_prev ) {
				ibitlen = 2;
				put_bits_into_array( sz_char_compressed, sizeof(sz_char_compressed),
						0 , 2 );
				if( idebug )printf( "Method: 00 offset 0 %s", szbinstr );
				break;
			}
			// check if there is a base in the base array
			for( i = 0; i < Limit2Stat; i++ ) {
				j = ifrq_prev * Limit2Stat + i;
				if( compress_base[j] == icharline ) {
					break;
				}
			}
			if( i < Limit2Stat ) { // means found
				ibitlen = 3;
				put_bits_into_array( sz_char_compressed, sizeof(sz_char_compressed),
						1 , 1 );
				put_bits_into_array( sz_char_compressed, sizeof(sz_char_compressed),
						2 , i );
				if (idebug) printf( "Method: 1X offset %d %s",i, szbinstr );
				break;
			}
			// if we are unable to compress, we are here . Then we output new base
				ibitlen = 2 + iCharWidth;
				put_bits_into_array( sz_char_compressed, sizeof(sz_char_compressed),
						1 , 2 );
				put_bits_into_array( sz_char_compressed, sizeof(sz_char_compressed),
						icharline , iCharWidth );
			if( idebug ) printf( "Method: 01 offset 0 %s", szbinstr );
			
		}
	} while( 0 ); // one time loop to be able use of break;
	ifrq_prev = icharline;
	if( idebug ) printf( " ccode size: %d\n", ibitlen );
	return ibitlen;
}

int frq_analyse( int ichar ) {
	int i;

	if( ifrq_prev >= 0 ) {
		ia_frqs_2c[ifrq_prev][ichar][0]++;
	}

	ifrq_prev = ichar;

	for( i = 0 ; i < ilasta; i++) {
		if( ia_frqs[i][0] == ichar ) {
			ia_frqs[i][1]++;
			return ia_frqs[i][1];
		}
	}
	// if here, means not found
	ia_frqs[i][0] = ichar;
	ia_frqs[i][1] = 0;
	ilasta++;

	return 1;
}

//also inits compress_base array
void print_analysis( ) {
	int i,j;
	for( i = 0; i < ilasta; i++ ) {
		printf("char %x frq %d\n",
			ia_frqs[i][0], ia_frqs[i][1] );

	}

	// we will implement sorting here as we need first 4 combinations
	// for compress
	int imaxf,z,ie,ifound;
	for( i = 0; i < (1<<iCharWidth)+1; i++ ) {

		z = 1;
		imaxf = 0x0FFFFFFF;
		while ( 1 ) { // we need only first 4 most frequent combinations

			//calculate next smaller element

			ifound = 0;	
			ie = 0; // ia_frqs_2c[i][0][0];

			for( j = 0; j < (1<<iCharWidth)+1; j++ ) {
				if( i==j ) continue; // we have special compression for same el.
				//printf("search for next smaller el: i=%d,i=%d,el=%d,ie=%d,maxf=%d\n",
				//		i,j,ia_frqs_2c[i][j][0],ie,imaxf );
				if( (ia_frqs_2c[i][j][0] < imaxf) && (ia_frqs_2c[i][j][0] > ie) ) {
					ifound = 1;
					ie = ia_frqs_2c[i][j][0];
				}
			}
			imaxf = ie;
			//printf("found: %d\n", ifound );
			if( !ifound ) break;


			for( j = 0; j < (1<<iCharWidth)+1; j++ ) {
				if( ia_frqs_2c[i][j][0] == ie ) {
					//if( i == j ) continue;
					printf( "frq 2 analysis\t c1\t%d\tc2\t%d\tfrq\t%d\n",
							i,j, ia_frqs_2c[i][j][0] );
					compress_base[ i * Limit2Stat + z ] = j;
					z++;
					if( z > Limit2Stat ) break; // here we are limiting for 3 first elems
				}
			}
			if( z > Limit2Stat ) break;
		}
	}
}

int main( int argc, char** argv ) {

	char szCharCopy[128];

	int iArraySkip = 3;
	char cOn = '#';
	char cOff = '.';
	
	int iStartShiftX = 1;
	int iStartShiftY = 2;

	int iHorSpacing = 1;
	int iVertSpacing = 2;

	//int iCharWidth = 5;
	//int iCharHeight = 9;

	int iCharsPerLine = 16;
	int iCharLines = 6;

	int icl,icc,irm;
	int icx0,icy0;
	int icx,icy,ii,inu,ib,i;

	if( argc > 1 && !strcmp( argv[1], "-debug" ) ) {
		idebug = 1;
	}

	memset( ia_frqs_2c, 0, sizeof( ia_frqs_2c ) );
	memset( compress_base, 0, sizeof( compress_base ) );

	for( irm = 0; irm < 2; irm ++ ) {
		
			for( icc = 0; icc < iCharLines; icc++ ) {
				icy0 = icc * ( iCharHeight + iVertSpacing );
				icy0 = icy0 + iArraySkip + iStartShiftY ;

				//printf("got dummy%d %s\n",icy0,dummy[icy0] );

				for( icl = 0; icl < iCharsPerLine; icl++ ) {
					icx0 = icl * ( iCharWidth + iHorSpacing ) + iStartShiftX;

					memset( szCharCopy, '\0', sizeof( szCharCopy ) ); 
					ii = 0;

					for( icy = icy0; icy < icy0 + iCharHeight ; icy++ ) {

						//printf("got dummy%d %s\n",icy,dummy[icy] );

						for( icx = icx0; icx < icx0 + iCharWidth; icx++ ) {
							szCharCopy[ ii++ ] = dummy[icy][icx];	
						}
						//printf( "---- %s\n", szCharCopy );

					}

					//printf( "---- %s\n", szCharCopy );

					int iis = 0;
					int ibc = 0;
					/*
					printf( "char_%x: .db",icc * iCharsPerLine + icl );
					for( i = 0; i < sizeof( szCharCopy ) ; i++ ) {
						if( szCharCopy[i] == cOn ) {
							ibc |= 1 << ( 8 - i + iis - 1);
						}
						if( i % 8 == 7 ) {
							iis = i + 1;
							printf( " 0x%02x", ibc );
							ibc = 0;	
							if( i > iCharWidth * iCharHeight ) break;
							printf(" ,");
						}
						
					}
					printf("\n");
					*/


					
					ifrq_prev = -1;
					int isum_bit_len = 0;
					memset(sz_char_compressed, 0, sizeof( sz_char_compressed ) );
					for( ii = 0 ; ii < iCharWidth * iCharHeight; ii++ ) {
						if( ii % iCharWidth == iCharWidth - 1 ) {
							ibc = 0;
							for( i=iis;i<=ii;i++) {
								if( szCharCopy[i] == cOn ) {
									//printf("(%d)",iCharWidth-i+iis-1);
									ibc |= 1 << (iCharWidth - i + iis - 1);
								}
								//printf("%c",szCharCopy[i]);
							}
							iis = ii+1;
							if( irm == 0 ) { // first run - calc base for compress
								printf(" %2x ",ibc);
								printf("frq: %d\n",frq_analyse( ibc ));
							} else if ( irm == 1 ) {
								isum_bit_len += do_compress( ibc );
							}
						}
					}
					if( irm == 1 ) {
						if( idebug ) printf("Summary bit length: %d\n", isum_bit_len );
						ibc = isum_bit_len % 8;
						if( ibc == 0 ) {
							ibc = isum_bit_len / 8;
						} else {
							ibc = ( isum_bit_len / 8 ) + 1;
						}
						printf( "char_%x: .db",icc * iCharsPerLine + icl );
						for( ii = 0; ii < ibc; ii++ ) {
							conv_bin_str( sz_char_compressed[ii] );
							printf( " %s ", szbinstr);
							//printf( " 0x%02x", sz_char_compressed[ii] & 0xFF );
						}
						printf("\n");
					}



				}

			}
			if( irm == 0 ) {
				print_analysis();
			}


			// now we will go and compress
			

		} // for irm
}
