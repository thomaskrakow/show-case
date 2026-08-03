

#if !defined( ICOMJDF_JDFURI_H )
#define ICOMJDF_JDFURI_H

#include "XMemory.h"

#include <xercesc/util/XMLString.hpp>


namespace ICOM{
	namespace JDF{

		class JDFURI : public ICOM::Framework::XMemory {
		public:

			enum Sheme{
				Sheme_File
			};

	
			/** @name Konstruktoren und Destruktor */
			//@{

			JDFURI(const char* path,Sheme sheme = Sheme_File);		
			

			virtual ~JDFURI();
			//@}
	
			/** @name Getter Methoden */
			//@{
			virtual const XMLCh*	getURI() const;

			//@}

			JDFURI*		duplicate() const;



	
	
		private:
		
			JDFURI();
			JDFURI(const JDFURI& e);
			
			Sheme		fSheme;
			XMLCh*		fURI;
	
		};
}

}

#endif /*ICOMJDF_JDFURI_H*/


 
