/* --------------------------------------------------------------
*	Revision:		$Rev: 14974 $
*
*	zuletzt ge�ndert am: 	$Date: 2010-10-04 12:45:04 +0200 (Mo, 04 Okt 2010) $
*
*	zueltzt ge�ndert von:	$Author: thomaskrakow $
*
*	Subversion Speicherort:	$HeadURL: http://svn.icomsoftware.dortmund/svn/source/pdflib/trunk/pdflib/src/src/Font.cpp $
*
* -------------------------------------------------------------- */

/*!		@page		Font.cpp Versionsdokumentation f�r Font.cpp
 *
 *		@section	Versionsdaten
 *		@verbatim	$HeadURL: http://svn.icomsoftware.dortmund/svn/source/pdflib/trunk/pdflib/src/src/Font.cpp $	@endverbatim
 *		@version	$Rev: 14974 $
 *		@date		$Date: 2010-10-04 12:45:04 +0200 (Mo, 04 Okt 2010) $
 *		@author		$Author: thomaskrakow $
 *
 *		@section	Anmerkungen
 */
/* --------------------------------------------------------------*/

#include "font/Font.h"

#include "utility/utility.h"

#include "stream/MemoryStream.h"
#include "filter/StreamFilterD.h"
#include "font/FontException.h"
using namespace ICOM::pdflib;

#include <memory>

#define new PDFLIB_ALLOCATOR

Font::Font(const Object* _obj_,pdflong objNum,pdflong genNum)
	: fFontDescriptorAdopted(false),
	fCmap(0),
	fHasDescendantFonts(false)
{
	encoding = NULL;

	firstChar = -1;
	lastChar = -1;
	fontDescriptor = NULL;

	fontWeight	= 0;
	ascent		= 0;
	descent		= 0;
	leading     = 0;
	capHeight	= 0;
	xHeight		= 0;
	stemV		= 0;
	stemH		= 0;
	avgWidth	= 0;
	maxWidth	= 0;
	missingWidth= 0;
	italicAngle = 0;
	standardFont= false;
	memset((void*)&fontFile, 0,sizeof(pdfulong));
	memset((void*)&fontFile2,0,sizeof(pdfulong));
	memset((void*)&fontFile3,0,sizeof(pdfulong));
	memset(fontBBox,0,4*sizeof(double));
	fontProgram =new FontProgram();

	inObj.objectNumber = objNum;
	inObj.generationNumber = genNum;
	inObj.oldObjectNumber = 0;

	if(!testObject(_obj_))
		throw internalError(__FILE__,__LINE__,"Objekt ist keine Schrift");

	parse(_obj_);
}

Font::~Font()
{
	if(encoding)
		delete encoding;

	if(fontDescriptor && !fFontDescriptorAdopted)
		delete fontDescriptor;

	if(fontProgram)
		delete fontProgram;

	if( fCmap ){
		delete fCmap;
		fCmap = 0;
	}

}

bool	Font::testObject(const Object* _obj_)
{
	if(_obj_==NULL)
		throw internalError(__FILE__,__LINE__,"Nullobjekt empfangen");

	if(!_obj_->isDictionaryObject())
		throw internalError(__FILE__,__LINE__,"Objekt ist kein Woerterbuch");

	const Object* _type_=_obj_->getDictionary()->lookUp("/Type");
	if(_type_==NULL)
		return false;

	if(_type_->getName().compare("/Font")==0)
		return true;

	return false;
}

