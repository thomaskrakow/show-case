


#include "ICOMFramework.h"
#include "TranscodingService.h"
#include "ICOMMacros.h"
#include "TranscoderCP1252.h"
#include "TranscoderIBM00500.h"
#include "TranscoderIBM01141.h"
#include "TranscoderUTF8.h"
#include "TranscoderUTF16.h"
#include "TranscoderUSASCII.h"
#include "TranscoderHPRoman8.h"
#include "TranscoderZapfDingbats.h"
#include "TranscoderSymbol.h"
#include "TranscoderException.h"
#include "ErrorHandler.h"
#include "ICOMFrameworkTypes.h"
#include "Janitor.h"
#include "FileInputStream.h"

#include <assert.h>
#include <string.h>

#define BUFFERSIZE 1024

namespace ICOM{ 
 namespace Framework{

	 static const char* const fgNamesEncoding[] = {
			"UTF8",
			"UTF16LE",
			"UTF16BE",
			"UTF32LE",
			"US-ASCII",
			"Windows 1252",
			"Windows 1252",
			"WinAnsi Encoding (PDF)",
			"EBCDIC International #5",
			"EBCDIC Deutschland",
			"HP Roman 8 (PCL)",
			"ZapfDingbats",
			"Symbol",
			""
		};




TranscodingService::TranscodingService(Encoding inputEncoding,Encoding outputEncoding) :
	fAutoDeleteInputTranscoder(true),
	fAutoDeleteOutputTranscoder(true)
{
	fInputTranscoder	= this->createTranscoder( inputEncoding );
	fOutputTranscoder	= this->createTranscoder( outputEncoding );
}

TranscodingService::TranscodingService(Transcoder* inputTranscoder,Encoding outputEncoding,bool autoDeleteTranscoder) :
	fInputTranscoder(inputTranscoder),
	fAutoDeleteInputTranscoder(autoDeleteTranscoder),
	fAutoDeleteOutputTranscoder(true)
{
	fOutputTranscoder	= this->createTranscoder( outputEncoding );
}

TranscodingService::TranscodingService(Encoding inputEncoding,Transcoder* outputTranscoder,bool autoDeleteTranscoder) :
	fOutputTranscoder(outputTranscoder),
	fAutoDeleteInputTranscoder(true),
	fAutoDeleteOutputTranscoder(autoDeleteTranscoder)
{
	fInputTranscoder	= this->createTranscoder( inputEncoding );
}

TranscodingService::TranscodingService(Transcoder* inputTranscoder,Transcoder* outputTranscoder,bool autoDeleteTranscoderInput,bool autoDeleteTranscoderOutput) :
	fInputTranscoder(inputTranscoder),
	fOutputTranscoder(outputTranscoder),
	fAutoDeleteInputTranscoder(autoDeleteTranscoderInput),
	fAutoDeleteOutputTranscoder(autoDeleteTranscoderOutput)
{

}


TranscodingService::~TranscodingService()
{
	if( fAutoDeleteInputTranscoder ){
		ICOM_DELETE_VARIABLE( fInputTranscoder );
	}

	if( fAutoDeleteOutputTranscoder ){
		ICOM_DELETE_VARIABLE( fOutputTranscoder );
	}

}

int		TranscodingService::transcode(const char* input,int inputBytes,char* output,int availableOutputBytes)
{
	ICF_uint32	utf32[BUFFERSIZE] = {0};
	ICF_uint32*  utf32Tmp = utf32;

	int	bytesEncoded = 0;
	int bytesWritten = 0;
	int	bytesInUTF32 = 0;

	while( bytesEncoded < inputBytes ){

		int toEncode = inputBytes - bytesEncoded > BUFFERSIZE ? BUFFERSIZE : inputBytes - bytesEncoded;
		if( fInputTranscoder ){
			bytesInUTF32 = fInputTranscoder->transcodeFrom( input + bytesEncoded,toEncode, (char*)utf32Tmp, BUFFERSIZE * sizeof(ICF_uint32) );
			bytesEncoded += toEncode;
		}else{
			utf32Tmp = (ICF_uint32*)input;
			bytesInUTF32 = inputBytes;
			bytesEncoded = inputBytes;
		}

		if( fOutputTranscoder ){
			bytesWritten += fOutputTranscoder->transcodeTo( (char*)utf32Tmp, bytesInUTF32, output + bytesWritten, availableOutputBytes - bytesWritten );
		}else{
			/* TM - 27.11.2014 - UPD - war vertauscht */
			/* memcpy( utf32Tmp, output + bytesWritten,bytesInUTF32); */
			memcpy(output + bytesWritten, utf32Tmp, bytesInUTF32);
			/* TM - 27.11.2014 - END */
			bytesWritten += bytesInUTF32;
		}
	}
	return bytesWritten;
}


Transcoder*		TranscodingService::createTranscoder(Encoding encoding)
{
	switch(encoding){
	case Encoding_UTF8:
		return new TranscoderUTF8();
	case Encoding_UTF16_LE:
		return new TranscoderUTF16(false);
	case Encoding_UTF16_BE:
		return new TranscoderUTF16(true);
	case Encoding_USASCII:
		return new TranscoderUSASCII();
	case Encoding_Windows1252:
	case Encoding_CP1252:
	case Encoding_PDFWinAnsiEncoding:
		return new TranscoderCP1252();
	case Encoding_IBM00500:
		return new TranscoderIBM00500();
	case Encoding_IBM01141:
		return new TranscoderIBM01141();
	case Encoding_HPRoman8:
		return new TranscoderHPRoman8();
	case Encoding_ZapfDingbats:
		return new TranscoderZapfDingbats();
	case Encoding_Symbol:
		return new TranscoderSymbol();
	case Encoding_UTF32_LE:
		return 0;
	default:
		ICOMErrorHandlerFatal(
			ICOMFramework::fgErrorHandler,
			TranscoderException,
			ErrorCodes::ERROR_TRANSCODINGSERVICE_UNKNOWN_ENCODING
		);
		break;
	}
	return 0;
}

int		TranscodingService::transcode(InputStream* input,OutputStream* output)
{
	/*
	 * Soviel Speicher allozieren ist zwar Mist, aber aus Schnelligkeitsgr�nden nicht anders machbar.
	 * Evtl. programmiert das jemand mal anders.
	 */
	char* inputBuffer = (char*)ICOMFramework::fgMemoryManager->allocate(input->getLength());
	Janitor<char> jan1(inputBuffer);
	ICF_uint32* utf32 = (ICF_uint32*)ICOMFramework::fgMemoryManager->allocate(input->getLength()*sizeof(ICF_uint32));
	Janitor<ICF_uint32> jan2(utf32);
	char* outputBuffer = (char*)ICOMFramework::fgMemoryManager->allocate(input->getLength()*sizeof(ICF_uint32));
	Janitor<char> jan3(outputBuffer);

	unsigned long bytesRead = input->read(inputBuffer,input->getLength());
	int bytesInUTF32 = fInputTranscoder->transcodeFrom(inputBuffer,bytesRead,(char*)utf32,input->getLength()*sizeof(ICF_uint32));
	int bytesWritten = fOutputTranscoder->transcodeTo((char*)utf32,bytesInUTF32,outputBuffer,input->getLength()*sizeof(ICF_uint32));
	output->write(outputBuffer,bytesWritten);

	return bytesWritten;
}

char* TranscodingService::transcode(const char* input, int inputBytes, int* outputBytes)
{
	int maxInputCharacters = inputBytes / fInputTranscoder->getMinBytesPerCharacter();	/* abgerundeter Wert ist gro� genug */
	int availableUTF32Bytes = maxInputCharacters * sizeof(ICF_uint32);
	ICF_uint32* utf32 = (ICF_uint32*)ICOMFramework::fgMemoryManager->allocate(availableUTF32Bytes);
	Janitor<ICF_uint32> janUTF32(utf32);
	int utf32Bytes = 0;

	if (fInputTranscoder)
	{
		utf32Bytes = fInputTranscoder->transcodeFrom(input, inputBytes, (char*)utf32, availableUTF32Bytes);
	}
	else
	{
		utf32Bytes = inputBytes;
		utf32 = (ICF_uint32*)input;
	}

	int numUTF32Characters = utf32Bytes / sizeof(ICF_uint32);
	int availableOutputBytes = (numUTF32Characters + 1) * fOutputTranscoder->getMaxBytesPerCharacter();	/* +1 f�r Null-Character */
	char* output = (char*)ICOMFramework::fgMemoryManager->allocate(availableOutputBytes);
	memset(output, 0, availableOutputBytes);
	int outBytes = 0;

	if (fOutputTranscoder)
	{
		outBytes = fOutputTranscoder->transcodeTo((char*)utf32, utf32Bytes, output, availableOutputBytes);
	}
	else
	{
		outBytes = utf32Bytes;
		memcpy(output, utf32, utf32Bytes);
	}

	if (outputBytes)
	{
		*outputBytes = outBytes;
	}

	return output;
}

void TranscodingService::useReplacementCharUTF32ToGlyphIndex(bool useReplacementUTF32ToGlyphIndex)
{
	if( fInputTranscoder ){
		fInputTranscoder->useReplacementCharUTF32ToGlyphIndex(useReplacementUTF32ToGlyphIndex);
	}

	if( fOutputTranscoder ){
		fOutputTranscoder->useReplacementCharUTF32ToGlyphIndex(useReplacementUTF32ToGlyphIndex);
	}
}


void TranscodingService::useReplacementCharGlyphIndexToUTF32(bool useReplacementGlyphIndexToUTF32)
{
	if( fInputTranscoder ){
		fInputTranscoder->useReplacementCharGlyphIndexToUTF32(useReplacementGlyphIndexToUTF32);
	}

	if( fOutputTranscoder ){
		fOutputTranscoder->useReplacementCharGlyphIndexToUTF32(useReplacementGlyphIndexToUTF32);
	}
}


void TranscodingService::setReplacementCharUTF32ToGlyphIndex(ICF_uint32 replacementCharUTF32ToGlyphIndex)
{
	if( fInputTranscoder ){
		fInputTranscoder->setReplacementCharUTF32ToGlyphIndex(replacementCharUTF32ToGlyphIndex);
	}

	if( fOutputTranscoder ){
		fOutputTranscoder->setReplacementCharUTF32ToGlyphIndex(replacementCharUTF32ToGlyphIndex);
	}
}


void TranscodingService::setReplacementCharGlyphIndexToUTF32(ICF_uint32 replacementCharGlyphIndexToUTF32)
{
	if( fInputTranscoder ){
		fInputTranscoder->setReplacementCharGlyphIndexToUTF32(replacementCharGlyphIndexToUTF32);
	}

	if( fOutputTranscoder ){
		fOutputTranscoder->setReplacementCharGlyphIndexToUTF32(replacementCharGlyphIndexToUTF32);
	}
}

TranscodingService::Encoding	TranscodingService::getEncondingFromBOM(const char* fileName,ICF_uint8& bomlength)
{
	FileInputStream	input(fileName);
	input.setPosition(0);
	return TranscodingService::getEncondingFromBOM(&input,bomlength);	
}

TranscodingService::Encoding	TranscodingService::getEncondingFromBOM(InputStream* input,ICF_uint8& bomlength)
{
	assert(input);
	char	bom[16] = {0};
	unsigned long len = input->read(bom,16*sizeof(char));
	return TranscodingService::getEncondingFromBOM(bom,len,bomlength);	
}

TranscodingService::Encoding	TranscodingService::getEncondingFromBOM(const char* data,ICF_uint64 length,ICF_uint8& bomlength)
{
	if( length >= 4 && 0 == memcmp(data,"\xFF\xFE\x00\x00",4) ){
		bomlength = 4;
		return Encoding_UTF32_LE;
	}

	if( length >= 3 && 0 == memcmp(data,"\xEF\xBB\xBF",3) ){
		bomlength = 3;
		return Encoding_UTF8;
	}

	if( length >= 2 && 0 == memcmp(data,"\xFF\xFE",2) ){
		bomlength = 2;
		return Encoding_UTF16_LE;
	}

	if( length >= 2 && 0 == memcmp(data,"\xFE\xFF",2) ){
		bomlength = 2;
		return Encoding_UTF16_BE;
	}	

	return Encoding_Native;
}

const char*	TranscodingService::getNameFromEncoding(TranscodingService::Encoding encoding)
{
	assert(Encoding_Min <= encoding && encoding <= Encoding_Max );
	return fgNamesEncoding[encoding];
}

TranscodingService::Encoding		TranscodingService::getEncodingFromName(const char* encodingName)
{
	for( TranscodingService::Encoding encoding = Encoding_Min;encoding <= Encoding_Max;){
		if( 0 == strcmp(encodingName,fgNamesEncoding[encoding] ) ){
			return encoding;
		}
		encoding = (TranscodingService::Encoding)( (int)encoding + 1 );
	}
	return Encoding_Max;
}

}
}
