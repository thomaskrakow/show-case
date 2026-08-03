

#if !defined(ICOM_FRAMEWORK_MUTEXMANAGER)
#define ICOM_FRAMEWORK_MUTEXMANAGER

#include "XMemory.h"

namespace ICOM{ 
 namespace Framework{

typedef void* MutexHandle;

//	Abstract class for mutex implementation.
//  This is be used to allow multiple mutex handling implementations.
class MutexManager : public XMemory
{
    public:
        MutexManager() {}
        virtual ~MutexManager() {}

		// Mutex operations
		virtual MutexHandle		create(MemoryManager* const manager) = 0;
		virtual void			destroy(MutexHandle mutex, MemoryManager* const manager) = 0;
		virtual void			lock(MutexHandle mutex) = 0;
		virtual void			unlock(MutexHandle mutex) = 0;
};

} 
 }


#endif