void	Font::parse(const Object* _obj_)
{
	//Subtype
	const Object* _tmp_ = _obj_->getDictionary()->lookUp("/Subtype");
	if(_tmp_ == NULL)
		throw syntaxError(__FILE__,__LINE__,"/Subtype ist nicht vorhanden");

	if(!_tmp_->isNameObject())
		throw syntaxError(__FILE__,__LINE__,"/Subtype ist ein indirektes Objekt");


	setSubtype( _tmp_->getName() );

	if( _obj_->getDictionary()->lookUp("/Name") )
		setName( _obj_->getDictionary()->lookUp("/Name")->getName() );
	if( _obj_->getDictionary()->lookUp("/BaseFont") )
		setBaseFont( _obj_->getDictionary()->lookUp("/BaseFont")->getName() );
	if( _obj_->getDictionary()->lookUp("/FirstChar") )
		setFirstChar( static_cast<int>(_obj_->getDictionary()->lookUp("/FirstChar")->getInteger()) );
	if( _obj_->getDictionary()->lookUp("/LastChar") )
		setLastChar( static_cast<int>(_obj_->getDictionary()->lookUp("/LastChar")->getInteger()) );
	if( _obj_->getDictionary()->lookUp("/DescendantFonts") ){
		fHasDescendantFonts = true;
	}




	if( _obj_->getDictionary()->lookUp("/Widths") )
	{
		if( _obj_->getDictionary()->lookUp("/Widths")->isIndirectObject() )
		{
			Object* tmpWidths = new Object();
			_obj_->getDictionary_M()->getIndirectObject(tmpWidths,"/Widths");
			parseWidthsArray(getFirstChar(),getLastChar(),tmpWidths);
			delete tmpWidths;
		}
		else
			parseWidthsArray(getFirstChar(),getLastChar(),
				_obj_->getDictionary_M()->lookUp_M("/Widths"));
	}

	if( _obj_->getDictionary()->lookUp("/FontDescriptor") )
	{
		if( _obj_->getDictionary()->lookUp("/FontDescriptor")->isIndirectObject() )
		{
			setFontDescriptor(new Object());
			_obj_->getDictionary_M()->getIndirectObject(getFontDescriptor(),"/FontDescriptor");
		}
		else
		{
			fFontDescriptorAdopted = true;
			setFontDescriptor(_obj_->getDictionary_M()->lookUp_M("/FontDescriptor"));
		}

		parseFontDescriptor(getFontDescriptor());
	}






	//Encoding
	_tmp_ = _obj_->getDictionary()->lookUp("/Encoding");
	if(_tmp_ == NULL)
	{
		encoding = new Encoding();
	}else	if(_tmp_->isIndirectObject())
	{
		Object* _encoding_ = new Object();
		_obj_->getDictionary()->getIndirectObject(_encoding_,"/Encoding");
		encoding = new Encoding(_encoding_);
		delete _encoding_;

	/*	if(encoding->isCMap()){
			cmap = encoding->cutCMap();
		}*/
	}else if(_tmp_->isDictionaryObject())
	{
		encoding = new Encoding(_tmp_);
	}

	//ToUnicode
	_tmp_ = _obj_->getDictionary()->lookUp("/ToUnicode");
	if(_tmp_ == NULL)
	{
		fCmap = NULL;
		return;
	}else	if(_tmp_->isIndirectObject())
	{
		Object cmap;
		_obj_->getDictionary()->getXRef()->getObject(&cmap,_tmp_->getIndirectObject()->objectNumber);
		MemoryStream cmapfile;
		if(!cmap.isStreamObject())
			throw syntaxError(__FILE__,__LINE__,"Der Eintrag /ToUnicode ist kein Stream");
		StreamFilterD	filter;
		cmap.getStream_M()->write(&cmapfile,&filter);
		cmapfile.setPosition(0);
		if( fCmap ){
			delete fCmap;
			fCmap = 0;
		}
		fCmap = new CMap(&cmapfile);
		
	}else if(_tmp_->isDictionaryObject())
		throw syntaxError(__FILE__,__LINE__,"Der Eintrag /ToUnicode ist kein Stream");
}

void	Font::setWidths(unsigned int character,double width)
{
	widths[character] = width;
}

double	Font::getWidths(unsigned int character)		const
{
	if( isStandardFont() )
		return 700;

	if( widths.empty() )
		return getMissingWidth();

	if( widths.find(character)!= widths.end() )
		return widths.find(character)->second;

	return getMissingWidth();
}

