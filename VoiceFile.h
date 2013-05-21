/* Sony voice recorder MSF reader
   using reverse-engineered structure

   Scott Lawrence 2013
*/
#include <iostream>

class VoiceFile {


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

#define kNumFolders (5)
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


private:
	bool valid;
	std::string vrDir;
	std::string vrVoiceDir;
	std::string vrMSFFile;

public:
	VoiceFile( std::string _path );
	~VoiceFile( void );

	bool Valid( void ) { return valid; }
public:
	void ConfigurePath( std::string _path );

	int Scan( void );


public:
	// these return empty string on fail (index out of range, etc)
	std::string GetFolderName( int folderIndex );
	std::string GetFileInFolder( int fileIndex, int folderIndex );

};
