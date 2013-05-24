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

class DiskUtils
{
public:
	DiskUtils( void );
	~DiskUtils( void );

public:
	// create a directory
	static void MakeDir( std::string _path );

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
	// returns TRUE if skipped
	static bool CopyFile( std::string fromPath, std::string toPath, bool skipIfExists = true );
};
