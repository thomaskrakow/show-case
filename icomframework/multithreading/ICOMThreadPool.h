
#ifndef _ICOM_FRAMEWORKTHREAD_H_
#define _ICOM_FRAMEWORKTHREAD_H_

#include "ICOMFramework.h"
#include "ErrorHandler.h"
#include "Runnable.h"
#include "ThreadManager.h"
#include "Mutexes.h"
#include "SignalManager.h"


#include <stack>
#include <set>

namespace ICOM{
	namespace Framework{
	
	
	/**
	 *  Portable Klasse zum Arbeiten mit Threads.
	 */
	class ICOMThreadPool : public XMemory
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
		 *  @param maxThreads Maximale Anzahl an Threads die dieser ThreadPool erzeugen darf.
		 */
		ICOMThreadPool(int maxThreads);

		/**	Der Standard-Destruktor.*/
		virtual ~ICOMThreadPool();
		//@}

		/*
		 * Erzeugt einen separaten Thread und ruft darin die Funktion runnable->run() auf. Laufen bereits fMaxThreads
		 * Threads gleichzeitig, dann wartet die Funktion maximal milliSeconds Millisekunden bis sich mindestens ein Thread beendet hat und erzeugt erst dann
		 * einen neuen Thread. Ist milliSeconds == -1, dann wartet die Funktion bis sich ein Thread beendet hat.
		 *
		 * Der R�ckgabewert ist State_Success wenn einer oder alle Threads beendet wurden. Ist der R�ckgabewert State_TimeOut, dann ist der im Parameter milliSeconds angegebene Timeout abgelaufen.
		 */
		virtual SignalManager::State	start(Runnable* runnable,int milliSeconds = -1,bool autoDelete = false);
		virtual SignalManager::State	start(const char* cmdline, int milliSeconds = -1);

		/*
		 * Wartet milliSeconds Millisekunden oder bis alle Threads beendet sind.
		 *
		 * Der R�ckgabewert ist State_Success wenn alle Threads beendet wurden. Ist der R�ckgabewert State_TimeOut, dann ist der im Parameter milliSeconds angegebene Timeout abgelaufen.
		 */
		virtual SignalManager::State	join(int milliSeconds = -1);


		/*
		 * Ist die maximale Anzahl an Threads in diesem Threadpool belegt, dann wartet diese Funktion $milliSeconds Millisekunden 
		 * bis wieder ein Thread erzeugt werden kann.
		 * Ist $milliSeconds == -1, dann wartet die Funktion unendlich lange.
		 * 
		 * Der Rückgabewert ist State_Success wenn ein neuer Thread erzeugt werden kann.
		 * Ist der Rückgabewert State_TimeOut, dann ist der im Parameter milliSeconds angegebene Timeout abgelaufen.
		 */
		virtual SignalManager::State	waitForFreePoolCapacity(int milliSeconds = -1);

	protected:

		virtual void	exceptionThrown(ICOM::Framework::ICOMException& exc);
		virtual void	rethrow();
		
	private:

		struct Argument {
			ThreadHandle	threadHandle;
			Runnable*		runnable;
			ICOMThreadPool*	threadPool;
		};

		class ThreadSurveillance : public XMemory {
		public:
			ThreadSurveillance(Argument* argument);
			~ThreadSurveillance();
		private:
			Argument*		fArgument;
		};

		static void*								wrapper_for_calling_run(void* arg);
	
		int											fMaxThreads; /* Maximale Anzahl an Threads die gleichzeitig laufen d�rfen. */
		ICOM::Framework::Mutex						fMutex;
		std::stack<ICOM::Framework::ICOMException>	fExceptionsThrownByThread;
		std::set<Argument*>							fArgumentsOfRunningThreads;
		SignalHandle								fSignalHandle; /* Ist gesetzt, wenn die maximale Anzahl an Threads läuft und ist signalisiert, wenn sich ein Thread beendet hat. */
		SignalHandle								fSignalHandleAll; /* Ist signalisiert, wenn alle Threads beendet sind. */
	};
	}
}


#endif //_ICOM_FRAMEWORKTHREAD_H_




 
