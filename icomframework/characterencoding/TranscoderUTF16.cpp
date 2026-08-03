
#include "ICOMFramework.h"
#include "TranscoderUTF16.h"
#include "TranscoderException.h"
#include "ErrorHandler.h"
#include "ICOMFrameworkTypes.h"

#include <string.h>

namespace ICOM{ 
 namespace Framework{


TranscoderUTF16::TranscoderUTF16(bool bigendian) :
	fBigEndian(bigendian)
{
}


TranscoderUTF16::~TranscoderUTF16()
{
}

int TranscoderUTF16::transcodeFrom(const char* inputBytes,const int inputLengthInBytes,char* const outputUTF32,const int availableOutputLengthInBytes)
{
    //
    //  Calculate the max chars we can do here. Its the lesser of the
    //  max output chars and the number of chars in the source.
    //
    const int srcChars = inputLengthInBytes / sizeof(ICF_uint16);
    const int countToDo = srcChars < inputLengthInBytes ? srcChars : inputLengthInBytes;

    // Look at the source data as UTF16 chars
    const ICF_uint16* asUTF16 = (const ICF_uint16*)inputBytes;

    // And get a mutable pointer to the output
    ICF_uint32* outPtr = (ICF_uint32*)outputUTF32;

    //
    //  If its swapped, we have to do a char by char swap and cast. Else
    //  we have to check whether our char and ICF_uint16 types are the same
    //  size or not. If so, we can optimize by just doing a buffer copy.
    //
    if (fBigEndian)
    {
        //
        //  And then do the swapping loop for the count we precalculated. Note
        //  that this also handles size conversion as well if char is not the
        //  same size as ICF_uint16.
        //
        for (int index = 0; index < countToDo; index++){
			*outPtr++ = ICF_uint16(((*asUTF16 >> 8) | (*asUTF16 << 8)) & 0xFFFF);
            asUTF16++;
		}
    }
     else
    {
        //
        //  If the char type is the same size as a UTF16 value on this
        //  platform, then we can do just a buffer copy straight to the target
        //  buffer since our source chars are UTF-16 chars. If its not, then
        //  we still have to do a loop and assign each one, in order to
        //  implicitly convert.
        //
        if (sizeof(ICF_uint32) == sizeof(ICF_uint16))
        {
            //  Notice we convert char count to byte count here!!!
            memcpy(outputUTF32, inputBytes, countToDo * sizeof(ICF_uint16));
        }
         else
        {
            for (int index = 0; index < countToDo; index++)
                *outPtr++ = ICF_uint32(*asUTF16++);
        }
    }

    // Return the chars we transcoded
    return countToDo * sizeof(ICF_uint16);;
}


int TranscoderUTF16::transcodeTo(const char* const inputBytes,const int inputLengthInBytes,char* const  toFill,const int availableOutputLengthInBytes)
{
    //
    //  Calculate the max chars we can do here. Its the lesser of the
    //  chars that we can fit into the output buffer, and the source
    //  chars available.
    //
    const int maxOutChars = availableOutputLengthInBytes / sizeof(ICF_uint16);
    const int countToDo = inputLengthInBytes < maxOutChars ? inputLengthInBytes : maxOutChars;

    //
    //  Get a pointer tot he output buffer in the UTF-16 character format
    //  that we need to work with. And get a mutable pointer to the source
    //  character buffer.
    //
    ICF_uint16*        outPtr = (ICF_uint16*)toFill;
    const ICF_uint32*    srcPtr = (ICF_uint32*)inputBytes;

    //
    //  If the target format is swapped from our native format, then handle
    //  it one way, else handle it another.
    //
    if (fBigEndian)
    {
        //
        //  And then do the swapping loop for the count we precalculated. Note
        //  that this also handles size conversion as well if char is not the
        //  same size as ICF_uint16.
        //
		for (int index = 0; index < countToDo; index++){
			const ICF_uint16 tmpCh = ICF_uint16(*srcPtr++);
			*outPtr++ = ICF_uint16(((tmpCh >> 8) | (tmpCh << 8)) & 0xFFFF);
		}
    }
     else
    {
        //
        //  If char and ICF_uint16 are the same size, we can just do a fast
        //  memory copy. Otherwise, we have to do a loop and downcast each
        //  character into its new 16 bit storage.
        //
        if (sizeof(ICF_uint32) == sizeof(ICF_uint16))
        {
            //  Notice we convert char count to byte count here!!!
            memcpy(toFill, inputBytes, countToDo * sizeof(ICF_uint16));
        }
         else
        {
            for (int index = 0; index < countToDo; index++)
                *outPtr++ = ICF_uint16(*srcPtr++);
        }
    }

    //Return the bytes we ate. Note we convert to a byte count here!
    return countToDo;
}


bool TranscoderUTF16::canTranscodeTo(const unsigned int)
{
    // We can handle anything
    return true;
}

int TranscoderUTF16::getMaxBytesPerCharacter()
{
	return 4;
}

int TranscoderUTF16::getMinBytesPerCharacter()
{
	return 2;
}

int TranscoderUTF16::getLengthOfNullCharacter()
{
	return 2;
}

}
}

