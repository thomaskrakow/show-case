#ifndef __XREF_H__
#define __XREF_H__

#include "pdflib_config.h"
#include "allocator/pdflib_allocator.h"
#include "PDFTypes.h"
#include "object/Object.h"
#include "font/Font.h"
#include "ErrorHandler.h"
#include "XMemory.h"
#include "PDFValidatorHandlerAbstract.h"


#include "stream/OutputStream.h"
#include "stream/InputStream.h"

#include <map>
#include <vector>

namespace ICOM{
	namespace pdflib{

		class XRefMap;
		class Stream;
		class Dictionary;
		class Object;
		class Font;
		struct IndirectObject;
		
		/** Hilfstruktur um Elemente in einem Objektsream (Typ /ObjStm) zu identifizieren.*/
		struct	XRefEntryInObjStm{
			pdflong		index;				/*!< Index innerhalb des Objektstreams. */  
			pdflong	objectNumber;		/*!< Objektnummer des Objektstreams indem das Objekt zu finden ist. */  
		};
		
		/**Hilfsstruktur zum finden von Objekten*/
		struct XRefEntry{
			pdflong	objectNumber;			/*!< Objektnummer. */  
			pdflong	generationNumber;		/*!< Generationsnummer. */  
			pdflong	offset;					/*!< Offset des Objekt oder des Objektstreams indem das Objekt steht. */  
			char	flag;					/*!< Objektflag (kann nur n oder f sein). */  
			XRefEntryInObjStm	objStm;	/*!< Falls Objekt komprimiert, dann sind hier die Informationen des Objektstreams. */ 
		};
		
		/**	Referenztabelle eines PDF-Dokuments
		*
		*	Mit dieser Klasse liest man die (alle) Referentabelle(n) in einem 
		*	PDF-Dokument ein. Dabei ist es egal, ob es eine Referenztabelle im 
		*	alten Format (PDF-Version 1.0 -1.4) oder neuen Format (ab PDF-Version 1.5,
		*	Cross Reference-Streams) im PDF-Dokument vorhanden sind. Des Weiteren
		*	kann man mit dieser Klasse die Objekte direkt aus der PDF-Datei holen,
		*	indems sie in eine Variable vom Typ Object geladen werden.
		*	Weiterhin besteht die Moeglichkeit Referenztabellen PDF-konform zu 
		*	schreiben um so auch eigene Dokumente zu erstellen.
		*
		*	Die Referenztabelle ist dabei als Liste auzufassen. In einem PDF-Dokument
		*	koennen mehrere Referenztabellen in einem Dokument vorkommen. Diese
		*	Tabellen sind dabei verlinkt (Schluesselwort /Prev im Trailer). Findet
		*	Die Klasse eine weitere Referenztabelle, so wird diese in der Variablen
		*	prev hinterlegt. Somit ist sichergestellt, das alle Objekte im PDF-Dokument
		*	gefunden werden.
		*
		*	@todo Es gibt noch keine M�glichkeit Cross-Reference-Streams zu erzeugen.
		*
		*/
		class XRef : public XMemory
		{
		public:

			typedef	std::map<pdfulong,Font*>::const_iterator const_iterator_font;
		
			// -----------------------------------------------------------------------
			//  Konstruktoren und Destruktor
			// -----------------------------------------------------------------------
			/** @name Konstruktoren und Destruktor */
			//@{
			/**Der Standardkonstruktor*/
			XRef(ErrorHandler* errorHandler = PDFFramework::fgErrorHandler,MemoryManager* manager = PDFFramework::fgMemoryManager);
			
			/**Der Destruktor*/
			~XRef();
			//@}
			
			// -----------------------------------------------------------------------
			//  Methoden zum parsen der Referenztabelle
			// -----------------------------------------------------------------------
			/** @name Methoden zum parsen der Referenztabelle */
			//@{
			/**	Liest die Referenztabelle eines PDF-Dokumentes ein.
			*
			*	Liest die Referenztabelle eines Dokumentes ein. Der
			*	Methode ist es dabei egal in welchem Fomat die 
			*	Referenztabelle vorliegt. Die Referentabelle kann 
			*	dabei entweder das alte Format (bis PDF 1.4), oder 
			*	das neue Format (ab PDF 1.5) haben. Bei Bedarf sucht
			*	diese Funktion automatisch nach den Anfang der ersten
			*	Referentabelle. Es wird automatisch erkannt, ob es sich
			*	um ein linearisiertes Dokument handelt oder nicht. Dazu 
			*	wird das erste Objekt eines PDF-Dokuments eingelesen und
			*	dann entschieden, ob es linearisiert ist oder nicht. Kommen
			*	mehrere Referenztabellen im Dokument vor erkennt die Methode
			*	dies automatisch und f�gt diese in der variablen prev hinzu.
			*
			*	@param	fInputStream	Stream aus dem die Referenztabelle ausgelesen
			*						werden soll.
			*
			*	@param	_searchRefTable_	Ist _searchRefTable_= true dann sucht
			*								die Methode nach der ersten Referenztabelle
			*								im Dokument. Ist _searchRefTable_= false
			*								wird davon ausgegangen, dass fInputStream
			*								am Anfang einer Referenztabelle steht.
			*
			*	@exception syntaxError	Fehler beim Parsen. Entweder ist die Referenztabelle
			*							syntaktisch nciht korrekt oder die Anzahl der Elemente
			*							stimmt nicht der vorgegebenen Anzahl der Elemente 
			*							�berein.
			*
			*	@exception internalError	Es wurde versucht ein weiteren Trailer zur
			*								Referenztabelle hinzuzuf�gen.
			*/
			void		parse(InputStream* fInputStream,bool _searchRefTable_=true, PDFValidatorHandlerAbstract* validator = 0);
			
