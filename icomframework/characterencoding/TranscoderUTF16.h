

#if !defined(ICOM_FRAMEWORK_TRANSCODERUTF16)
#define ICOM_FRAMEWORK_TRANSCODERUTF16

#include "Transcoder.h"

namespace ICOM{ 
 namespace Framework{


class TranscoderUTF16 : public Transcoder
{
public :
    TranscoderUTF16(bool bigendian = false);
    virtual ~TranscoderUTF16();


    virtual int transcodeFrom(const char* inputBytes,int inputLengthInBytes,char* const outputUTF32,int availableOutputLengthInBytes);
    virtual int transcodeTo(const char* const inputBytes,int inputLengthInBytes,char* const  toFill,int availableOutputLengthInBytes);
	
	virtual int	getMaxBytesPerCharacter();
	virtual int	getMinBytesPerCharacter();
	virtual int getLengthOfNullCharacter();

protected:

    virtual bool canTranscodeTo(unsigned int  toCheck);


private :

    TranscoderUTF16(const TranscoderUTF16&);
    TranscoderUTF16& operator=(const TranscoderUTF16&);


    bool    fBigEndian;
};

	}
}

#endif
