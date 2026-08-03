

#if !defined( ICOMJDF_ABSTRACTRESOURCE_H )
#define ICOMJDF_ABSTRACTRESOURCE_H

#include "XMemory.h"
#include "AbstractResourcePool.h"
#include "JDFIntegerList.h"


namespace ICOM{
	namespace JDF{

		class ResourceLink;

		class AbstractResource : public AbstractResourcePool
		{

			friend class JDFNode;
			friend class AbstractResourcePool;

		public:			

			enum Class{
				Class_Parameter,
				Class_Quantity,
				Class_Intent,
				Class_Consumable,
				Class_Special /* Wird gesondert behandelt. (z.b. Part ist vom Typ Special */
			};

			enum Status{
				Status_Available,
				Status_Unavailable
			};

			enum PipeProtocol{
				PipeProtocol_NotSet,
				PipeProtocol_Internal,
				PipeProtocol_JMF,
				PipeProtocol_JMFPush,
				PipeProtocol_JMFPull,
				PipeProtocol_None				
			};

	
			/** @name Konstruktoren und Destruktor */
			//@{			
			virtual ~AbstractResource();
			//@}
	
			/** @name Getter Methoden */
			//@{
			
			virtual const XMLCh*			getID();

			//@}

			virtual void					setStatus(Status status);
			virtual void					setPipeProtocol(PipeProtocol pipeProtocol);
			virtual void					OnExtendDOM(xercesc::DOMDocument* document,xercesc::DOMElement* element);

			

		protected:

			AbstractResource(Class cl,AbstractResourcePool* resourcePool);

			

		private:

			Class					fClass;
			Status					fStatus;
			XMLCh*					fID;
			PipeProtocol			fPipeProtocol;

			ICF_uint16				fRefCount; /* Anzahl der ResourceRef-Elemente, welche auf diese Ressource verweisen */

	
		};
}

}

#endif /*ICOMJDF_ABSTRACTRESOURCE_H*/


 
