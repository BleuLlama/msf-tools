/* Sony voice recorder MSF reader
   using reverse-engineered structure

   Scott Lawrence 2013
*/
#include <iostream>
#include <stdlib.h>	/* for GETENV */

#include "DiskUtils.h"
#include "VoiceFile.h"


void usage( char * av0 )
{
	std::cout << "Usage:  " << av0 << std::endl;
	std::cout << std::endl;
	std::cout << "note: define VOICERECORDER env var to set root path." << std::endl;
	std::cout << "  eg:" << std::endl;
	std::cout << "      export VOICERECORDER=/Volumes/IC_RECORDER" << std::endl;
	std::cout << "      export VOICERECORDER=/v/" << std::endl;
	std::cout << "      export VOICERECORDER=v:\\" << std::endl;
}

int main( int argc, char ** argv )
{
	char * pth;

	pth = getenv( "VOICERECORDER" );
	if( !pth ) {
		usage( argv[0] );
		return -1;
	}

	// NOTE: on windows, put in a scan for a drive with label "IC_RECORDER"

	VoiceFile vf( pth );
	if( !vf.Valid() ) {
		std::cerr << "Directory is invalid: " << pth << std::endl;
		return -2;
	}
	return vf.Scan();
}
