
#if !defined(ICOM_FRAMEWORK_TRANSCODINGSERVICE)
#define ICOM_FRAMEWORK_TRANSCODINGSERVICE

#include "XMemory.h"
#include "ICOMFramework.h"
#include "InputStream.h"
#include "OutputStream.h"

namespace ICOM{ 
 namespace Framework{
 
	class Transcoder;

	class  TranscodingService : public XMemory
	{
	public :	
	
		enum Encoding{
			/* Diese Encodings müssen immer die ersten Encodings sein! */
			Encoding_Min,
			Encoding_UTF8 = Encoding_Min,

			Encoding_UTF16_LE,
			Encoding_UTF16_BE,
			Encoding_UTF32_LE,
			Encoding_USASCII,
			Encoding_Windows1252,
			Encoding_CP1252,
			Encoding_PDFWinAnsiEncoding,
			Encoding_IBM00500,			/* EBCDIC International #5 - http://www-01.ibm.com/software/globalization/cp/cp00500.html */
			Encoding_IBM01141,			/* EBCDIC Deutschland */
			Encoding_HPRoman8,  		/* HP Roman 8, Roman-8 in PCL*/
			Encoding_ZapfDingbats,  	/* ZapfDingbats http://unicode.org/Public/MAPPINGS/VENDORS/ADOBE/zdingbat.txt */
			Encoding_Symbol,			/* Symbol ftp://unicode.org/Public/MAPPINGS/VENDORS/ADOBE/symbol.txt */			

			/* Diese Kodierung muss immer die letzte in dieser Liste sein! */
			Encoding_Native,				/* Kodierung nicht bekannt oder String ist bereits in der Zielkodierung (z.b. Kodierung des Fonts) */
			Encoding_Max = Encoding_Native
		};

		TranscodingService(Encoding inputEncoding,Encoding outputEncoding);
		TranscodingService(Transcoder* inputTranscoder,Encoding outputEncoding,bool autoDeleteTranscoder = true);
		TranscodingService(Encoding inputEncoding,Transcoder* outputTranscoder,bool autoDeleteTranscoder = true);
		TranscodingService(Transcoder* inputTranscoder,Transcoder* outputTranscoder,bool autoDeleteTranscoderInput = true,bool autoDeleteTranscoderOutput = true);
		virtual ~TranscodingService();

		int		transcode(const char* input,int inputBytes,char* output,int availableOutputBytes);
		int		transcode(InputStream* input,OutputStream* output);
		char*	transcode(const char* input, int inputBytes, int* outputBytes = 0);

		virtual void useReplacementCharUTF32ToGlyphIndex(bool useReplacementUTF32ToGlyphIndex);
		virtual void useReplacementCharGlyphIndexToUTF32(bool useReplacementGlyphIndexToUTF32);
		virtual void setReplacementCharUTF32ToGlyphIndex(ICF_uint32 replacementCharUTF32ToGlyphIndex);
		virtual void setReplacementCharGlyphIndexToUTF32(ICF_uint32 replacementCharGlyphIndexToUTF32);

		/*
		 * Diese Funktionen versuchen das Encoding einer Datei bzw. von Binï¿½rdaten anhand des BOM zu
		 * bestimmen. Wurde kein bekanntes BOM Muster gefunden, so liefert diese Funktion Encoding_Native
		 * zurï¿½ck. Die Lï¿½nge des BOM wird in der Variablen bomlength abgelegt.
		 */
		static Encoding	getEncondingFromBOM(const char* fileName,ICF_uint8& bomlength);
		static Encoding	getEncondingFromBOM(InputStream* input,ICF_uint8& bomlength);
		static Encoding	getEncondingFromBOM(const char* data,ICF_uint64 length,ICF_uint8& bomlength);

		/*
		 * Liefert den Namen des Encoding durch einen 0-terminierten String zurï¿½ck.
		 */
		static const char*	getNameFromEncoding(Encoding encoding);

		/*
		 * Liefert das Encoding anhand des Namens zurï¿½ck. Ist das Encoding nicht bekannt, dann wird Encoding_Native
		 * zurï¿½ckgeliefert.
		 */
		static Encoding		getEncodingFromName(const char* encodingName);
		
	protected:
	

	private :

		Transcoder*		createTranscoder(Encoding encoding);


		// -----------------------------------------------------------------------
		//  Unimplemented constructors and operators
		// -----------------------------------------------------------------------
		TranscodingService(const TranscodingService&);
		TranscodingService& operator=(const TranscodingService&);

		Transcoder*	fInputTranscoder;
		bool		fAutoDeleteInputTranscoder;

		Transcoder*	fOutputTranscoder;
		bool		fAutoDeleteOutputTranscoder;			
		

	};

} 
 }

#endif
