

#include "NoThreadMutexManager.h"

namespace ICOM{ 
 namespace Framework{

/*
	The NoThread mutex manager is for use where no threading is used
	in an environment. Since no threading is used, mutexes are not
	needed, so the implementation does essentially nothing.
*/


NoThreadMutexManager::NoThreadMutexManager()
{
}


NoThreadMutexManager::~NoThreadMutexManager()
{
}


MutexHandle NoThreadMutexManager::create(MemoryManager* const manager)
{
    return 0;
}


void NoThreadMutexManager::destroy(MutexHandle mutex, MemoryManager* const manager)
{
}


void NoThreadMutexManager::lock(MutexHandle mutex)
{
}


void NoThreadMutexManager::unlock(MutexHandle mutex)
{
}


} 
 }

