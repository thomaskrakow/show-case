#if !defined(ICOM_FRAMEWORK_TRANSCODERUSASCII)
#define ICOM_FRAMEWORK_TRANSCODERUSASCII

#include "Transcoder256Table.h"
#include "ICOMFrameworkTypes.h"

namespace ICOM{ 
 namespace Framework{

	class  TranscoderUSASCII : public Transcoder256Table
	{
	public :	
		TranscoderUSASCII();
		virtual ~TranscoderUSASCII();
		
	private :
		// -----------------------------------------------------------------------
		//  Unimplemented constructors and operators
		// -----------------------------------------------------------------------		
		TranscoderUSASCII(const TranscoderUSASCII&);
		TranscoderUSASCII& operator=(const TranscoderUSASCII&);
	};

 } 
}

#endif
