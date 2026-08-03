
#if !defined(ICOM_FRAMEWORK_MUTEXTES)
#define ICOM_FRAMEWORK_MUTEXTES

#include "XMemory.h"
#include "ICOMFramework.h"

namespace ICOM{ 
 namespace Framework{

class  Mutex : public XMemory
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    Mutex(MemoryManager* const manager = ICOMFramework::fgMemoryManager);

    ~Mutex();


    // -----------------------------------------------------------------------
    //  Lock control methods
    // -----------------------------------------------------------------------
    void lock();
    void unlock();


private :
    // -----------------------------------------------------------------------
    //  Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    Mutex(const Mutex&);
    Mutex& operator=(const Mutex&);


    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fHandle
    //      The raw mutex handle. Its just a void pointer so we do not
    //      pass judgement on its value at all. We just pass it into the
    //      platform utilities methods which knows what's really in it.
    // -----------------------------------------------------------------------
    void*   fHandle;


};


class  MutexLock : public XMemory
{
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
public:
    MutexLock(Mutex* const toLock);
    ~MutexLock();


private :
    // -----------------------------------------------------------------------
    //  Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    MutexLock();
    MutexLock(const MutexLock&);
    MutexLock& operator=(const MutexLock&);


    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fToLock
    //      The mutex object that we are locking
    // -----------------------------------------------------------------------
    Mutex*   fToLock;
};

} 
 }

#endif
