/*
   FileName:      logmanager.h

   Created Time:  2011-08-04 12:47:16

   Auther:        Cao Jiayin

   Email:         soraytrace@hotmail.com

   Location:      China, Shanghai

   Description:   SORT is short for Simple Open-source Ray Tracing. Anyone could checkout the source code from
                'sourceforge', https://soraytrace.svn.sourceforge.net/svnroot/soraytrace. And anyone is free to
                modify or publish the source code. It's cross platform. You could compile the source code in 
                linux and windows , g++ or visual studio 2008 is required.
*/

#ifndef	SORT_LOGMANAGER
#define	SORT_LOGMANAGER

// include the header file
#include "utility/singleton.h"
#include <fstream>

// a empty class for output a new line
class _ENDL{
public:
	_ENDL( bool flag ){ m_bCrash = flag; }
	
	//whether crash after the message
	bool m_bCrash;
};

/////////////////////////////////////////////////////////////
// definition of log manager
class	LogManager : public Singleton<LogManager>
{
// public method
public:
	// destructor
	~LogManager();
	
	// enable or disable log
	// para 'enable' : enable or disable the log system
	void SetLogEnabled( bool enable );
	
	// set the header of log
	void SetHeader( const string& str );

// private method
private:
	// the logger file
	std::ofstream 	m_fileLog;
	// the file name of logger
	const std::string	m_sLogFileName;
	// enable log
	bool	m_bLogEnable;

// private method
	// private constructor to avoid second instance in the app
	LogManager( bool enable = true );
	// initialize the system
	void	_init();
	// release the system
	void	_release();

	// set friend class
	friend LogManager& operator<<( LogManager& , unsigned );
	friend LogManager& operator<<( LogManager& , int );
	friend LogManager& operator<<( LogManager& , float );
	friend LogManager& operator<<( LogManager& , const char* );
	friend LogManager& operator<<( LogManager& , const std::string& );
	friend LogManager& operator<<( LogManager& , _ENDL );

	friend class Singleton<LogManager>;
};

// declare some useful macro for convinience
#define	LOG_FILE_INFO	"(File:"<<__FILE__<<"  Line:"<<__LINE__<<") "
#define LOG				LogManager::GetSingleton()
#define LOG_WARNING		LOG<<LOG_FILE_INFO<<"WARNING: "
#define	LOG_ERROR		LOG<<LOG_FILE_INFO<<"ERROR: "
#define	LOG_HEADER(str)	LogManager::GetSingleton().SetHeader( str )

#define ENDL _ENDL(false)
#define	CRASH _ENDL(true)

// the log manager stream operator
LogManager& operator<<( LogManager& , unsigned );
LogManager& operator<<( LogManager& , int );
LogManager& operator<<( LogManager& , const char* );
LogManager& operator<<( LogManager& , float );
LogManager& operator<<( LogManager& , const std::string& );
LogManager& operator<<( LogManager& , _ENDL );

#endif
