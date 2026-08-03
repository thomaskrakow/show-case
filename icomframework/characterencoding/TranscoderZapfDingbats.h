
#if !defined(ICOM_FRAMEWORK_TRANSCODERZAPFDINGBATS)
#define ICOM_FRAMEWORK_TRANSCODERZAPFDINGBATS

#include "Transcoder256Table.h"
#include "ICOMFrameworkTypes.h"


namespace ICOM{ 
 namespace Framework{

	class  TranscoderZapfDingbats : public Transcoder256Table
	{
	public :	
	
		
		TranscoderZapfDingbats();
		virtual ~TranscoderZapfDingbats();
		


		
	private :
		// -----------------------------------------------------------------------
		//  Unimplemented constructors and operators
		// -----------------------------------------------------------------------		
		TranscoderZapfDingbats(const TranscoderZapfDingbats&);
		TranscoderZapfDingbats& operator=(const TranscoderZapfDingbats&);

	};

} 
 }

#endif