void	Font::parseWidthsArray(unsigned int _firstChar_,unsigned int _lastChar_,const Object* obj)
{
	unsigned int currentChar = _firstChar_;
	for(unsigned int i=0;currentChar<_lastChar_;i++,currentChar++)
	{
		if(i<obj->getArray()->size())
		{
			setWidths(
				currentChar,
				static_cast<unsigned int>(obj->getArray()->operator[](i)->getReal())
				);
		}else
		{
			setWidths(currentChar,0);
		}
	}
}


void	Font::parseFontDescriptor(const Object* _obj_)
{
	if(_obj_ == NULL)
		throw nullPointer(__FILE__,__LINE__);

	if( _obj_->getDictionary()->lookUp("/Type") )
	{
		if(	_obj_->getDictionary()->lookUp("/Type")->getName() != "/FontDescriptor" )
			throw invalidType(__FILE__,__LINE__,"Objekt ist nicht vom Typ /FontDescriptor");
	}else
	{
		// TODO: Fehlerbehandlung falls Objekttyp nicht angegeben ist.
	}

	/**TODO:*/
//	if( !_obj_->getDictionary()->lookUp("/FontName") )
//		logfile.logWarning(__FILE__,__LINE__,"Schluesselwort /FontName fehlt im FontDescriptor");

	fontName		=	getString(_obj_,	"/FontName"			);
	fontFamily		=	getString(_obj_,	"/FontFamily"		);
	fontStretch		=	getString(_obj_,	"/FontStretch"		);
	charSet			=	getString(_obj_,	"/CharSet"			);

	getNumber(_obj_,"/FontWeight",fontWeight);
	getNumber(_obj_,"/ItalicAngle",italicAngle);
	getNumber(_obj_,"/Ascent",ascent);
	getNumber(_obj_,"/Descent",descent);
	getNumber(_obj_,"/Leading",leading);
	getNumber(_obj_,"/CapHeight",capHeight);
	getNumber(_obj_,"/XHeight",xHeight);
	getNumber(_obj_,"/StemV",stemV);
	getNumber(_obj_,"/StemH",stemH);
	getNumber(_obj_,"/AvgWidth",avgWidth);
	getNumber(_obj_,"/MaxWidth",maxWidth);
	getNumber(_obj_,"/MissingWidth",missingWidth);

	if( !getNumber(_obj_,"/Flags",flags) )
	{
		//throw missingKey(__FILE__,__LINE__,"Schluesselwort /Flags im FontDesriptor nicht vorhanden");
	}

	if( !getNumber(_obj_,"/ItalicAngle",italicAngle) )
	{
		//throw missingKey(__FILE__,__LINE__,"Schluesselwort /ItalicAngle im FontDesriptor nicht vorhanden");
	}

	if( _obj_->getDictionary()->lookUp("/FontBBox") )
	{
		if(_obj_->getDictionary()->lookUp("/FontBBox")->isArrayObject())
		{
			for( unsigned int i =0;i<4;i++)
				fontBBox[i] = _obj_->getDictionary()->lookUp("/FontBBox")->getArray()->operator[](i)->getReal();
		}else
		{
			auto_ptr<Object> objfb(new Object());
			_obj_->getDictionary()->getXRef()->getObject(
				objfb.get(),
				_obj_->getDictionary()->lookUp("/FontBBox")->getIndirectObject()->objectNumber
				);
			for( unsigned int i =0;i<4;i++)
				fontBBox[i] = objfb->getArray()->operator[](i)->getReal();
			objfb.reset();
		}
	}

	if( _obj_->getDictionary()->lookUp("/FontFile") )
		fontFile = _obj_->getDictionary()->lookUp("/FontFile")->getIndirectObject()->objectNumber;

	if( _obj_->getDictionary()->lookUp("/FontFile2") )
		fontFile2 = _obj_->getDictionary()->lookUp("/FontFile2")->getIndirectObject()->objectNumber;

	if( _obj_->getDictionary()->lookUp("/FontFile3") )
		fontFile3 = _obj_->getDictionary()->lookUp("/FontFile3")->getIndirectObject()->objectNumber;

	Object*	fontFileObj=new Object();
	if(fontFile != 0)
	{
		_obj_->getDictionary_M()->getXRef()->getObject(fontFileObj,fontFile);
	}else if(fontFile2 != 0)
	{
		_obj_->getDictionary_M()->getXRef()->getObject(fontFileObj,fontFile2);
	}else if(fontFile3 != 0)
	{
		_obj_->getDictionary_M()->getXRef()->getObject(fontFileObj,fontFile3);
	}

	try{
		if(fontFile+fontFile2+fontFile3!=0)
			fontProgram->load(fontFileObj);
	}catch(FontException&)
	{
	}
	delete fontFileObj;
}

