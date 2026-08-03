
#ifndef _POSIXTHREADMANAGER_H_
#define _POSIXTHREADMANAGER_H_

#include "pdflib_config.h"
#include "ICOMFramework.h"
#include "ErrorHandler.h"
#include "ThreadManager.h"




namespace ICOM{
	namespace Framework{

	class PosixThreadManager : public ThreadManager
	{
	public:
	
		// -----------------------------------------------------------------------
		//  Konstruktoren und Destruktor
		// -----------------------------------------------------------------------
		/** @name Konstruktoren und Destruktor */
		//@{
		
		PosixThreadManager();


		/**	Der Standard-Destruktor.*/
		virtual ~PosixThreadManager();
		//@}
		
		

		virtual ThreadHandle	createThread(void *(*thread_func)(void*), void *arg);
		virtual State			joinThread(ThreadHandle handle,int milliSeconds = -1);
		virtual State			joinMultipleThreads(ThreadHandle* handles,int count,bool waitAll,int milliSeconds = -1);
		virtual void			terminateThread(ThreadHandle handle);
		
		virtual ThreadHandle				createProcess(const char* cmdline);
		virtual ThreadManager::State		joinProcess(ThreadHandle handle, int milliSeconds = -1);
		virtual void						terminateProcess(ThreadHandle handle);



	};
	}
}


#endif //_POSIXTHREADMANAGER_H_




 
