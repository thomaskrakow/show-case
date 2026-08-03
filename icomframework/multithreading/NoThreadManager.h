
#ifndef _NOTHREADMANAGER_H_
#define _NOTHREADMANAGER_H_

#include "icomframework_config.h"
#include "ICOMFramework.h"
#include "ErrorHandler.h"
#include "ThreadManager.h"




namespace ICOM{
	namespace Framework{

	class NoThreadManager : public ThreadManager
	{
	public:
	
		// -----------------------------------------------------------------------
		//  Konstruktoren und Destruktor
		// -----------------------------------------------------------------------
		/** @name Konstruktoren und Destruktor */
		//@{
		
		NoThreadManager();


		/**	Der Standard-Destruktor.*/
		virtual ~NoThreadManager();
		//@}
		
		

		virtual ThreadHandle				createThread(void *(*thread_func)(void*), void *arg);
		virtual ThreadManager::State		joinThread(ThreadHandle handle,int milliSeconds = -1);
		virtual void						terminateThread(ThreadHandle handle);



	protected:

	};
	}
}


#endif //_NOTHREADMANAGER_H_




 
