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
#include <iomanip> /* for setw */
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h> /* directory stuff */

#include "VoiceFile.h"
#include "DiskUtils.h"


////////////////////////////////////////////////////////////////////////////////

VoiceFile::VoiceFile( std::string _path )
	: valid( false )
	, buffer( NULL )
	, vrDir( "." )
	, vrVoiceDir( "." )
	, vrMSFFile( "x" )
{
	if( DiskUtils::IsValidDir( _path )) {
		this->ConfigurePath( _path );
	} else {
		return;
	}


	/* load in the buffer */
	FILE * fp = NULL;

	size_t bufsize = 0;
	size_t readbytes = 0;

	/* attempt to open the file */
	fp = fopen( this->vrMSFFile.c_str(), "r" );

	if( !fp ) {
		std::cerr << "Unable to open file: " << vrMSFFile << std::endl;
		return;
	}

	/* read it in to a buffer */
	fseek( fp, 0, SEEK_END );
	bufsize = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	//printf( "File is %ld bytes\n", bufsize );
	if( bufsize != 16384 ) {
		std::cerr << "File is the wrong size." << std::endl;
		std::cerr << "Bad things might happen" << std::endl;
		std::cerr << std::endl;
	}

	this->buffer = (unsigned char *) malloc( sizeof( unsigned char ) * bufsize );

	if( !this->buffer ) {
		std::cerr << "Mem Alloc error!" << std::endl;
		fclose( fp );
		return;
	}


	readbytes = fread( this->buffer, sizeof( char ), bufsize, fp );
	fclose( fp );

	if( readbytes == 0 ) {
		std::cerr << "Couldn't read!" << std::endl;
		return;
	}


	/* 1. validate its' the right file */
	if(   this->buffer[0] != 'M'
	   || this->buffer[1] != 'S'
	   || this->buffer[2] != 'G'
	   || this->buffer[3] != 'L' ) {
		std::cerr << "This is not a Message List file!" << std::endl;
		return;
	}


	valid = true;
	return;
}

