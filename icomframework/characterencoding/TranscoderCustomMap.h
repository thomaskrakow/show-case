#if !defined(ICOM_FRAMEWORK_TRANSCODERCUSTOMMAP)
#define ICOM_FRAMEWORK_TRANSCODERCUSTOMMAP

#include "Transcoder.h"
#include "ICOMFrameworkTypes.h"

#include <map>

namespace ICOM{ 
	namespace Framework{

		class  TranscoderCustomMap : public Transcoder
		{
		public :	
			typedef std::map<ICF_uint32, ICF_uint32> TransMap;

			TranscoderCustomMap(const TransMap* UTF32ToGlyphIndex, const TransMap* GlyphIndexToUTF32);
			virtual ~TranscoderCustomMap();

			virtual int	transcodeFrom(const char* inputBytes, int inputLengthInBytes, char* const outputUTF32, int availableOutputLengthInBytes);
			virtual int	transcodeTo(const char* const inputUTF32, int inputLengthInBytes, char* const outputBytes, int availableOutputLengthInBytes);

			/*void useReplacementCharUTF32ToGlyphIndex(bool useReplacementUTF32ToGlyphIndex);
			void useReplacementCharGlyphIndexToUTF32(bool useReplacementGlyphIndexToUTF32);
			void setReplacementCharUTF32ToGlyphIndex(ICF_uint32 replacementCharUTF32ToGlyphIndex);
			void setReplacementCharGlyphIndexToUTF32(ICF_uint32 replacementCharGlyphIndexToUTF32);*/

			virtual int	getMaxBytesPerCharacter();
			virtual int	getMinBytesPerCharacter();
			virtual int getLengthOfNullCharacter();

		protected:
		
		private :
			// -----------------------------------------------------------------------
			//  Unimplemented constructors and operators
			// -----------------------------------------------------------------------
			TranscoderCustomMap(const TranscoderCustomMap&);
			TranscoderCustomMap& operator=(const TranscoderCustomMap&);
		
			const TransMap*	fUTF32ToGlyphIndex;
			const TransMap*	fGlyphIndexToUTF32;
			/*bool fUseReplacementCharUTF32ToGlyphIndex;
			bool fUseReplacementCharGlyphIndexToUTF32;
			ICF_uint32 fReplacementCharUTF32ToGlyphIndex;
			ICF_uint32 fReplacementCharGlyphIndexToUTF32;*/
		};
	} 
}

#endif /*ICOM_FRAMEWORK_TRANSCODERCUSTOMMAP*/
