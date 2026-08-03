


#include "ICOMFramework.h"
#include "Transcoder.h"

using namespace ICOM::Framework;


Transcoder::Transcoder() :
	fUseReplacementCharGlyphIndexToUTF32(false),
	fUseReplacementCharUTF32ToGlyphIndex(false),
	fReplacementCharGlyphIndexToUTF32(0),
	fReplacementCharUTF32ToGlyphIndex(0)
{

}


Transcoder::~Transcoder()
{

}

void Transcoder::useReplacementCharUTF32ToGlyphIndex(bool useReplacementUTF32ToGlyphIndex)
{
	fUseReplacementCharUTF32ToGlyphIndex = useReplacementUTF32ToGlyphIndex;
}

void Transcoder::useReplacementCharGlyphIndexToUTF32(bool useReplacementGlyphIndexToUTF32)
{
	fUseReplacementCharGlyphIndexToUTF32 = useReplacementGlyphIndexToUTF32;
}

void Transcoder::setReplacementCharUTF32ToGlyphIndex(ICF_uint32 replacementCharUTF32ToGlyphIndex)
{
	fReplacementCharUTF32ToGlyphIndex = replacementCharUTF32ToGlyphIndex;
}

void Transcoder::setReplacementCharGlyphIndexToUTF32(ICF_uint32 replacementCharGlyphIndexToUTF32)
{
	fReplacementCharGlyphIndexToUTF32 = replacementCharGlyphIndexToUTF32;
}

