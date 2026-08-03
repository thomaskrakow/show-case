/* --------------------------------------------------------------
*	Revision:		$Rev: 14974 $
*
*	zuletzt geändert am: 	$Date: 2010-10-04 12:45:04 +0200 (Mo, 04 Okt 2010) $
*
*	zueltzt geändert von:	$Author: thomaskrakow $
*
*	Subversion Speicherort:	$HeadURL: http://svn.icomsoftware.dortmund/svn/source/pdflib/trunk/pdflib/src/src/Font.h $
*
* -------------------------------------------------------------- */

/*!		@page		Font.h Versionsdokumentation für Font.h
 *
 *		@section	Versionsdaten
 *		@verbatim	$HeadURL: http://svn.icomsoftware.dortmund/svn/source/pdflib/trunk/pdflib/src/src/Font.h $	@endverbatim
 *		@version	$Rev: 14974 $
 *		@date		$Date: 2010-10-04 12:45:04 +0200 (Mo, 04 Okt 2010) $
 *		@author		$Author: thomaskrakow $
 *
 *		@section	Anmerkungen
 */
/* --------------------------------------------------------------*/

#ifndef __FONT_H__
#define __FONT_H__

#include "pdflib_config.h"
#include "allocator/pdflib_allocator.h"
#include "font/FontProgram.h"
#include "PDFTypes.h"
#include "object/Object.h"
#include "font/Encoding.h"
#include "CMap.h"

#include <vector>

namespace ICOM{
	namespace pdflib{

		class CMap;
		class FontProgram;
		class Encoding;
	
	/** Klasse zum arbeiten mit Objekten vom Typ Font
	*
	*	Diese Klasse bietet Schnittstellen zum bearbeiten und
	*	untersuchen von Font-Objekten. Bisher wird nur untersucht, 
	*	ob alle notwendigen Schluesselwoerter im Objekt enthalten 
	*	sind und ggf. das Encoding-Objekt erzeugt.
	*/
	class Font PDFLIB_MASTERCLASS
	{
	public:
	
		// -----------------------------------------------------------------------
		//  Konstruktoren und Destruktor
		// -----------------------------------------------------------------------
		/** @name Konstruktoren und Destruktor */
		//@{
		
		/**	Erzeugt eine Font-Klasse aus dem entsprechenden Font-Objekt.
		*
		*	Hier wird ueberprueft, ob das uebergebene Objekt tatsaechlich ein
		*	Font-Objekt ist. Dies geschieht mittels Aufruf der Funktion
		*	#testObject . Ist das Objekt tatsaechlich ein Schriftobjekt wird,
		*	mittels der Funktion #parse, die internen Variablen gefuellt.
		*
		*	@param _obj_ 	Ein Zeiger auf ein Font-Objekt.
		*
		*	@exception	internalError	Objekt _obj_ ist kein Font-Objekt.
		*/
		Font(const Object* _obj_,pdflong objNum,pdflong genNum);
		
		/** Destruktor
		*
		*	Loescht ggf. eine erzeugte Encoding-Klasse.
		*/
		virtual ~Font();
		//@}

		char	decode(char _c_) const;
		char	encode(char _c_) const;

		
		// -----------------------------------------------------------------------
		//  Getter-Methoden
		// -----------------------------------------------------------------------
		/** @name Getter-Methoden */
		//@{
		
		/**	Gibt eine Instanz der Klasse Encoding zurueck.
		*
		*	@return	Das in der Variablen #encoding gespeicherte Encoding
		*			Objekt.
		*/
		const Encoding*	getEncoding() const	{return encoding;}
			
		const CMap*		getCMap()	const	{return fCmap;}

		pdflong			getObjectNumber()	const	{return inObj.objectNumber;}
		pdflong			getGenerationNumber() const {return inObj.generationNumber;}

		std::string		getSubtype()	const		{return subtype;}
		std::string		getName()		const		{return name;}
		std::string		getBaseFont()	const		{return baseFont;}
		std::string		getFontName()	const		{return fontName;}
		std::string		getFontFamily()	const		{return fontFamily;}
		std::string		getFontStretch() const		{return fontStretch;}
		std::string		getCharSet()	const		{return charSet;}
		std::string		getMD5Checksum()	const;

		int				getFirstChar()		const	{return firstChar;}
		int				getLastChar()		const	{return lastChar;}
		int				getFontWeight()		const	{return fontWeight;}
		int				getItalicAngle()	const	{return italicAngle;}
		int				getAscent()			const	{return ascent;}
		int				getDescent()		const	{return descent;}
		int				getLeading()		const	{return leading;}
		int				getCapHeight()		const	{return capHeight;}
		int				getXHeight()		const	{return xHeight;}
		int				getStemV()			const	{return stemV;}
		int				getStemH()			const	{return stemH;}
		int				getAvgWidth()		const	{return avgWidth;}
		int				getMaxWidth()		const	{return maxWidth;}
		int				getMissingWidth()	const	{return missingWidth;}

		unsigned long	getFlags()			const	{return flags;}
		unsigned long	getNumberOfGlyphs()	const;
		double			getFontBBox(unsigned int i)		const;

		bool			isEmbedded()	const {return fontFile || fontFile2 || fontFile3;}
		bool			isHasDescendantFonts() const {return fHasDescendantFonts;}
		bool			isUnicode ()	const {return fCmap != NULL;}