VoiceFile::~VoiceFile( void )
{
	if( buffer ) {
		free( buffer );
		buffer = NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////

void VoiceFile::ConfigurePath( std::string _path )
{
	this->vrDir.assign( _path );
	this->vrVoiceDir.assign( _path );
	vrVoiceDir += "/VOICE/";

	vrMSFFile.assign( vrVoiceDir );
	vrMSFFile += "MSGLISTS.MSF";

}


int VoiceFile::Dump( void )
{
	if( !this->valid ) {
		std::cerr << "Invalid file!" << std::endl;
		return -10;
	}

	/* meta dump! */

	std::cout << "Folders:" << std::endl;
	std::cout << "   Voice Recorder at " << vrDir << std::endl;
	std::cout << "        Voice Dir is " << vrVoiceDir << std::endl;
	std::cout << "         MSF File is " << vrMSFFile << std::endl;
	std::cout << std::endl;

	/* folder dumps! */

	for( int f=0 ; f < this->GetFolderCount() ; f++ )
	{
		std::cout << "Folder " << f << " is named \"" << this->GetFolderName( f ) << "\"" << std::endl;
		std::cout << "   path: " << this->GetPathForFolder( f ) << std::endl;
		std::cout << "  count: " << this->GetFileCountForFolder( f ) << std::endl;

		for( int i=0 ; i < this->GetFileCountForFolder( f ) ; i++ )
		{
			std::cout << "     " << std::right << std::setw( 2 ) << i << ":" << std::endl;

			std::string indent( "       " );

			std::cout << indent << "  8.3: " << this->GetFileInFolder( i, f ) << std::endl;
			std::cout << indent << " date: " << this->PrintableDateForFileInFolder( i, f ) << std::endl;

			std::string lfn = DiskUtils::LFNFrom83( this->GetPathForFolder( f ), this->GetFileInFolder( i, f) );
			std::cout << indent << "  lfn: " << lfn << std::endl;
			std::cout << indent << "  sfn: " << this->SortableDateForFileInFolder( i, f ) << "__"
							 << this->GetFileInFolder( i, f ) << std::endl;

		}


		std::cout << std::endl;

	}

	return 0;
}


////////////////////////////////////////////////////////////////////////////////

bool VoiceFile::ValidFolder( int folderIndex )
{
	if( !this->valid || folderIndex < 0 || folderIndex >= kNumFolders ) return false;
	return true;
}

long VoiceFile::GetOffsetToFolder( int folderIndex )
{
	if( !this->ValidFolder( folderIndex ) ) return 0;

	return ( kFileHeaderSz + ( folderIndex * kFolderSize ));
}

long VoiceFile::GetOffsetToFile( int fileIndex )
{
	if( !this->valid || fileIndex < 0 || fileIndex >= kFolderNItems  ) return 0;

	return kFolderHeaderSz + (fileIndex * kFolderItemSz);
}



////////////////////////////////////////////////////////////////////////////////

int VoiceFile::GetFolderCount( void )
{
	return kNumFolders;
}


int VoiceFile::GetFileCountForFolder( int folderIndex )
{
	if( !this->ValidFolder( folderIndex )) return 0;

	long offs = this->GetOffsetToFolder( folderIndex );

	int count = 0;

	offs += kFolderHeaderSz;

	// check for items until we find a null item
	for( count = 0 ; (count < kFolderNItems) && (this->buffer[offs] != '\0') ; count++, offs+= kFolderItemSz);

	return count;
}



std::string VoiceFile::GetFolderName( int folderIndex )
{
	if( !this->ValidFolder( folderIndex )) return "";

	long folderOffset = this->GetOffsetToFolder( folderIndex );
	
	char * folderName = (char * ) (this->buffer + folderOffset  + 4);

	return folderName;
}

std::string VoiceFile::GetPathForFolder( int folderIndex )
{
	if( !this->ValidFolder( folderIndex )) return "";

	std::string folderPath( this->vrVoiceDir );
	folderPath.append( this->GetFolderName( folderIndex ));
	folderPath.append( "/" );

	return folderPath;
}



bool VoiceFile::ValidFile( int fileIndex, int folderIndex )
{
	if( !this->ValidFolder( folderIndex ) ) return false;
	if( fileIndex < 0 ) return false;
	if( fileIndex >= this->GetFileCountForFolder( folderIndex )) return false;
	return true;
}


std::string VoiceFile::GetFileInFolder( int fileIndex, int folderIndex )
{
	if( !this->valid || !this->ValidFile( fileIndex, folderIndex )) return "";

	// get the offset to the item
	long offs = this->GetOffsetToFolder( folderIndex );
	offs += this->GetOffsetToFile( fileIndex );
	
	sFldItm *fi = (sFldItm *)&this->buffer[ offs ];
	char fn[9];


	for( int i=0 ; i<8 ; i++ ) {
		fn[i] = fi->file[i];
	}
	fn[8] = '\0';

	std::string filename( fn );
	filename.append( ".MP3" );

	return filename;
}


std::string VoiceFile::PrintableDateForFileInFolder( int fileIndex, int folderIndex )
{
	if( !this->valid || !this->ValidFile( fileIndex, folderIndex )) return "";

	long offs = this->GetOffsetToFolder( folderIndex );
	offs += this->GetOffsetToFile( fileIndex );

	sFldItm *fi = (sFldItm *)&this->buffer[ offs ];

	char b[128];

	snprintf( b, 127, "%d-%02d-%02d  %02d:%02d:%02d",
		fi->Year + 1980, fi->Month, fi->Day,
		fi->Hour, fi->Minute, fi->Second );

	return b;
}

std::string VoiceFile::SortableDateForFileInFolder( int fileIndex, int folderIndex )
{
	if( !this->valid || !this->ValidFile( fileIndex, folderIndex )) return "";

	long offs = this->GetOffsetToFolder( folderIndex );
	offs += this->GetOffsetToFile( fileIndex );

	sFldItm *fi = (sFldItm *)&this->buffer[ offs ];

	char b[128];

	snprintf( b, 127, "%d_%02d_%02d_%02d%02d%02d",
		fi->Year + 1980, fi->Month, fi->Day,
		fi->Hour, fi->Minute, fi->Second );

	return b;
}