double			Font::getFontBBox(unsigned int i)		const
{
	if(i > 3)
		throw internalError(__FILE__,__LINE__,"Ein Rechteck hat nur vier Punkte.");

	return fontBBox[i];
}

void		Font::setBaseFont(std::string _baseFont_)	
{
	baseFont = _baseFont_;
	standardFont = false;

	if( baseFont == "/Times−Roman") standardFont = true;
	else if( baseFont == "/Times−Bold") standardFont = true;
	else if( baseFont == "/Times−Italic") standardFont = true;
	else if( baseFont == "/Times−BoldItalic") standardFont = true;
	else if( baseFont == "/Helvetica") standardFont = true;
	else if( baseFont == "/Helvetica−Bold") standardFont = true;
	else if( baseFont == "/Helvetica−Oblique") standardFont = true;
	else if( baseFont == "/Helvetica−BoldOblique") standardFont = true;
	else if( baseFont == "/Courier") standardFont = true;
	else if( baseFont == "/Courier−Bold") standardFont = true;
	else if( baseFont == "/Courier−Oblique") standardFont = true;
	else if( baseFont == "/Courier−BoldOblique") standardFont = true;
	else if( baseFont == "/Symbol") standardFont = true;
	else if( baseFont == "/ZapfDingbats") standardFont = true;
}

char	Font::decode(char _c_) const
{
	if( isType1Font() )
	{
		if( getEncoding() )
		{
			string symbolName=getEncoding()->getSymbolNameFromEncodedChar(_c_);
			if (symbolName.empty()) {
				if (fontProgram)
				{
					if (fontProgram->hasGlyphNames())
					{
						return fontProgram->characterCodeToWinAnsiCode(_c_);
					}
				}
				return getEncoding()->decode(_c_);
			}

			if(fontProgram)
			{
				if(fontProgram->hasGlyphNames())
				{
					char ret=fontProgram->getCharacterCodeFromGlyphName(symbolName);
					if(ret==0)
						return getEncoding()->decode(_c_);
					return ret;
				}
			}

			return getEncoding()->decode(_c_);
		}
	}

	if( getEncoding() )
		return getEncoding()->decode(_c_);

	return _c_;

}

char	Font::encode(char _c_) const
{
	return _c_;
	if( isType1Font() )
	{
		if( getEncoding() )
		{
			string symbolName=getEncoding()->getSymbolNameFromEncodedChar(_c_);
			if(symbolName.empty())
				return getEncoding()->encode(_c_);

			if(fontProgram->hasGlyphNames())
			{
				return fontProgram->getCharacterCodeFromGlyphName(symbolName);
			}else
			{
				return getEncoding()->encode(_c_);
			}
		}
	}

	if( getEncoding() )
		return getEncoding()->encode(_c_);

	return _c_;

}

void	Font::saveFontFile(std::string file)
{
	if( this->isEmbedded() )
		this->fontProgram->saveFontFile(file);
}

std::string		Font::getMD5Checksum()	const	
{
	return fontProgram->getMD5Sum();
}

unsigned long	Font::getNumberOfGlyphs()	const
{
	if ( this->fontProgram)
	{
		return this->fontProgram->getNumberOfGlyphs();
	}

	return 0;
} 
