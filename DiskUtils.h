/* Sony voice recorder MSF reader
   using reverse-engineered structure

   Scott Lawrence 2013
*/

#include <iostream>

class DiskUtils
{
public:
	DiskUtils( void );
	~DiskUtils( void );

public:
	static bool IsValidDir( std::string _path );



	static long FileSize( std::string _path );

	// LFNFrom83
	// 	determine the long filename associated with the 8.3 pased in
	// 	the problem is that the lfn through a regular algo could map to multiple names
	// 	since we won't have a lot of files, let's just dumb-match it
	//
	//	so first, we check filesizes
	//	for alike-sizes, we check a few bytes at 0x800 bytes in, where files tend to differ.
	static std::string LFNFrom83( std::string _path, std::string eightthree );

	static void CopyFile( std::string fromPath, std::string toPath );
};
