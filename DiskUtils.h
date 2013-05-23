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
	// is a path valid?
	static bool IsValidDir( std::string _path );

	// how big is the file?
	static long FileSize( std::string _path );

	// are the two files reasonably similar to be considered the same mp3?
	// 	- same file size, same bunch of bytes in the middle.
	// 	- NOTE: this is NOT extensive.
	static bool IsSimilarFile( std::string _pathA, std::string _pathB );

	// LFNFrom83
	// 	determine the long filename associated with the 8.3 pased in
	// 	the problem is that the lfn through a regular algo could map to multiple names
	// 	since we won't have a lot of files, let's just dumb-match it
	//
	//	so first, we check filesizes
	//	for alike-sizes, we check a few bytes at 0x800 bytes in, where files tend to differ.
	static std::string LFNFrom83( std::string _path, std::string eightthree );

	// copy a file from path fromPath to path toPath
	static void CopyFile( std::string fromPath, std::string toPath );
};
