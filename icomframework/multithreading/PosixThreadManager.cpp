
extern "C"{
#include <pthread.h>
}
#include <errno.h>


#include "PosixThreadManager.h"
#include "PosixThreadManagerException.h"
#include "ICOMFramework.h"


#include <stdlib.h>
#include <assert.h>

using namespace ICOM::Framework;


PosixThreadManager::PosixThreadManager()
{
}


PosixThreadManager::~PosixThreadManager()
{
}

		
		

ThreadHandle	PosixThreadManager::createThread(void *(*thread_func)(void*), void *arg)
{
	/*
	 * Let's create the thread
	 */
	 


	pthread_t*	thread = (pthread_t*)ICOM::Framework::ICOMFramework::fgMemoryManager->allocate( sizeof(pthread_t) );
	
	int rc = pthread_create(thread,NULL,thread_func,arg);
						
	/*
	 * Check if thread is created successfully.
	 */	
	if(rc != 0){
		ICOMErrorHandlerFatal1(
			ICOM::Framework::ICOMFramework::fgErrorHandler,
			PosixThreadManagerException,
			ErrorCodes::ERROR_CREATE_THREAD,
			ICOMString::toString(rc).c_str()
		);
	}
	
	/*
	 * return the handle
	 */
	return (ThreadHandle)thread;
}

ThreadManager::State			PosixThreadManager::joinThread(ThreadHandle handle,int milliSeconds)
{
	/* TODO: Den Fall milliSeconds != -1 bitte implementieren */
	assert(milliSeconds == -1);

	assert( handle != 0 );

	/*
	 * Joint to th thread.
	 */
	int ret = pthread_join( *(pthread_t*)handle, 0 );
				
	/*
	 * Some error handling.
	 */
	if(ret != 0 && ret!=ESRCH){
		ICOMErrorHandlerFatal2(
				ICOM::Framework::ICOMFramework::fgErrorHandler,
			PosixThreadManagerException,
			ErrorCodes::ERROR_JOIN_THREAD,
			ICOMString::toString(ret).c_str(),
			ICOMString::toString(errno).c_str()
		);
	}

	return ThreadManager::State_Success;
}


void			PosixThreadManager::terminateThread(ThreadHandle handle)
{
	if(!pthread_cancel( *(pthread_t*)handle) ){
		ICOMErrorHandlerFatal1(
				ICOM::Framework::ICOMFramework::fgErrorHandler,
			PosixThreadManagerException,
			ErrorCodes::ERROR_TERMINATE_THREAD,
			ICOMString::toString(errno).c_str()
		);
	}
}

PosixThreadManager::State			PosixThreadManager::joinMultipleThreads(ThreadHandle* handles,int count,bool waitAll,int milliSeconds)
{
	/* TODO: Bitte implementieren */
	assert(false);
}

ThreadHandle				PosixThreadManager::createProcess(const char* cmdline)
{
	/* TODO: Bitte implementieren */
	assert(false);	
}

ThreadManager::State			PosixThreadManager::joinProcess(ThreadHandle handle, int milliSeconds)
{
	/* TODO: Bitte implementieren */
	assert(false);

	return ThreadManager::State_Success;
}


void			PosixThreadManager::terminateProcess(ThreadHandle handle)
{
	/* TODO: Bitte implementieren */
	assert(false);
}



 
