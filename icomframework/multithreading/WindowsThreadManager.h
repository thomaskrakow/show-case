
#ifndef _WINDOWSTHREADMANAGER_H_
#define _WINDOWSTHREADMANAGER_H_

#include "icomframework_config.h"
#include "ICOMFramework.h"
#include "ErrorHandler.h"
#include "ThreadManager.h"




namespace ICOM{
	namespace Framework{

	class WindowsThreadManager : public ThreadManager
	{
	public:
	
		// -----------------------------------------------------------------------
		//  Konstruktoren und Destruktor
		// -----------------------------------------------------------------------
		/** @name Konstruktoren und Destruktor */
		//@{
		
		WindowsThreadManager();


		/**	Der Standard-Destruktor.*/
		virtual ~WindowsThreadManager();
		//@}
		
		

		virtual ThreadHandle				createThread(void *(*thread_func)(void*), void *arg);
		virtual ThreadManager::State		joinThread(ThreadHandle handle,int milliSeconds = -1);
		virtual void						terminateThread(ThreadHandle handle);


		virtual ThreadHandle				createProcess(const char* cmdline);
		virtual ThreadManager::State		joinProcess(ThreadHandle handle, int milliSeconds = -1);
		virtual void						terminateProcess(ThreadHandle handle);



	protected:

	};
	}
}


#endif //_WINDOWSTHREADMANAGER_H_




 