			//@}
			
			
			// -----------------------------------------------------------------------
			//  Getter Methoden
			// -----------------------------------------------------------------------
			/** @name Getter Methoden */
			//@{		
			/**	Ueberprueft, ob ein Objekt in der Referenztabelle vorhanden ist.
			*
			*	@param	_objNum_	Zu ueberpruefende Objektnummer.
			*
			*	@retval true	Objekt ist in Referenztabelle vorhanden.
			*	@retval false	Objekt ist nicht in Referenztabelle vorhanden.
			*/
			bool				exist(pdflong _objNum_) const;		

			/**	Gibt das Flag des angegebenen Objekts zurueck.
			*
			*	@param	_objectNumber__		Objektnummer.
			*
			*	@retval 0		Objekt ist nicht in Referenztabelle vorhanden.
			*	@retval sonst	Flag des Objekts.
			*/			
			char				getFlag(const pdflong _objectNumber_) const;
			
			/**	Gibt die groesste vorkommende Objektnummer zurueck.
			*
			*	@return		groesste Objektnummer in Referenztabelle.
			*/
			pdflong				getGreatestObjectNumber() const;
			
			/** Gibt ein konkretes Objekt zurueck.
			*
			*	Mit dieser Methode kann man einzelne Objekte aus
			*	der Referenztabelle anfordern, die dann aus dem
			*	intern gespeicherten Stream ausgelesen werden.
			*	Das Objekt muss dabei vorhr erzeugt worden sein.
			*
			*	Beispiel:
			*	@code
			*		Object* _obj_ = new object(__FILE__,__LINE__);
			*		xref->getObject(_obj_,100);
			*		delete _obj_;			*
			*	@endcode
			*
			*	@param	_obj_	Variable in der das Objekt abgelegt werden soll.
			*
			*	@param	_objNum_	Objektnummer des angeforderten Objekts.
			*
			*	@retval true		Objekt wurde erfolgreich ausgelesen.
			*	@retval	false		Es wurde ein Nullobjekt oder kein Objekt ausgelesen.
			*
			*	@exception internalError	_obj_ ist ein Nullzeiger.
			*
			*	@exception IOError		Die Datei ist nicht geoeffnet oder die
			*							Referenztabelle ist fehlerhaft.
			*	@exception syntaxError	An der angegebenen Stelle ist ein syntaktisch
			*							fehlerhaftes Objekt.
			*							Bei komprimierten Objekten ist das Superobjekt
			*							kein oder ein fehlerhaftes Objektstream.
			*/
			bool				getObject(Object* _obj_,pdflong _objNum_) const;
			
			/**	Gibt das im Trailer angegebene Root-Objekt zurueck.
			*
			*	@return 	Das Root-Objekt im Trailer.
			*
			*	@exception	internalError 	In der referentabelle ist kein Trailer
			*								hinterlegt.
			*
			*	@exception	IOError 	Das Root-Objekt wurde nicht gefunden.
			*/
			Object*		getRoot() const;
			
			/**	Gibt die Objektnummer der Wurzel des Seitenbaums zurueck.
			*
			*	@return Die Objektnummer der Wurzel des Seitenbaums.
			*
			*	@exception	syntaxError	Angefordertes Objekt ist nicht 
			*							vom richtigen Typ, oder enthaelt 
			*							nicht alle Elemente.
			*/
			IndirectObject				getRootPages() const;
			
			/**	Gibt die Wurzel des Seitenbaums zurueck.
			*
			*	@param	_obj_ Objekt indem die wurzel gespeichert werden soll.
			*
			*
			*	@exception syntaxError	Diese Exception tritt auf, wenn\n
			*							-der Katalog kein indirektes Objekt ist,\n
			*							-der Katalog kein Woerterbuch ist,\n
			*							-der Eintrag /Pages im Katalog fehlt,\n
			*							-der Eintrag /Pages im Kataolg kein indirektes Objekt ist.
			*
			*	@exception nullPointer	_obj_ ist ein Nullzeiger.
			*/
			XRef*				getPrev() const	{return prev;}
			
			/**
			 *  Speichert das Wurzelobjekt des Dokumentenbaumes in die angegebene Variable.
			 *
			 *  @param _obj_ Objekt indem das Wurzelobjekt des Dokumentenbaumes abgelegt werden soll.
			 */
			void				getRootPages(Object* _obj_) const;
			
