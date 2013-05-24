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
/*
#include <iostream>
#include <string>

#include <stdlib.h>
#include <string.h>
*/
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h> /* directory stuff */

#include "DiskUtils.h"


////////////////////////////////////////////////////////////////////////////////

DiskUtils::DiskUtils( void )
{
	std::cerr << "YOU SHOULD NEVER SEE THIS." << std::endl;
}

DiskUtils::~DiskUtils( void )
{
}

////////////////////////////////////////////////////////////////////////////////

void DiskUtils::MakeDir( std::string _path )
{
	// if it's already there, we're done
	if( DiskUtils::IsValidDir( _path )) return;

	// okay. try to create it
#if defined (_WIN32) || defined( __MINGW32__ )
	_mkdir( _path.c_str() );
#else
	mkdir( _path.c_str(), 0755 );
#endif

	
}

bool DiskUtils::IsValidDir( std::string _path )
{
	// stat the path to get ISDIR
	struct stat sb;
	if (stat(_path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
	{
		return true;
	}
	return false;
}


long DiskUtils::FileSize( std::string _path )
{
	// stat the path to get a file size.
	struct stat sb;
	if (stat(_path.c_str(), &sb) == 0 )
	{
		return (long) sb.st_size;
	}
	return 0;
}


bool DiskUtils::IsSimilarFile( std::string _pathA, std::string _pathB )
{
	// just in case we're stupid, make sure the paths aren't
	// exact, if so, short circuit out quick.
	// (quick check for alike files)
	if( _pathA.compare( _pathB ) == 0 ) return true;

	// 1. check filesizes. (quick check for non-differing files)
	long sizeA = DiskUtils::FileSize( _pathA );
	long sizeB = DiskUtils::FileSize( _pathB );

	// file sizes are different.  just bail out.
	if( sizeA != sizeB ) return false;	// both are different-sized

	if( sizeA == 0 || sizeB == 0 ) return false;	// one or the other couldn't be opened.

	// 2. check a few bytes to see if they're the same
	FILE * fpA = fopen( _pathA.c_str(), "rb" );
	FILE * fpB = fopen( _pathB.c_str(), "rb" );

	int differences = 0;

	if( fpA && fpB ) {
		// check at 0x0000 and again at 0x800 for different bytes
		for( int offs = 0 ; offs < 0x900 ; offs += 0x800 )
		{
			// go to 0x0000 or 0x800
			fseek( fpA, offs, SEEK_SET );
			fseek( fpB, offs, SEEK_SET );

			for( int addr=0 ; addr < 0x100  ; addr++ ) 
			{
				int dA = fgetc( fpA );
				int dB = fgetc( fpB );

				if( dA != dB ) differences++;
			}
		}
	}

	if( fpA ) fclose( fpA );
	if( fpB ) fclose( fpB );

	return (differences == 0) ? true : false;
}



// LFNFrom83
// 	determine the long filename associated with the 8.3 pased in
// 	the problem is that the lfn through a regular algo could map to multiple names
// 	since we won't have a lot of files, let's just dumb-match it
//
//	so first, we check filesizes
//	for alike-sizes, we check a few bytes at 0x800 bytes in, where files tend to differ.
std::string DiskUtils::LFNFrom83( std::string _path, std::string eightthree )
{
	int foundcount = 0;
	std::string foundname( "" );


	// expand out the full path
	std::string e3_fullpath( _path );
	e3_fullpath.append( eightthree );

	// okay. let's now search the directory for the LFN with the same size.
	DIR * theDir = opendir( _path.c_str() );
	struct dirent * entry;
	if( theDir ) {

		// check each entry
		while(( entry = readdir( theDir )) != NULL ) {

			// build a full path to the file
			std::string lfn_fullpath( _path );
			lfn_fullpath.append( entry->d_name );

			// are they similar?
			bool isSimilar = DiskUtils::IsSimilarFile( e3_fullpath, lfn_fullpath );

			// WE GOT ONE!!!
			if( isSimilar ) {
				foundcount++;
				foundname.assign( entry->d_name );
			}
		}


		closedir( theDir );
	}

	// okay. there was a match. return it.
	if( foundcount >= 1 ) {
		return foundname;
	}

	return _path + eightthree;
}


#define kCopyFileBufSize	((4096) * 64 )
bool DiskUtils::CopyFile( std::string fromPath, std::string toPath, bool skipIfExists )
{
	char buf[ kCopyFileBufSize ];

	// if the file is already there, and it's okay to skip, we're done
	if( skipIfExists ) {
		if( DiskUtils::IsSimilarFile( fromPath, toPath )) return true;
	}
	
	// copy the file from fromPath to toPath. ;)

	// open the files
	FILE * inf = fopen( fromPath.c_str(), "rb" );
	if( !inf ) {
		std::cerr << "Couldn't open input file " << fromPath << std::endl;
		return true;
	}

	FILE * outf = fopen( toPath.c_str(), "wb" );
	if( !outf ) {
		std::cerr << "Couldn't open output file " << toPath << std::endl;
		fclose( inf );
		return true;
	}


	// copy it over one buffer at a time
	size_t n = 0;
	do {
		n = fread( buf, 1, kCopyFileBufSize, inf );
		fwrite( buf, n, 1, outf );
	} while( n!= 0 );

	// close the files
	fclose( inf );
	fclose( outf );

	return false;
}
