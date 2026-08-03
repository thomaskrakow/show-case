
#if !defined(ICOM_FRAMEWORK_TRANSCODERCP1252)
#define ICOM_FRAMEWORK_TRANSCODERCP1252

#include "Transcoder256Table.h"
#include "ICOMFrameworkTypes.h"


namespace ICOM{ 
 namespace Framework{

	class  TranscoderCP1252 : public Transcoder256Table
	{
	public :	
	
		
		TranscoderCP1252();
		virtual ~TranscoderCP1252();
		


		
	private :
		// -----------------------------------------------------------------------
		//  Unimplemented constructors and operators
		// -----------------------------------------------------------------------		
		TranscoderCP1252(const TranscoderCP1252&);
		TranscoderCP1252& operator=(const TranscoderCP1252&);

	};

} 
 }

#endif
