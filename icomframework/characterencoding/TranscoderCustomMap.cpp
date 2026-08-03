#include "ICOMFramework.h"
#include "TranscoderCustomMap.h"
#include "TranscoderException.h"
#include "ErrorHandler.h"

namespace ICOM{ 
	namespace Framework{

TranscoderCustomMap::TranscoderCustomMap(const TransMap* UTF32ToGlyphIndex, const TransMap* GlyphIndexToUTF32) :
	fUTF32ToGlyphIndex(UTF32ToGlyphIndex),
	fGlyphIndexToUTF32(GlyphIndexToUTF32)/*,
	fUseReplacementCharGlyphIndexToUTF32(false),
	fUseReplacementCharUTF32ToGlyphIndex(false),
	fReplacementCharGlyphIndexToUTF32(0),
	fReplacementCharUTF32ToGlyphIndex(0)*/
{
}

TranscoderCustomMap::~TranscoderCustomMap()
{
}

int	TranscoderCustomMap::transcodeFrom(const char* inputBytes, int inputLengthInBytes, char* const outputUTF32, int availableOutputLengthInBytes)
{
	/*
	**	Input:	GlyphIndizes des Textes, Pro GlyphIndex 4 Byte
	**	Output:	Text in UTF32 (4 Byte pro Zeichen)
	**	Return:	Länge des Outputs in Byte
	*/

	/* Output-Länge = ganze Zeichen des Inputs */
    int countToDo = 4 * (inputLengthInBytes / 4);

	if (countToDo > availableOutputLengthInBytes)
	{
		ICOMErrorHandlerFatal(ICOMFramework::fgErrorHandler, TranscoderException, ErrorCodes::ERROR_TRANSCODER_CUSTOMMAP_NOT_ENOUGH_SPACE);
	}

	/* Umkodierung */
    const ICF_uint32* srcPtr = (ICF_uint32*)inputBytes;
    const ICF_uint32* endPtr = (ICF_uint32*)(inputBytes + inputLengthInBytes - inputLengthInBytes % 4);
    ICF_uint32* outPtr = (ICF_uint32*)outputUTF32;
	ICF_uint32 nextOut = 0;

	TransMap::const_iterator itMap;

    while (srcPtr < endPtr)
    {
		itMap = fGlyphIndexToUTF32->find(*srcPtr);

		if (itMap != fGlyphIndexToUTF32->end())
		{
			*outPtr = itMap->second;
		}
		else
		{
			/* Character nicht in Map enthalten */
			if (!fUseReplacementCharGlyphIndexToUTF32)
			{
	            ICOMErrorHandlerFatal(ICOMFramework::fgErrorHandler, TranscoderException, ErrorCodes::ERROR_TRANSCODER_CHARACTER_NOT_REPRESENTABLE);
			}
			else
			{
				*outPtr = fReplacementCharGlyphIndexToUTF32;
			}
		}

		srcPtr++;
		outPtr++;
	}

	return countToDo;
}

int	TranscoderCustomMap::transcodeTo(const char* const inputUTF32, int inputLengthInBytes, char* const outputBytes, int availableOutputLengthInBytes)
{
	/*
	 * Input:	Text in UTF32 (4 Byte pro Zeichen)
	 * Output:	GlyphIndizes des Textes, Pro GlyphIndex 4 Byte
	 * Return:	Länge des Outputs in Byte
	 */

	/* Output-Länge = ganze Zeichen des Inputs */
    int countToDo = 4 * (inputLengthInBytes / 4);

	if (countToDo > availableOutputLengthInBytes)
	{
		ICOMErrorHandlerFatal(ICOMFramework::fgErrorHandler, TranscoderException, ErrorCodes::ERROR_TRANSCODER_CUSTOMMAP_NOT_ENOUGH_SPACE);
	}

	/* Umkodierung */
    const ICF_uint32* srcPtr = (ICF_uint32*)inputUTF32;
    const ICF_uint32* endPtr = (ICF_uint32*)(inputUTF32 + inputLengthInBytes - inputLengthInBytes % 4);
    ICF_uint32* outPtr = (ICF_uint32*)outputBytes;
	ICF_uint32 nextOut = 0;

	TransMap::const_iterator itMap;

	while (srcPtr < endPtr)
    {
		itMap = fUTF32ToGlyphIndex->find(*srcPtr);

		if (itMap != fUTF32ToGlyphIndex->end())
		{
			*outPtr = itMap->second;
		}
		else
		{
			/* Character nicht in Map enthalten */
			if (!fUseReplacementCharUTF32ToGlyphIndex)
			{
	            ICOMErrorHandlerFatal(ICOMFramework::fgErrorHandler, TranscoderException, ErrorCodes::ERROR_TRANSCODER_CHARACTER_NOT_REPRESENTABLE);
			}
			else
			{
				*outPtr = fReplacementCharUTF32ToGlyphIndex;
			}
		}

		srcPtr++;
		outPtr++;
	}

	return countToDo;
}

//void TranscoderCustomMap::useReplacementCharUTF32ToGlyphIndex(bool useReplacementUTF32ToGlyphIndex)
//{
//	fUseReplacementCharUTF32ToGlyphIndex = useReplacementUTF32ToGlyphIndex;
//}
//
//void TranscoderCustomMap::useReplacementCharGlyphIndexToUTF32(bool useReplacementGlyphIndexToUTF32)
//{
//	fUseReplacementCharGlyphIndexToUTF32 = useReplacementGlyphIndexToUTF32;
//}
//
//void TranscoderCustomMap::setReplacementCharUTF32ToGlyphIndex(ICF_uint32 replacementCharUTF32ToGlyphIndex)
//{
//	fReplacementCharUTF32ToGlyphIndex = replacementCharUTF32ToGlyphIndex;
//}
//
//void TranscoderCustomMap::setReplacementCharGlyphIndexToUTF32(ICF_uint32 replacementCharGlyphIndexToUTF32)
//{
//	fReplacementCharGlyphIndexToUTF32 = replacementCharGlyphIndexToUTF32;
//}

int TranscoderCustomMap::getMaxBytesPerCharacter()
{
	return 4;
}

int TranscoderCustomMap::getMinBytesPerCharacter()
{
	return 4;
}

int TranscoderCustomMap::getLengthOfNullCharacter()
{
	return 4;
}

}
}
