


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include "ICOMFramework.h"
#include "Mutexes.h"
#include "MutexManager.h"

namespace ICOM{ 
 namespace Framework{

// ---------------------------------------------------------------------------
//  Mutex: Constructors and Destructor
// ---------------------------------------------------------------------------
Mutex::Mutex(MemoryManager* const manager) :
    fHandle(0)
{
    // Ask the per-platform driver to make us a mutex
    fHandle = ICOMFramework::fgMutexManager->create(manager);
}


Mutex::~Mutex()
{
    if (fHandle)
    {
		ICOMFramework::fgMutexManager->destroy(fHandle,ICOMFramework::fgMemoryManager);
        fHandle = 0;
    }
}


// ---------------------------------------------------------------------------
//  Mutex: Lock control methods
// ---------------------------------------------------------------------------
void Mutex::lock()
{
    ICOMFramework::fgMutexManager->lock(fHandle);
}

void Mutex::unlock()
{
    ICOMFramework::fgMutexManager->unlock(fHandle);
}



// ---------------------------------------------------------------------------
//  MutexLock: Constructors and Destructor
// ---------------------------------------------------------------------------
MutexLock::MutexLock(Mutex* const toLock) :

    fToLock(toLock)
{
    fToLock->lock();
}


MutexLock::~MutexLock()
{
    fToLock->unlock();
}

} 
 }
