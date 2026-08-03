
#if !defined(ICOM_FRAMEWORK_TRANSCODERSYMBOL)
#define ICOM_FRAMEWORK_TRANSCODERSYMBOL

#include "Transcoder256Table.h"
#include "ICOMFrameworkTypes.h"


namespace ICOM{ 
 namespace Framework{

	class  TranscoderSymbol : public Transcoder256Table
	{
	public :	
	
		
		TranscoderSymbol();
		virtual ~TranscoderSymbol();
		


		
	private :
		// -----------------------------------------------------------------------
		//  Unimplemented constructors and operators
		// -----------------------------------------------------------------------		
		TranscoderSymbol(const TranscoderSymbol&);
		TranscoderSymbol& operator=(const TranscoderSymbol&);

	};

} 
 }

#endif
