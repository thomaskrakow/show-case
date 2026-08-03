
#ifndef _ICOM_FRAMEWORKTHREAD_H_
#define _ICOM_FRAMEWORKTHREAD_H_

#include "ICOMFramework.h"
#include "ErrorHandler.h"
#include "Runnable.h"
#include "ThreadManager.h"




namespace ICOM{
	namespace Framework{
	
	
	/**
	 *  Portable Klasse zum Arbeiten mit Threads.
	 */
	class ICOMThread : public XMemory
	{
	public:
	
		// -----------------------------------------------------------------------
		//  Konstruktoren und Destruktor
		// -----------------------------------------------------------------------
		/** @name Konstruktoren und Destruktor */
		//@{
		
		/**
		 *  Konstruktor
		 *
		 *  @param runnable Klasse vom Typ Runnable.
		 */
		ICOMThread(Runnable*	runnable);

		/**	Der Standard-Destruktor.*/
		virtual ~ICOMThread();
		//@}
		
		/**
		 *  Startet einen Thread.
		 *
		 *  Ruft die Funktion run der Variablen #fRunnable auf.
		 */
		virtual void			start();
		
		/**
		 *  Wartet solange bis die Funktion run sich beendet.
		 */
		virtual void			join(int milliSeconds = -1);


		/**
		 *  Beendet einen Thread (Aufräumprozesse können inerhalb des Threads nicht statt finden.)
		 */
		virtual void			terminate();

	protected:

		
	private:

		static void*				wrapper_for_calling_run(void* runnable);
	
		Runnable*				fRunnable;
		ThreadHandle			fThreadHandle;
	



	};
	}
}


#endif //_ICOM_FRAMEWORKTHREAD_H_




 
