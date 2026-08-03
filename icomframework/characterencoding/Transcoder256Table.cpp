


#include "ICOMFramework.h"
#include "Transcoder256Table.h"
#include "TranscoderException.h"
#include "ErrorHandler.h"

namespace ICOM{ 
 namespace Framework{
 
 

Transcoder256Table::Transcoder256Table
		(
			const ICF_uint32* const                        fromTable
			, const Transcoder256Table::Transcoder256TableRecord* const    toTable
			, const int                           toTableSize
		) :
		fFromTable(fromTable),
		fToSize(toTableSize),
		fToTable(toTable)
{
}


Transcoder256Table::~Transcoder256Table()
{

}


int	Transcoder256Table::transcodeFrom(const char* inputBytes,int inputLengthInBytes,char* const outputUTF32,int availableOutputLengthInBytes)
{
    //
    //  Calculate the max chars we can do here. Its the lesser of the
    //  max output chars and the number of chars in the source.
    //
    int countToDo = inputLengthInBytes * 4;
	if( countToDo  > availableOutputLengthInBytes ){
		 ICOMErrorHandlerFatal(
			ICOMFramework::fgErrorHandler,
			TranscoderException,
			ErrorCodes::ERROR_TRANSCODER256_NOT_ENOUGH_SPACE
		);
	}

    //
    //  Loop through the count we have to do and map each char via the
    //  lookup table.
    //
    const char*  srcPtr = inputBytes;
    const char*  endPtr = (inputBytes + inputLengthInBytes);
    ICF_uint32*          outPtr = (ICF_uint32*)outputUTF32;
    while (srcPtr < endPtr)
    {
        ICF_uint32 uniCh = fFromTable[*srcPtr++];
        if (uniCh != 0xFFFF)
        {
            *outPtr++ = uniCh;
            continue;
        }
    }

    // Return the chars we transcoded
    return countToDo;
}

int	Transcoder256Table::transcodeTo(const char* const inputUTF32,int inputLengthInBytes,char* const outputBytes,int availableOutputLengthInBytes)
{
	//
    //  Calculate the max chars we can do here. Its the lesser of the
    //  max output chars and the number of chars in the source.
    //
    int countToDo = inputLengthInBytes / 4 ;
	if( countToDo  > availableOutputLengthInBytes ){
		 ICOMErrorHandlerFatal(
			ICOMFramework::fgErrorHandler,
			TranscoderException,
			ErrorCodes::ERROR_TRANSCODER256_NOT_ENOUGH_SPACE
		);
	}

    //
    //  Loop through the count we have to do and map each char via the
    //  lookup table.
    //
    const ICF_uint32*    srcPtr = (ICF_uint32*)inputUTF32;
    const ICF_uint32*    endPtr = (ICF_uint32*)(inputUTF32 + inputLengthInBytes - inputLengthInBytes % 4);
    char*        outPtr = outputBytes;
    char         nextOut;
    while (srcPtr < endPtr)
    {
        //
        //  Get the next src char out to a temp, then do a binary search
        //  of the 'to' table for this entry.
        //
        if ((nextOut = xlatOneTo(*srcPtr))!=0)
        {
            *outPtr++ = nextOut;
            srcPtr++;
            continue;
        }

        //
        //  Its not representable so, according to the options, either
        //  throw or use the replacement.
        //
        if ( !fUseReplacementCharUTF32ToGlyphIndex )
        {
            ICOMErrorHandlerFatal(
				ICOMFramework::fgErrorHandler,
				TranscoderException,
				ErrorCodes::ERROR_TRANSCODER_CHARACTER_NOT_REPRESENTABLE
			);
        }

        // Eat the source char and use the replacement char
        srcPtr++;
        *outPtr++ = fReplacementCharUTF32ToGlyphIndex;
    }

    // Return the bytes we transcoded
    return countToDo;
}

char Transcoder256Table::xlatOneTo(const ICF_uint32 toXlat) const
{
    int lowOfs = 0;
    int hiOfs = fToSize - 1;
    do
    {
        // Calc the mid point of the low and high offset.
        const int midOfs = ((hiOfs - lowOfs) / 2) + lowOfs;

        //
        //  If our test char is greater than the mid point char, then
        //  we move up to the upper half. Else we move to the lower
        //  half. If its equal, then its our guy.
        //
        if (toXlat > fToTable[midOfs].intCh)
        {
            lowOfs = midOfs;
        }
         else if (toXlat < fToTable[midOfs].intCh)
        {
            hiOfs = midOfs;
        }
         else
        {
            return fToTable[midOfs].extCh;
        }
    }   while (lowOfs + 1 < hiOfs);

    // Check the high end of the range otherwise the
    // last item in the table may never be found.
        if (toXlat == fToTable[hiOfs].intCh)
        {
            return fToTable[hiOfs].extCh;
        }

    return 0;
}

int Transcoder256Table::getMaxBytesPerCharacter()
{
	return 1;
}

int Transcoder256Table::getMinBytesPerCharacter()
{
	return 1;
}

int Transcoder256Table::getLengthOfNullCharacter()
{
	return 1;
}

}
}
