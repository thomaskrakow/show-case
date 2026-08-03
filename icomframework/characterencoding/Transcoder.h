
#if !defined(ICOM_FRAMEWORK_TRANSCODER)
#define ICOM_FRAMEWORK_TRANSCODER

#include "XMemory.h"
#include "ICOMFramework.h"

namespace ICOM{ 
 namespace Framework{

	class  Transcoder : public XMemory
	{
	public :	

		virtual ~Transcoder();

		virtual void useReplacementCharUTF32ToGlyphIndex(bool useReplacementUTF32ToGlyphIndex);
		virtual void useReplacementCharGlyphIndexToUTF32(bool useReplacementGlyphIndexToUTF32);
		virtual void setReplacementCharUTF32ToGlyphIndex(ICF_uint32 replacementCharUTF32ToGlyphIndex);
		virtual void setReplacementCharGlyphIndexToUTF32(ICF_uint32 replacementCharGlyphIndexToUTF32);
		
		virtual int	transcodeFrom(const char* inputBytes,int inputLengthInBytes,char* const outputUTF32,int availableOutputLengthInBytes) = 0;
		virtual int	transcodeTo(const char* const inputUTF32,int inputLengthInBytes,char* const outputBytes,int availableOutputLengthInBytes) = 0;

		virtual int	getMaxBytesPerCharacter() = 0;
		virtual int	getMinBytesPerCharacter() = 0;
		virtual int getLengthOfNullCharacter() = 0;
		
	protected:
	
		Transcoder();

		//bool		fUseReplaementChar;
		//char		fReplacementChar;

		bool		fUseReplacementCharUTF32ToGlyphIndex;
		bool		fUseReplacementCharGlyphIndexToUTF32;
		ICF_uint32	fReplacementCharUTF32ToGlyphIndex;
		ICF_uint32	fReplacementCharGlyphIndexToUTF32;

	private :
		// -----------------------------------------------------------------------
		//  Unimplemented constructors and operators
		// -----------------------------------------------------------------------
		Transcoder(const Transcoder&);
		Transcoder& operator=(const Transcoder&);

		



	};

} 
 }

#endif
