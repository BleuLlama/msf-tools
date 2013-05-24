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
#include <iomanip>	/* string formatting */
#include <stdlib.h>	/* for GETENV */

#include "DiskUtils.h"
#include "VoiceFile.h"


void usage( char * av0 )
{
	std::cout << "Usage:  " << av0 << " <LOCAL PATH TO SAVE TO>" << std::endl;
	std::cout << std::endl;
	std::cout << "note: define VOICERECORDER env var to set root path." << std::endl;
	std::cout << "  eg:" << std::endl;
	std::cout << "      export VOICERECORDER=/Volumes/IC_RECORDER" << std::endl;
	std::cout << "      export VOICERECORDER=/v/" << std::endl;
	std::cout << "      export VOICERECORDER=v:\\" << std::endl;

	std::cout << std::endl;
	std::cout << "Version 1.0  yorgle@gmail.com  Scott Lawrence" << std::endl;
}

int main( int argc, char ** argv )
{
	char * pth;

	pth = getenv( "VOICERECORDER" );
	if( !pth ) {
		usage( argv[0] );
		return -1;
	}

	if( argc != 2 ) {
		usage( argv[0] );
		return -3;
	}

	std::string targetDir( argv[1] );
	if( !DiskUtils::IsValidDir( targetDir ))
	{
		DiskUtils::MakeDir( targetDir );

		if( !DiskUtils::IsValidDir( targetDir ))
		{
			std::cerr << "Unable to use target directory: " << targetDir << std::endl;
			return -4;
		}
	}

	// NOTE: on windows, put in a scan for a drive with label "IC_RECORDER"

	VoiceFile vf( pth );
	if( !vf.Valid() ) {
		std::cerr << "Directory is invalid: " << pth << std::endl;
		return -2;
	}


	for( int f=0 ; f<vf.GetFolderCount() ; f++ )
	{
		int fc = vf.GetFileCountForFolder( f );
		if( fc > 0 ) {
			std::string plural = "s";
			if( fc == 1 ) plural = "";

			std::cout << std::endl;
			std::cout << vf.GetFolderName( f ) << ": Extracting " << fc << " item" << plural << "." << std::endl;
	
			std::string folderPath = vf.GetPathForFolder( f );

			for( int i=0 ; i < fc ; i ++ ) {
				std::string sourceFile( folderPath );
				sourceFile.append( "/" );
				sourceFile.append( vf.GetFileInFolder( i, f ));

				std::string destFile( targetDir );
				destFile.append( "/" );
				destFile.append( vf.GetFolderName( f ));

				DiskUtils::MakeDir( destFile );

				destFile.append( "/" );
				destFile.append( vf.SortableDateForFileInFolder( i, f ));
				destFile.append( "__" );
				destFile.append( vf.GetFileInFolder( i, f ) );

				float fs = ((float)DiskUtils::FileSize( sourceFile )) / 1024.0/1024.0; 
				std::cout << "  Copying " << vf.GetFileInFolder( i, f )
					  << "  --  " << vf.PrintableDateForFileInFolder( i, f ) 
					  << " (" << std::setprecision( 2 ) << fs << " MB)";

				bool skipped = DiskUtils::CopyFile( sourceFile, destFile );
				if( skipped ) {
					std::cout << " (skipped)";
				}
				std::cout << std::endl;
			}
		}
	}

	return 0;
}
