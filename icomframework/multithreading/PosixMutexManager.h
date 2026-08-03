

#if !defined(ICOM_FRAMEWORK_POSIXMUTEXMANAGER)
#define ICOM_FRAMEWORK_POSIXMUTEXMANAGER

#include "MutexManager.h"

namespace ICOM{ 
 namespace Framework{

//	Posix mutex implementation.
class PosixMutexManager : public MutexManager
{
    public:
        PosixMutexManager();
        virtual ~PosixMutexManager();

		// Mutex operations
		virtual MutexHandle		create(MemoryManager* const manager);
		virtual void			destroy(MutexHandle mutex, MemoryManager* const manager);
		virtual void			lock(MutexHandle mutex);
		virtual void			unlock(MutexHandle mutex);
};

} 
 }


#endif

