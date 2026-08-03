

#if !defined(ICOM_FRAMEWORK_WINDOWSMUTEXMANAGER)
#define ICOM_FRAMEWORK_WINDOWSMUTEXMANAGER

#include "MutexManager.h"
#include "ErrorHandler.h"

namespace ICOM{ 
 namespace Framework{

/*
	The mutex manager to use on MS Windows platforms
*/
class WindowsMutexManager : public MutexManager
{
    public:
        WindowsMutexManager(ErrorHandler* errorHandler = ICOMFramework::fgErrorHandler,MemoryManager* manager = ICOMFramework::fgMemoryManager);
        virtual ~WindowsMutexManager();

		// Mutex operations
		virtual MutexHandle		create(MemoryManager* const manager);
		virtual void			destroy(MutexHandle mutex, MemoryManager* const manager);
		virtual void			lock(MutexHandle mutex);
		virtual void			unlock(MutexHandle mutex);

	protected:

		MemoryManager*			fMemoryManager;
		ErrorHandler*			fErrorHandler;
};

} 
 }


#endif

