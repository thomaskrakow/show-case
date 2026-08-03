
#ifndef _ICOM_FRAMEWORK_JANITORFUNC_H_
#define _ICOM_FRAMEWORK_JANITORFUNC_H_

#include "ICOMFramework.h"
#include "ErrorHandler.h"
#include "Runnable.h"
#include "ThreadManager.h"




namespace ICOM{
	namespace Framework{
	
	/*
	 * Übergeben wird ein Funktionszeiger, welcher im Desktuktor aufgerufen wird.
	 *
	 * Diese Klasse kann dazu genutzt werden Aufräumroutinen auszuführen. Dies mach gerade dann Sinn, wenn die
	 * Aufräumfunktionen sonst an mehreren Stellen in einer Funktion aufgerufen werden würden.
	 */
	class JanitorFunc : public XMemory
	{
	public:
	
		// -----------------------------------------------------------------------
		//  Konstruktoren und Destruktor
		// -----------------------------------------------------------------------
		/** @name Konstruktoren und Destruktor */
		//@{
		
		/**
		 *  Konstruktor
		 */
		JanitorFunc(void func());

		/**	Der Standard-Destruktor.*/
		virtual ~JanitorFunc();
		//@}

	
		
	private:

		void (*fFunc)();
	



	};
	}
}


#endif //_ICOM_FRAMEWORK_JANITORFUNC_H_




 
