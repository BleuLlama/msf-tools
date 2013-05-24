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

	// NOTE: on windows, put in a scan for a drive with label "IC_RECORDER"

	VoiceFile vf( pth );
	if( !vf.Valid() ) {
		std::cerr << "Directory is invalid: " << pth << std::endl;
		return -2;
	}

	int ret = vf.Dump();

	return ret;
}
