/* Sony voice recorder MSF reader
   using reverse-engineered structure

   Scott Lawrence 2013
*/
#include <iostream>
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

	std::cout << "Voice Recorder at " << vrDir << std::endl;
	std::cout << "     Voice Dir is " << vrVoiceDir << std::endl;
	std::cout << "      MSF File is " << vrMSFFile << std::endl;
}


int VoiceFile::Scan( void )
{
	int i;
	int fld;

	if( !this->valid ) {
		std::cerr << "Invalid file!" << std::endl;
		return -10;
	}

	/* working on moving this into the below functions. */

	/* 2. folder dumps! */
	for( fld = 0; fld < kNumFolders ; fld++ )
	{
		int fitem;
		int folderOffset = 0;
		int folderNameOffset = 0;
		char * folderName;

		/* compute the start offset */
		folderOffset = kFileHeaderSz + (fld * kFolderSize );

		folderNameOffset = folderOffset + 4;
		folderName = (char * ) (this->buffer + folderNameOffset);

		std::string folderPath( this->vrVoiceDir );
		folderPath.append( folderName );
		folderPath.append( "/" );

		printf( "\nFolder %s: 0x%04x %s\n",
				folderName,
				folderOffset, folderPath.c_str() );

		for( fitem = 0 ; fitem < 100 ; fitem++ )
		{
			int itemIdx = folderOffset + kFolderHeaderSz
					+ (fitem * kFolderItemSz );
			if( this->buffer[ itemIdx] != 0 ) {
				char fn[9];
				sFldItm *fi = (sFldItm *)&this->buffer[ itemIdx ];

				/* there's an item here! */

				for( i=0 ; i<8 ; i++ ) {
					fn[i] = fi->file[i];
				}
				fn[8] = '\0';

				printf( "    %3d: %s.MP3  ", fitem, fn );
				printf( "%d-%02d-%02d  %02d:%02d:%02d\n",
					fi->Year + 1980, fi->Month, fi->Day,
					fi->Hour, fi->Minute, fi->Second );

				std::string eightthree( fn );
				eightthree.append( ".MP3" );
				std::string lfn = DiskUtils::LFNFrom83( folderPath, eightthree );
				printf( "       : %s\n", lfn.c_str() );
			}
		}
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////


std::string VoiceFile::GetFolderName( int folderIndex )
{
	return "FolderX";
}

std::string VoiceFile::GetPathForFolder( int folderIndex )
{
	return "FOO/";
}


std::string VoiceFile::GetFileInFolder( int fileIndex, int folderIndex )
{
	return "FileX";
}

std::string VoiceFile::PrintableDateForFileInFolder( int fileIndex, int folderindex )
{
	return "date/time";
}