		bool			isFixedPitch()	const	{return (flags & _flag_fixedPitch) != 0;}
		bool			isSerif()		const	{return (flags & _flag_Serif) != 0;}
		bool			isSymbolic()	const	{return (flags & _flag_Symbolic) != 0;}
		bool			isScript()		const	{return (flags & _flag_Script) != 0;}
		bool			isNosymbolic()	const	{return (flags & _flag_Nosymbolic) != 0;}
		bool			isItalic()		const	{return (flags & _flag_Italic) != 0;}
		bool			isAllCap()		const	{return (flags & _flag_AllCap) != 0;}
		bool			isSmallCap()	const	{return (flags & _flag_SmallCap) != 0;}
		bool			isStandardFont()const	{return	standardFont;}
		bool			isForceBold()	const	{return (flags & _flag_ForceBold) != 0;}

		bool			isType0Font()	const	{return getSubtype()=="/Type0";}
		bool			isType1Font()	const	{return getSubtype()=="/Type1";}
		bool			isType3Font()	const	{return getSubtype()=="/Type3";}
		bool			isMMType1Font()	const	{return getSubtype()=="/MMType1";}
		bool			isTrueTypeFont()	const	{return getSubtype()=="/TrueType";}
		bool			isCIDFontType0Font()	const	{return getSubtype()=="/CIDFontType0";}
		bool			isCIDFontType2Font()	const	{return getSubtype()=="/CIDFontType2";}

		double			getWidths(unsigned int character)		const;
		Object*	getFontDescriptor()		const		{return fontDescriptor;}

		FontProgram*	getFontProgram() const {return fontProgram;}
		//@}

		void	saveFontFile(std::string file);

	protected:
	private:
	
		/** Ueberprueft, ob das Objekt ein korrektes
		*	Schriftobjekt ist.
		*
		*	Hier wird ueberpruft, ob das Objekt kein Nullobjekt
		*	und ein Woerterbuch ist. Anschliessend wird ueberpruft, 
		*	ob der Eintrag /Type vorhanden ist und ggf. der Wert 
		*	/Font ist.
		*
		*	@param	Das zu ueberpruefende Schriftobjekt.
		*
		*	@retval true	Das objekt ist ein korrektes Schriftobjekt.
		*	@retval	false	Das Objekt ist kein korrektes Schriftobjekt.
		*/
		bool	testObject(const Object* _obj_);
		
		/**	Analysiert das Font-Objekt und fuellt die privaten
		*	Variablen.
		*
		*	Kommt der Eintrag /Encoding im Font-Objekte vor,
		*	so wird der Wert extrahiert und eine Instanz von
		*	Encoding erzeugt. Das Ergebnis wird in #encoding 
		*	gespeichert. Ist kein /Encoding Eintrag vorhanden,
		*	so wird trotzdem eine Instanz der Klasse Encoding 
		*	erzeugt, bloss ohne Encoding-Objekt.
		*
		*	@param	_obj_ Das zu untersuchende Schriftobjekt.
		*
		*	@exception	synatxError /Subtype ist nicht vorhanden, 
		*				oder ein indirektes Objekt.
		*/
		void	parse(const Object* _obj_);

		void	parseFontDescriptor(const Object* _obj_);
		void	parseWidthsArray(unsigned int _firstChar_,unsigned int _lastChar_,const Object* obj);

		void		setSubtype(std::string _subtype_)			{subtype = _subtype_;}
		void		setName(std::string _name_)					{name = _name_;}
		void		setBaseFont(std::string _baseFont_);
		void		setFirstChar(int _firstChar_)		{firstChar = _firstChar_;}
		void		setLastChar(int _lastChar_)		{lastChar = _lastChar_;}

		void		setWidths(unsigned int character,double width);
		void		setFontDescriptor(Object* _fontDescriptor_)		{fontDescriptor = _fontDescriptor_;}
		
		/**Den Kopierkonstrukter kann man nicht direkt aufrufen.*/
		Font(const Font& _font_);

		IndirectObject	inObj;

		Encoding*	encoding;	//!< Instanz eines Encoding-Objektes zum entsprechenden Eintrag im Woerterbuch
		CMap*		fCmap;

		std::string		subtype;    //!< Wert des Schlusselwortes /Subtype
		std::string		name;
		std::string		baseFont;
		int				firstChar;
		int				lastChar;
		Object*			fontDescriptor;


		/** @name Font Descriptor */
		//@{
		bool			fFontDescriptorAdopted;
		std::string		fontName;
		std::string		fontFamily;
		std::string		fontStretch;
		int				fontWeight;
		unsigned long	flags;
		double			fontBBox[4];
		int				italicAngle;
		int				ascent;
		int				descent;
		int				leading;
		int				capHeight;
		int				xHeight;
		int				stemV;
		int				stemH;
		int				avgWidth;
		int				maxWidth;
		int				missingWidth;
		bool			standardFont;
		bool			fHasDescendantFonts;
		std::string		charSet;
		std::map<unsigned int,double>	widths;

		pdfulong		fontFile;
		pdfulong		fontFile2;
		pdfulong		fontFile3;

		FontProgram*	fontProgram;

		static const unsigned long	_flag_fixedPitch	= 0x01;
		static const unsigned long	_flag_Serif			= 0x02;
		static const unsigned long	_flag_Symbolic		= 0x04;
		static const unsigned long	_flag_Script		= 0x08;
		static const unsigned long	_flag_Nosymbolic	= 0x10;
		static const unsigned long	_flag_Italic		= 0x20;

		static const unsigned long	_flag_AllCap		= 0x20000;
		static const unsigned long	_flag_SmallCap		= 0x40000;
		static const unsigned long	_flag_ForceBold		= 0x80000;
		//@}

	};
	}
}


#endif //__FONT_H__




 
