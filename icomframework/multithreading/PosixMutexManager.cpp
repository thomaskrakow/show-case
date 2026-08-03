
// on some platforms, THREAD_MUTEX_RECURSIVE is defined only if _GNU_SOURCE is defined
#ifndef _GNU_SOURCE
 #define _GNU_SOURCE
#endif

#include <pthread.h>
#include <errno.h>

#include "PosixMutexManager.h"
#include "PosixMutexManagerException.h"
#include "ErrorHandler.h"


namespace ICOM{ 
 namespace Framework{


//	Wrap up the mutex with XMemory
class PosixMutexWrap : public XMemory {
public:
	pthread_mutex_t	m;
};


PosixMutexManager::PosixMutexManager()
{
}


PosixMutexManager::~PosixMutexManager()
{
}


MutexHandle PosixMutexManager::create(MemoryManager* const manager)
{
    PosixMutexWrap* mutex = new PosixMutexWrap;
    
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    
    if (pthread_mutex_init(&mutex->m, &attr)){
		ICOMErrorHandlerFatal1(
				ICOM::Framework::ICOMFramework::fgErrorHandler,
				PosixMutexManagerException,
				ErrorCodes::ERROR_INIT_MUTEX,
				ICOMString::toString(errno).c_str()
		);
    }
        
    pthread_mutexattr_destroy(&attr);

    return (void*)(mutex);
}


void PosixMutexManager::destroy(MutexHandle mutex, MemoryManager* const manager)
{
	PosixMutexWrap* _mutex = (PosixMutexWrap*)(mutex);
    if (_mutex != NULL)
    {
        if (pthread_mutex_destroy(&_mutex->m))
        {
    		ICOMErrorHandlerFatal1(
    				ICOM::Framework::ICOMFramework::fgErrorHandler,
    				PosixMutexManagerException,
    				ErrorCodes::ERROR_DESTROY_MUTEX,
    				ICOMString::toString(errno).c_str()
    		);
        }
        delete _mutex;
    }
}


void PosixMutexManager::lock(MutexHandle mutex)
{
	PosixMutexWrap* _mutex = (PosixMutexWrap*)(mutex);
    if (_mutex != NULL)
    {
        if (pthread_mutex_lock(&_mutex->m)){
        	ICOMErrorHandlerFatal1(
        	  	ICOM::Framework::ICOMFramework::fgErrorHandler,
        	  	PosixMutexManagerException,
        	  	ErrorCodes::ERROR_LOCK_MUTEX,
        	  	ICOMString::toString(errno).c_str()
        	 );

        }
    }
}


void PosixMutexManager::unlock(MutexHandle mutex)
{
	PosixMutexWrap* _mutex = (PosixMutexWrap*)(mutex);
    if (_mutex != NULL)
    {
        if (pthread_mutex_unlock(&_mutex->m))
        	ICOMErrorHandlerFatal1(
        	  	ICOM::Framework::ICOMFramework::fgErrorHandler,
        	  	PosixMutexManagerException,
        	  	ErrorCodes::ERROR_UNLOCK_MUTEX,
        	  	ICOMString::toString(errno).c_str()
        	 );

        }
    }
}


} 


