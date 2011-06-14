/*
 * filename :	path.h
 *
 * programmer :	Cao Jiayin
 */

#ifndef	SORT_PATH
#define	SORT_PATH

#if defined(SORT_IN_WINDOWS)
#include <windows.h>
#endif

// get current directory
string GetExecutableDir()
{
	const int maxLen = 2048;
	char buf[ maxLen ];

#if defined(SORT_IN_LINUX)
	int c = readlink( "/proc/self/exe", buf, maxLen - 1 );
#elif defined(SORT_IN_WINDOWS)
	// get the current module filename
	GetModuleFileNameA( NULL , buf , maxLen );

	// remove the file name
	int len = strlen( buf );
	for( int i = len - 1 ; i > 0 ; i-- )
	{
		if( buf[i] == '\\' )
		{
			buf[i+1] = 0;
			break;
		}
	}
#endif

	return string(buf);
}

#endif