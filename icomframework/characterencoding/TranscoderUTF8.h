
#if !defined(ICOM_FRAMEWORK_TRANSCODERUTF8)
#define ICOM_FRAMEWORK_TRANSCODERUTF8

#include "Transcoder.h"


namespace ICOM{ 
 namespace Framework{

	class  TranscoderUTF8 : public Transcoder
	{
	public :	
		TranscoderUTF8();
		virtual ~TranscoderUTF8();
		
		virtual int		transcodeFrom(const char* inputBytes,int inputLengthInBytes,char* const outputUTF32,int availableOutputLengthInBytes);
		virtual int		transcodeTo(const char* const inputUTF32,int inputLengthInBytes,char* const outputBytes,int availableOutputLengthInBytes);
		
		virtual int	getMaxBytesPerCharacter();
		virtual int	getMinBytesPerCharacter();
		virtual int getLengthOfNullCharacter();

	protected:
	
		void 			checkTrailingBytes(const char      toCheck, const unsigned int trailingBytes, const unsigned int position) const;

	private :
		// -----------------------------------------------------------------------
		//  Unimplemented constructors and operators
		// -----------------------------------------------------------------------
		TranscoderUTF8(const TranscoderUTF8&);
		TranscoderUTF8& operator=(const TranscoderUTF8&);



	};

} 
 }

#endif
