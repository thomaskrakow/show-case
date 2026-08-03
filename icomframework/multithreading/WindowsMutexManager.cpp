

#include <windows.h>

#include "XMemory.h"
#include "WindowsMutexManager.h"
#include "MemoryManager.h"

namespace ICOM{ 
 namespace Framework{

// Wrap up the critical section with XMemory
//
class CSWrap: public XMemory
{
public:
  CRITICAL_SECTION cs;
};

WindowsMutexManager::WindowsMutexManager(ErrorHandler* errorHandler,MemoryManager* manager) : 
	fMemoryManager(manager),
	fErrorHandler(errorHandler)
{
}


WindowsMutexManager::~WindowsMutexManager()
{
}


MutexHandle WindowsMutexManager::create(MemoryManager* const manager)
{
    CSWrap* mutex = new (manager) CSWrap;
    InitializeCriticalSection(&mutex->cs);
    return mutex;
}


void WindowsMutexManager::destroy(MutexHandle mtx, MemoryManager* const)
{
    CSWrap* mutex = (CSWrap*)mtx;
    if (mutex != 0)
    {
      ::DeleteCriticalSection(&mutex->cs);
      delete mutex;
    }
}


void WindowsMutexManager::lock(MutexHandle mtx)
{
    CSWrap* mutex = (CSWrap*)mtx;
    ::EnterCriticalSection(&mutex->cs);
}


void WindowsMutexManager::unlock(MutexHandle mtx)
{
    CSWrap* mutex = (CSWrap*)mtx;
    ::LeaveCriticalSection(&mutex->cs);
}


} 
 }
