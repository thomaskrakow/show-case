
#ifndef _ICOM_FRAMEWORK_JANITORFILE_H_
#define _ICOM_FRAMEWORK_JANITORFILE_H_

#include "ICOMFramework.h"
#include "ErrorHandler.h"
#include "Runnable.h"
#include "ThreadManager.h"




namespace ICOM{
	namespace Framework{
	
	
	class JanitorFile : public XMemory
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
		JanitorFile(const char*	filename);

		/**	Der Standard-Destruktor.*/
		virtual ~JanitorFile();
		//@}

		virtual void			reset(const char* file);
		virtual const char*		getFileName() const;
		
		
	private:

		const char*		fFileName;
	



	};
	}
}


#endif //_ICOM_FRAMEWORK_JANITORFILE_H_




 
