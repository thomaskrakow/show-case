
#include "NoThreadManager.h"


using namespace ICOM::Framework;


NoThreadManager::NoThreadManager()
{
}


NoThreadManager::~NoThreadManager()
{
}

		
		

ThreadHandle	NoThreadManager::createThread(void *(*thread_func)(void*), void *arg)
{
	(*thread_func)(arg);
	return (ThreadHandle)0;
}

ThreadManager::State			NoThreadManager::joinThread(ThreadHandle handle, int milliSeconds)
{
	return ThreadManager::State_Success;
}


void			NoThreadManager::terminateThread(ThreadHandle handle)
{

}






 
