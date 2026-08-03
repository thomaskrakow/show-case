
#ifndef __BASESTREAM_H__
#define __BASESTREAM_H__

#include "pdflib_config.h"
#include "allocator/pdflib_allocator.h"
#include "PDFTypes.h"
#include "object/Dictionary.h"
#include "filter/BaseFilter.h"
#include "stream/InputStream.h"
#include "stream/OutputStream.h"
#include "stream/MemoryStream.h"

namespace ICOM{
	namespace pdflib{

		class BaseFilter;
		
		/** Diese Klasse implementiert den speziellen Objekttyp Stream.
		*
		*	Ein Stream ist ein Woerterbuch zusammen mit Binaerdaten. Das Woerterbuch
		*	wird in der variablen dictionary gespeichert. Die Binaerdaten werden
		*	in der Variablen fData gespeichert. Zusaetzlich enthaelt die Klasse eine
		*	Variable filter. In ihr ist der bei der Ausgabe anzuwendende Filter zu 
		*	speichern. 
		*
		*/
		class BaseStream PDFLIB_MASTERCLASS
		{
		public:
		
			// -----------------------------------------------------------------------
			//  Konstruktoren und Destruktor
			// -----------------------------------------------------------------------
			/** @name Konstruktoren und Destruktor */
			//@{
			
			/**	Erstellt ein Stream-Objekt.
			*
			*	@param _xref_ 	Die Referenztabelle dem das Objekt
			*					angehoert oder angehoeren soll.
			*/
			BaseStream(const XRef* _xref_);
			
			/**	Der Standard-Destruktor.*/
			virtual ~BaseStream();
			//@}

			/**	Gibt das Woerterbuch des Stream-Objekts zurueck.
			*
			*	@return	Das Woerterbuch des Stream-Objekts.
			*
			*	@see				#getDictionary_M();
			*/
			const Dictionary*		getDictionary() const {return dictionary;};
			
			/**	Gibt das Woerterbuch des Stream-Objekts zurueck.
			*
			*	@return	Das Woerterbuch des Stream-Objekts.
			*
			*	@see				#getDictionary();
			*/
			Dictionary*				getDictionary_M() const {return dictionary;};
			void					setDicionary(Dictionary* dic) {dictionary = dic;}
			
			/**	Gibt die Laenge des Streams zuruck.
			*
			*	Die Laenge des Streams wird intern in der Variablen getLength gespeichert.
			*	Ist die Laenge noch nicht bekannt, so wird die Laenge aus dem Woerterbuch
			*	des Streamobjekts ausgelesen und in der Variablen getLength gespeichert.
			*	Beim wiederholten aufruf der Funktion wird nur noch der Wert der Variablen
			*	getLength zurueck gegeben. Somit wird das mehrmalige auslesen der Laenge aus dem
			*	Woerterbuch vermieden.
			*
			*	@return					Die Laenge des Streams.
			*
			*	@exception syntaxError 	Das Schluesselwort /Length ist nicht im Woerterbuch
			*							des Streams vorhanden.
			*
			*	@exception	IOError		/Length verweist auf ein indirektes Objekt,
			*							welches wiederum auf ein indirektes objekt verweist.
			*/
			virtual pdflong			getLength();
			
			/**	Liest den Orginaldatenstrom aus einen Stream.
			*
			*	@param 	_stream_ 	Stream in dem die Daten gespeichert sind.
			*	@param	_getLength_	Wieviel Bytes sollen ausgelesen werden.
			*
			*	@exception IOError	Diese Exception tritt in folgenden Faellen auf.
			*						- Die Variable _stream_ ist ein Nullzeiger.
			*						- Die Variable _getLength_ ist kleiner als Null.
			*						- Die interne Variable ist kein MemoryStream-Objekt.
			*
			*	@todo	Die Orginaldaten sollten auch aus anderen Streams ausgelesen
			*			werden koennen.
			*					
			*/
			virtual void			readRawData(InputStream* _stream_,pdfulong _getLength_);
			
			/**	Setzt den Filter des Streams.
			*
			*	Dieser Filter wird bei der Ausgabe des Streams auf die Binaerdaten und
			*	ggf. auf das Woerterbuch des Streams angewendet. Ist der Filter ein
			*	ein Nullzeiger bedeutet dies, dass kein Filter auf die daten des Streams 
			*	anzuwenden ist.
			*
			*	@param	_filter_	Filter der auf die Daten des Streamobjekts anzuwenden ist.
			*/
			virtual void			setFilter(BaseFilter* _filter_)	{filter=_filter_;}
			virtual BaseFilter*		getFilter() const {return filter;}
			virtual void			write(OutputStream* _stream_);
			virtual void			write(OutputStream* _stream_,BaseFilter* _filter_);

			void			copy(Object* _dst_) const;
			MemoryStream*	getData() const	{return fData;}

			::std::string		getMD5Checksum(unsigned char* md5);

			void	setLength(pdflong _length_) {length=_length_;}

		protected:
		private:
		
			/**Den Kopierkonstrukter kann man nicht direkt aufrufen.*/
			BaseStream(const BaseStream& _baseStream_);
		
			Dictionary*		dictionary;
			BaseFilter*		filter;
			MemoryStream*	fData;
			const XRef*		fXRef;
			pdflong			length;
		};
	}
}

#endif // __BASESTREAM_H__
 
