/* Sony voice recorder MSF reader
   using reverse-engineered structure

   Scott Lawrence 2013
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

bool DiskUtils::IsValidDir( std::string _path )
{
	struct stat sb;
	if (stat(_path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
	{
		return true;
	}
	return false;

}



long DiskUtils::FileSize( std::string _path )
{
	struct stat sb;
	if (stat(_path.c_str(), &sb) == 0 )
	{
		return (long) sb.st_size;
	}
	return 0;
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
	std::string e3_fullpath( _path );
	e3_fullpath.append( eightthree );
	long e3_size = DiskUtils::FileSize( e3_fullpath );

	// okay. let's now search the directory for the LFN with the same size.

	DIR * d = opendir( _path.c_str() );
	struct dirent * entry;
	int foundcount = 0;
	std::string foundname( "" );
	if( d ) {

		while(( entry = readdir( d )) != NULL ) {
			std::string lfn_fullpath( _path );
			lfn_fullpath.append( entry->d_name );
			long lfn_size = DiskUtils::FileSize( lfn_fullpath );

			if( lfn_size == e3_size ) {
				// okay. it's the right size.  let's compare
				// bytes and check to see that they are the same
				
				FILE * fp_83 = fopen( e3_fullpath.c_str(), "rb" );
				FILE * fp_lfn = fopen( lfn_fullpath.c_str(), "rb" );

				if( fp_83 && fp_lfn ) {
					int diffs = 0;

					// skip in a bit
					fseek( fp_83, 0x800, SEEK_SET );
					fseek( fp_lfn, 0x800, SEEK_SET );
					
					// check 0x100 bytes.  On MP3s, thes will be different.
					// with these files, it should be enough.
					for( int b=0 ; b<0x100 ; b++ ) {
						int a1 = fgetc( fp_83 );
						int a2 = fgetc( fp_lfn );
						if( a1 != a2 ) diffs++;
					}
					
					fclose( fp_83 );
					fclose( fp_lfn );

					if( diffs == 0 ) {
						foundcount++;
						foundname.assign( entry->d_name );
					}
				}

			}
		}


		closedir( d );
	}

	// okay. there was a match. return it.
	if( foundcount >= 1 ) {
		return foundname;
	}

	return _path + eightthree;
}


#define kCopyFileBufSize	((4096) * 64 )
void DiskUtils::CopyFile( std::string fromPath, std::string toPath )
{
	char buf[ kCopyFileBufSize ];

	FILE * inf = fopen( fromPath.c_str(), "rb" );
	if( !inf ) {
		std::cerr << "Couldn't open input file " << fromPath << std::endl;
		return;
	}

	FILE * outf = fopen( toPath.c_str(), "wb" );
	if( !outf ) {
		std::cerr << "Couldn't open output file " << toPath << std::endl;
		fclose( inf );
		return;
	}


	size_t n = 0;
	do {
		n = fread( buf, 1, kCopyFileBufSize, inf );
		fwrite( buf, n, 1, outf );
	} while( n!= 0 );

	fclose( inf );
	fclose( outf );
}
