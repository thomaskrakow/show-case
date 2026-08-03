

#if !defined(ICOM_FRAMEWORK_NOTHREADMUTEXMANAGER)
#define ICOM_FRAMEWORK_NOTHREADMUTEXMANAGER

#include "MutexManager.h"

namespace ICOM{ 
 namespace Framework{

/*
	The NoThread mutex manager is for use where no threading is used
	in an environment. Since no threading is used, mutexes are not
	needed, so the implementation does essentially nothing.
*/
class NoThreadMutexManager : public MutexManager
{
    public:
        NoThreadMutexManager();
        virtual ~NoThreadMutexManager();

		// Mutex operations
		virtual MutexHandle		create(MemoryManager* const manager);
		virtual void			destroy(MutexHandle mutex, MemoryManager* const manager);
		virtual void			lock(MutexHandle mutex);
		virtual void			unlock(MutexHandle mutex);
};

} 
 }


#endif

