
#if !defined(ICOM_FRAMEWORK_TRANSCODERIBM01141)
#define ICOM_FRAMEWORK_TRANSCODERIBM01141

#include "Transcoder256Table.h"
#include "ICOMFrameworkTypes.h"


namespace ICOM{ 
 namespace Framework{

	class  TranscoderIBM01141 : public Transcoder256Table
	{
	public :	
	
		
		TranscoderIBM01141();
		virtual ~TranscoderIBM01141();
		


		
	private :
		// -----------------------------------------------------------------------
		//  Unimplemented constructors and operators
		// -----------------------------------------------------------------------		
		TranscoderIBM01141(const TranscoderIBM01141&);
		TranscoderIBM01141& operator=(const TranscoderIBM01141&);

	};

} 
 }

#endif
