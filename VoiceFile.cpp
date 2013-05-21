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
	, vrDir( "." )
	, vrVoiceDir( "." )
	, vrMSFFile( "x" )
{
	if( DiskUtils::IsValidDir( _path )) {
		valid = true;
		this->ConfigurePath( _path );
	}
}

VoiceFile::~VoiceFile( void )
{
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
	FILE * fp = NULL;
	size_t bufsize = 0;
	size_t readbytes = 0;
	unsigned char *buf;
	int i;
	int fld;

	/* attempt to open the file */
	fp = fopen( this->vrMSFFile.c_str(), "r" );

	if( !fp ) {
		std::cerr << "Unable to open file: " << vrMSFFile << std::endl;
		return -1;
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

	buf = (unsigned char *) malloc( sizeof( unsigned char ) * bufsize );

	readbytes = fread( buf, sizeof( char ), bufsize, fp );
	fclose( fp );

	if( readbytes == 0 ) {
		return -3;
	}

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
		int folderNameOffset = 0;
		char * folderName;

		/* compute the start offset */
		folderOffset = kFileHeaderSz + (fld * kFolderSize );

		folderNameOffset = folderOffset + 4;
		folderName = (char * ) (buf + folderNameOffset);

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


std::string VoiceFile::GetFileInFolder( int fileIndex, int folderIndex )
{
	return "FileX";
}
