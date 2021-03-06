/*
   FileName:      memmanager.h

   Created Time:  2011-08-04 12:47:31

   Auther:        Cao Jiayin

   Email:         soraytrace@hotmail.com

   Location:      China, Shanghai

   Description:   SORT is short for Simple Open-source Ray Tracing. Anyone could checkout the source code from
                'sourceforge', https://soraytrace.svn.sourceforge.net/svnroot/soraytrace. And anyone is free to
                modify or publish the source code. It's cross platform. You could compile the source code in 
                linux and windows , g++ or visual studio 2008 is required.
*/

#ifndef	SORT_MEMMANAGER
#define	SORT_MEMMANAGER

// include the header
#include "utility/singleton.h"
#include "platform/multithread/multithread.h"
#include "logmanager.h"
#include <map>

struct Memory
{
	char*		m_memory;
	unsigned	m_offset;
	unsigned	m_size;

	Memory()
	{
		m_memory=0;
		m_offset=0;
		m_size=0;
	}
	~Memory()
	{
		delete[] m_memory;
		m_offset=0;
		m_size=0;
	}
};

//////////////////////////////////////////////////////////////////////////////////
// definition of memory manager
// desc :	Memory manager could allocate small memory space efficiently. Actually
//			it would pre-allocate some memmory space , it will return alloacted space 
//			when asked for memory space , not really allocate it so that to save
//			more time on allocating memory.
class	MemManager : public Singleton<MemManager>
{
// public method
public:
	// default constructor
	MemManager();
	// destructor
	~MemManager();

	// pre-allocate memory
	void PreMalloc( unsigned size , unsigned id = 0 );

	// clear the allocated memory
	void ClearMem( unsigned id=0 );

	// de-allocate memory
	void DeAlloc( unsigned id=0 );

	// get pointer and set the offset
	template< typename T >
	T* GetPtr(unsigned count , unsigned id=0)
	{
		// get the memory pointer first
		Memory* mem = _getMemory(id);

		if( mem == 0 )
			LOG_ERROR<<"No memory with id "<<id<<"."<<CRASH;

		// check if the memory is enough
		if( sizeof( T ) * count + mem->m_offset > mem->m_size )
			LOG_ERROR<<"There is not enough memory in memory manager.(mem id:"<<id<<")"<<CRASH;

		unsigned addr = mem->m_offset;
		mem->m_offset += sizeof(T) * count;
		return (T*)(mem->m_memory + addr);
	}

	// get the offset of the memory
	unsigned GetOffset( unsigned id=0 ) const
	{
		Memory* mem = _getMemory(id);
		if( mem == 0 )
			LOG_ERROR<<"No memory with id "<<id<<"."<<CRASH;
		return mem->m_offset;
	}

// private field
private:
	// the memories
	map<unsigned,Memory*> m_MemPool;

	// get memory
	Memory*	_getMemory( unsigned id ) const
	{
		map<unsigned,Memory*>::const_iterator it = m_MemPool.find(id);
		if( it != m_MemPool.end() )
			return it->second;
		return 0;
	}

	// dealloc all memory
	void _deallocAllMemory()
	{
		map<unsigned,Memory*>::const_iterator it = m_MemPool.begin();
		while( it != m_MemPool.end() )
		{
			delete it->second;
			it++;
		}
		m_MemPool.clear();
	}

	friend class Singleton<MemManager>;
};

// allocate memory
#define	SORT_MALLOC(T) SORT_MALLOC_ID(T,ThreadId())
#define SORT_MALLOC_ID(T,id) new (MemManager::GetSingleton().GetPtr<T>(1,id)) T
#define SORT_MALLOC_ARRAY(T,c) SORT_MALLOC_ARRAY_ID( T , c , ThreadId() )
#define	SORT_MALLOC_ARRAY_ID(T,c,id) new (MemManager::GetSingleton().GetPtr<T>(c,id)) T

// get sort memory
#define	SORT_MEMORY_ID(T,id) MemManager::GetSingleton().GetPtr<T>(0,id)

// premalloc memory
inline void SORT_PREMALLOC(unsigned size , unsigned id=0)
{
	MemManager::GetSingleton().PreMalloc(size,id);
}

//clear memory
inline void SORT_CLEARMEM(unsigned id=0)
{
	MemManager::GetSingleton().ClearMem(id);
}

// dealloc
inline void SORT_DEALLOC(unsigned id=0)
{
	MemManager::GetSingleton().DeAlloc(id);
}

// get the offset
inline unsigned SORT_OFFSET(unsigned id=0)
{
	return MemManager::GetSingleton().GetOffset(id);
}

#endif