			/**
			 *  Gibt das Trailerobjekt zurueck.
			 *
			 *  @return Trailerobjekt zur Referenztabelle.
			 */
			const Object*		getTrailer() const;
			
			/**
			 *  Gibt das Trailerobjekt zurueck.
			 *
			 *  @return Trailerobjekt zur Referenztabelle.
			 */
			Object*				getTrailer_M() const;
			pdflong				getICOMMetadataObjNum();
			
			/**
			 *  Gibt den Offset an dem die Referenztabelle steht zurueck.
			 *
			 *  @return Offset an dem die Referenztabelle steht.
			 */
			pdflong				getXRefOffset() const	{return offset;}
			
			/**
			 *  Gibt die Anzahl der Eintraege der Referenztabelle zurueck.
			 *
			 *  @return Anzahl der Eintraege in der Referenztabelle.
			 */
			pdflong				size() const;
			//@}
			
			// -----------------------------------------------------------------------
			//  Methoden zum manipulieren der Referenztabelle
			// -----------------------------------------------------------------------
			/** @name Methoden zum manipulieren der Referenztabelle */
			//@{	
			
			/**
			 *  Fuegt einen Eintrag in der Referenztabelle hinzu.
			 *
			 *  @param entry Eintrag der hinzugefuegt werden soll.
			 */
			bool		addEntry(XRefEntry entry);
			
			/**
			 *  Fuegt einen Eintrag in der Referenztabelle hinzu.
			 *
			 *  @param objnum Objektnummer des Objektes welcher hinzugefuegt werden soll.
			 *  @param gennum Generationsnummer des Objektes welches hinzugefuegt werden soll.
			 *  @param offset Byteposition innerhalb der Ausgabedatei von dem Objekt welches 
			 *     hinzugefuegt werden soll.
			 */
			bool		addEntry(pdflong objnum,pdflong gennum,pdflong offset);
			
			/**
			 *  Setzt die Byteposition eines Objektes innerhalb einer Datei.
			 *
			 *  @param objnum Objektnummer des Objektes welcher hinzugefuegt werden soll.
			 *  @param gennum Generationsnummer des Objektes welches hinzugefuegt werden soll.
			 *  @param offset Byteposition innerhalb der Ausgabedatei von dem Objekt welches 
			 *     hinzugefuegt werden soll.  
			 */
			void		setOffset(pdflong objnum,pdflong gennum,pdflong offset);
			/*bool		removeEntry(const pdflong _objectNumber_);*/
			void		setPrev(pdflong _offset_);
			void		setICOMMetadataObjNum(pdflong objnum);
			void		setRoot(pdflong _objNum_,pdflong _genNum_);
			void		setStream(InputStream* fInputStream) {stream = fInputStream;}
			bool		isLinear() const	{return linearizedObject > 0;}
			InputStream*		getStream() const {return stream;}
			//@}
			
			// -----------------------------------------------------------------------
			//  Sonstige Methoden
			// -----------------------------------------------------------------------
			/** @name Sonstige Methoden */
			//@{
			void		checkTrailer(Object* _trailer_, PDFValidatorHandlerAbstract* validator = 0);
			void		write(OutputStream* fInputStream);
			void		cacheFont(pdfulong objNum,pdfulong genNum,Object* obj);
			const Font*	getFont(pdfulong objNum) const;
			std::map<pdfulong,Font*>::const_iterator font_begin() const {return fonts.begin();}
			std::map<pdfulong,Font*>::const_iterator fonts_end() const {return fonts.end();}
			//@}

			void	setAllowSubsections(bool val);
			
		protected:
			const XRefEntry*	getEntry(const pdflong _objectNumber_) const;
			void				write(const XRefEntry* _entry_,OutputStream* fInputStream) const;
			void				findStart(InputStream* fInputStream);
			void				parseXRefStream(InputStream* fInputStream,const Dictionary* _dictionary_, PDFValidatorHandlerAbstract* validator = 0);
			void				parse_xref_stream(InputStream* fInputStream, PDFValidatorHandlerAbstract* validator = 0);
			void				interpretXRefStreamEntry(const std::vector< std::vector<unsigned char> >& _entry_,pdflong _objNum_);
			pdflong				hexToDec(const vector<unsigned char>& _hex_);
			bool				getObject(Object* _obj_,pdflong _objNum_,InputStream* fInputStream,pdflong _n_,pdflong _first_) const;
			
			MemoryManager*			fMemoryManager;
			ErrorHandler*			fErrorHandler;
		private:

			void				_read_trailer_(InputStream* fInputStream, PDFValidatorHandlerAbstract* validator = 0);
			void				_setLinearizedObject(pdfulong _objNum_) {linearizedObject = _objNum_;}
			
			XRefMap*						fXRefMap;
			Object*							trailer;
			pdfulong						offset;
			pdflong							greatestObjectnumber;
			InputStream*					stream;
			pdfulong						linearizedObject;

			XRef*							prev;

			std::map<pdfulong,Font*>		fonts;
			pdflong							fICOMMetadataObjNum;
		};
	}

}

#endif  //__XREF_H__


 
