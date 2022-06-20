
#include "lcd_chars_courier_test.xpm"
#include <stdio.h>

int main( int argc, char** argv ) {

	char szCharCopy[128];

	int iArraySkip = 3;
	char cOn = '#';
	char cOff = '.';
	
	int iStartShiftX = 0;
	int iStartShiftY = 1;

	int iHorSpacing = 0;
	int iVertSpacing = 3;

	int iCharWidth = 6;
	int iCharHeight = 8;

	int iCharsPerLine = 16;
	int iCharLines = 6;

	int icl,icc;
	int icx0,icy0;
	int icx,icy,ii,inu,ib;
		
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

			printf( "char_%x: .db",icc * iCharsPerLine + icl );

			for( ii = 0; ii < 6 ; ii++ ) {
				inu = 0;
				if( ii > 0 ) printf(",");
				for( ib = 0; ib < 7  ; ib++ ) {
					//printf("%c", szCharCopy[ ii * 8 + ib ] );
					if( szCharCopy[ ii * 8 + ib ] == cOn ) {
						inu |= 1;
					}
					inu = inu << 1;
				}
				//printf( "inu: %X\n", inu );
				printf( " 0x%02x", inu );

			}
			printf ("\n");


		}

	}
}
