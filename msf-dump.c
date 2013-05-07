/* Sony voice recorder MSF reader
   using reverse-engineered structure

   Scott Lawrence 2013
*/

#include <stdio.h>
#include <stdlib.h>

#define kMsgFilename "MSGLISTS.MSF"

#define kFileHeaderSz (0x0020)

typedef struct sMSF_Header {
	unsigned char buf[kFileHeaderSz];  /* we'll use a char buf for now */
	/*
		st	contents
		00	MSGL - 4d, 53, 47, 4c

00000000  4d 53 47 4c 04 00 00 26  07 dd 05 06 10 05 33 01  |MSGL...&......3.|
00000010  00 00 00 03 00 40 00 20  00 20 03 00 1f 00 00 00  |.....@. . ......|
---
same file, resaved:
00000000  4d 53 47 4c 04 00 00 26  07 dd 05 06 10 08 0e 01  |MSGL...&........|
00000010  00 00 00 03 00 40 00 20  00 20 03 00 21 00 00 00  |.....@. . ..!...|
					       ^^
	*/
} sMSF_Header;

#define kNumFolders (6)
#define kFolderHeaderSz	( 0x0040 )
#define kFolderItemSz	( 0x0020 )
#define kFolderNItems	( 99 )
#define kFolderPadSz	( 0x0020 )
#define kFolderSize	( kFolderHeaderSz + (kFolderNItems * kFolderItemSz ) + kFolderPadSz )

typedef struct sFldHdr {
	unsigned char buf[ kFolderHeaderSz ];

	/* contents are:
		46 44 00 16 NN 00..
		F  D  .  .  (L) ...
		where (L) is the folder name 'A'..'F'
	*/
} sFldHdr;

typedef struct sFldItm {
	unsigned char Pad4D;	/* 4d, 00 for unused */
	unsigned char Pad0A;	/* 0x00 */
	unsigned char src;	/* 0x6b = voice recorder, 0x00 = external */
	unsigned char PadF[6];	/* 0xffs */
	unsigned char Pad2;	/* 0x02 */
	unsigned char Pad0[6];	/* 0x00s */
	char file[8];		/* [8].3 filename */
	unsigned char Year;	/* years since 1980, 1980 = 0 */
	unsigned char Month;	/* month of year, Jan = 1 */
	unsigned char Day;	/* day of month */
	unsigned char Hour;	/* 24 hour time hour */
	unsigned char Minute;	/* minutes */
	unsigned char Second;	/* seconds */
	unsigned char Pad00[2];	/* 0x00s */
} sFldItm;




int Scan( char * fn )
{
	FILE * fp = NULL;
	size_t bufsize = 0;
	size_t readbytes = 0;
	unsigned char *buf;
	int i;
	int fld;

	/* attempt to open the file */
	fp = fopen( fn, "r" );

	if( !fp ) {
		fprintf( stderr, "Unable to open file: %s", fn );
		return -1;
	}

	/* read it in to a buffer */
	fseek( fp, 0, SEEK_END );
	bufsize = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	printf( "File is %ld bytes\n", bufsize );

	buf = (unsigned char *) malloc( sizeof( unsigned char ) * bufsize );

	readbytes = fread( buf, sizeof( char ), bufsize, fp );
	fclose( fp );

/*
	for( i=0 ; i < 0x20 ; i++ ) {
		printf( "%04x %02x\n", i, buf[i] );
	}
*/

	/* 1. validate its' the right file */
	if(   buf[0] != 'M'
	   || buf[1] != 'S'
	   || buf[2] != 'G'
	   || buf[3] != 'L' ) {
	   	fprintf( stderr, "This is not a Message List file!\n" );
		return -2;
	}

	/* 2. folder dumps! */
	for( fld = 0; fld < kNumFolders ; fld++ )
	{
		int fitem;
		int folderOffset = 0;

		/* compute the start offset */
		folderOffset = kFileHeaderSz + 
			(fld * (kFolderHeaderSz 
			 	+ (kFolderItemSz * kFolderNItems )
			        + kFolderPadSz ));

		printf( "Folder %c:  (0x%04x)\n", 'A' + fld, folderOffset );

		for( fitem = 0 ; fitem < 100 ; fitem++ )
		{
			int itemIdx = folderOffset + kFolderHeaderSz
					+ (fitem * kFolderItemSz );
			if( buf[ itemIdx] != 0 ) {
				char fn[9];
				sFldItm *fi = (sFldItm *)&buf[ itemIdx ];

				/* there's an item here! */

				for( i=0 ; i<8 ; i++ ) {
					fn[i] = fi->file[i];
				}
				fn[8] = '\0';

				printf( "    %3d: %s.MP3  ", fitem, fn );
				printf( "%d-%02d-%02d  %02d:%02d:%02d\n",
					fi->Year + 1980, fi->Month, fi->Day,
					fi->Hour, fi->Minute, fi->Second );
			}
		}
	}

	return 0;
}


int main( int argc, char ** argv )
{
	char * pth;
	if( argc != 2 ) {
		printf( "Usage:  %s <path to folder containing .MSF file>\n", argv[0] );
		printf( "  eg:  %s /Volumes/IC_RECORDER/VOICE\n", argv[0] );
		printf( "  eg:  %s V:\\VOICE\n", argv[0] );
		return -1;
	}

	pth = (char *) malloc( sizeof( char ) * (strlen( argv[1] ) 
					      +  strlen( kMsgFilename )
					      +  2 ));
	strcpy( pth, argv[1] );
	if( pth[ strlen( pth )-1] != '/' ) {
		strcat( pth, "/" );
	}
	strcat( pth, kMsgFilename );

	printf( "Using %s\n", pth );

	return Scan( pth );
}
