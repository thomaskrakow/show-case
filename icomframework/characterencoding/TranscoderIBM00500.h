
#if !defined(ICOM_FRAMEWORK_TRANSCODERIBM00500)
#define ICOM_FRAMEWORK_TRANSCODERIBM00500

#include "Transcoder256Table.h"
#include "ICOMFrameworkTypes.h"


namespace ICOM{ 
 namespace Framework{

	class  TranscoderIBM00500 : public Transcoder256Table
	{
	public :	
	
		
		TranscoderIBM00500();
		virtual ~TranscoderIBM00500();
		


		
	private :
		// -----------------------------------------------------------------------
		//  Unimplemented constructors and operators
		// -----------------------------------------------------------------------		
		TranscoderIBM00500(const TranscoderIBM00500&);
		TranscoderIBM00500& operator=(const TranscoderIBM00500&);

	};

} 
 }

#endif
