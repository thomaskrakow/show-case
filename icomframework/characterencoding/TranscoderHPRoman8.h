
#if !defined(ICOM_FRAMEWORK_TRANSCODERHPROMAN8)
#define ICOM_FRAMEWORK_TRANSCODERHPROMAN8

#include "Transcoder256Table.h"
#include "ICOMFrameworkTypes.h"


namespace ICOM{ 
 namespace Framework{

	class  TranscoderHPRoman8 : public Transcoder256Table
	{
	public :	
	
		
		TranscoderHPRoman8();
		virtual ~TranscoderHPRoman8();
		


		
	private :
		// -----------------------------------------------------------------------
		//  Unimplemented constructors and operators
		// -----------------------------------------------------------------------		
		TranscoderHPRoman8(const TranscoderHPRoman8&);
		TranscoderHPRoman8& operator=(const TranscoderHPRoman8&);

	};

} 
 }

#endif
