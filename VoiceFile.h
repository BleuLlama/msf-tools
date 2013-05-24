/* Sony voice recorder MSF reader
   using reverse-engineered structure

The MIT License (MIT)

Copyright (c) 2013 Scott Lawrence

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include <iostream>


////////////////////////////////////////////////////////////////////////////////
// File header  (0x20 bytes)

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

/* possible structure:
 
	4 bytes	- MSGL	- file signature
	5 bytes	- UNK
	1 byte  - 05     - number of folders?
	... 
	Possible contents:
		- date of last save
		- voice recorder model ID
		- settings for the voice recorder
		- checksum
*/



} sMSF_Header;


////////////////////////////////////////////////////////////////////////////////
// Folders

#define kNumFolders (5)
#define kFolderHeaderSz	( 0x0040 )
#define kFolderItemSz	( 0x0020 )
#define kFolderNItems	( 99 )
#define kFolderPadSz	( 0x0020 )
#define kFolderSize	( kFolderHeaderSz + (kFolderNItems * kFolderItemSz ) + kFolderPadSz )


// folder header (1 per folder, 0x20 bytes)

typedef struct sFldHdr {
	unsigned char buf[ kFolderHeaderSz ];

	/* contents are:
		46 44 00 16 NN 00..
		F  D  .  .  (L) ...
		where (L) is the folder name 'A'..'F'
	*/
} sFldHdr;


// folder item (99 per folder, 0x20 bytes each)

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

////////////////////////////////////////////////////////////////////////////////


class VoiceFile {

private:
	bool valid;		// is the loaded thing valid?
	unsigned char * buffer;	// buffer of the file (it's only 16k)
	std::string vrDir;	// directory of the voice recorder
	std::string vrVoiceDir;	// directory of the VOICE folder
	std::string vrMSFFile;	// path to the full MSF file

public:
	// create (including parse), destroy
	VoiceFile( std::string _path );
	~VoiceFile( void );


public:
	// did we load a valid path? (accessor)
	bool Valid( void ) { return valid; }

	// configure all of the path stuff, based on the voice recorder path
	void ConfigurePath( std::string _path );

public:
	// dump the entire contents to stdout
	int Dump( void );


private:
	// internal accessors to positions in the buffer
	long GetOffsetToFolder( int folderIndex );
	long GetOffsetToFile( int fileIndex );

public:
	// is the folder idx passed in valid?
	bool ValidFolder( int folderIndex );

	// get the number of folders
	int GetFolderCount( void );

public:
	// these return empty string on fail (index out of range, etc)

	// get the name of the specified folder
	std::string GetFolderName( int folderIndex );

	// get the path to the specified folder
	std::string GetPathForFolder( int folderIndex );


	// get the number of items in the folder
	int GetFileCountForFolder( int folderIndex );

	// are the passed in indexes valid?
	bool ValidFile( int fileIndex, int FolderIndex );

	// get the filename (8.3) for the specified indexes
	std::string GetFileInFolder( int fileIndex, int folderIndex );

	// get the printable version of the date for the specified indexes
	std::string PrintableDateForFileInFolder( int fileIndex, int folderindex );
	//
	// get the filename-sortable version of the date for the specified indexes
	std::string SortableDateForFileInFolder( int fileIndex, int folderindex );

};


