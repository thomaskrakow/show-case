
#include "Exception.h"
#include "Parser.h"
#include "XRef.h"
#include "ErrorCodes.h"
#include "PDFCoreException.h"
#include "XRefMap.h"
#include "iclibdefines.h"
#include "UUID.h"

#include "stream/FileOutputStream.h"
#include "stream/FileInputStream.h"
#include "stream/MemoryStream.h"
#include "stream/StreamUtility.h"

#include "filter/FlateFilterD.h"
#include "filter/StreamFilterD.h"
#include "filter/QueueFilter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace ICOM::pdflib;

#define new PDFLIB_ALLOCATOR

XRef::XRef(ErrorHandler* errorHandler,MemoryManager* manager) : 
	fMemoryManager(manager),
	fErrorHandler(errorHandler),
	fICOMMetadataObjNum(0),
	fXRefMap(0),
	linearizedObject(0)
{
	prev=NULL;
	offset=0;
	greatestObjectnumber=0;
	trailer=NULL;
	stream=NULL;

	fXRefMap = new XRefMap;
}

XRef::~XRef()
{
	if( fXRefMap ){
		delete fXRefMap;
		fXRefMap = 0;
	}

	if(prev)
		delete prev;

	if(trailer)
		delete trailer;

	while( !fonts.empty() )
	{
		delete fonts.begin()->second;
		fonts.erase(fonts.begin());
	}
}

bool		XRef::exist(pdflong _objNum_) const
{
	if(fXRefMap->exist(_objNum_))
		return true;
	else if(prev==NULL)
		return false;
	else
		return prev->exist(_objNum_);
}

bool		XRef::addEntry(XRefEntry entry)
{
	bool ret = fXRefMap->pushBack( entry );

	if(greatestObjectnumber<entry.objectNumber)
		greatestObjectnumber=entry.objectNumber;

	if( entry.objectNumber == 0)
		return true;

	if(ret == false){
		PDFErrorHandlerFatal1(
				fErrorHandler,
				PDFCoreException,
				ErrorCodes::ERROR_DOUBLE_OBJECT_IN_XREF,
				PDFString::toString(entry.objectNumber).c_str()
			);
	}

	return true;
}

/*bool		XRef::removeEntry(const pdflong _objectNumber_)
{
	fXRefMap->remove( _objectNumber_ );

	return true;
}*/

void		XRef::_read_trailer_(InputStream* _stream_, PDFValidatorHandlerAbstract* validator)
{
	char _buf_[7];
	_stream_->read(_buf_,7);
	if( strncmp(_buf_,"trailer",6) != 0){
		PDFErrorHandlerFatal(
				fErrorHandler,
				PDFCoreException,
				ErrorCodes::ERROR_MISSING_TRAILER
			);
	}
	Parser p(_stream_);
	if(trailer){
		PDFErrorHandlerFatal(
				fErrorHandler,
				PDFCoreException,
				ErrorCodes::ERROR_INTERNAL_TRAILER_ALREADY_EXIST
			);
	}
	trailer=new Object();
	p.getObject(trailer,NULL,this);
	this->checkTrailer(trailer, validator);
	//trailer->setXRef(this);
	const Object*	_prev_=trailer->getDictionary()->lookUp("/Prev");
	if(_prev_!=NULL)
	{
		if(_prev_->getInteger()<(pdflong)_stream_->getLength())
		{
			_stream_->setPosition(_prev_->getInteger());
			prev=new XRef();
			prev->parse(_stream_,false);
		}else
		{
			//logfile.logStatus(__FILE__,__LINE__,
			//	"Es wird auf eine Referenztabelle verwiesen die nicht existiert",0);
			/**TODO:*/
		}
		if(prev)
		{
			if(greatestObjectnumber<prev->getGreatestObjectNumber())
				greatestObjectnumber=prev->getGreatestObjectNumber();
		}
	}
	_stream_->setPosition(offset);
}

void		XRef::parse(InputStream* _stream_,bool _searchRefTable_, PDFValidatorHandlerAbstract* validator)
{
	stream = _stream_;
	if(!_stream_->good()){
		PDFErrorHandlerFatal1(
				fErrorHandler,
				PDFCoreException,
				ErrorCodes::ERROR_READING_STREAM_XREF,
				_stream_->getURI()
			);
	}

	if(_searchRefTable_)
		findStart(_stream_);

/*	if( !this->isLinear() ){
		pdflong tablesize = _stream_->getLength() - _stream_->getPosition();
		tablesize = tablesize / 20;
		fXRefMap->resize( ICLIB_MIN(tablesize * sizeof(XRefMap), 1024 *1024 * 10) ); 
	}*/

	while(_stream_->good() && lexer::isWhitespace(_stream_->peek()))
		_stream_->get();
	offset = _stream_->getPosition();
	
	string _msg_;
	switch(_stream_->peek())
	{
	case 'x':
		break;
	case '%':
		PDFErrorHandlerFatal(
				fErrorHandler,
				PDFCoreException,
				ErrorCodes::ERROR_COMMENT_IN_XREF
			);
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		parse_xref_stream(_stream_, validator);
		if(trailer->getDictionary()->lookUp("/Prev")!=NULL)
		{
			if(trailer->getDictionary()->lookUp("/Prev")->getInteger()<(pdflong)_stream_->getLength())
			{
				_stream_->setPosition(trailer->getDictionary()->lookUp("/Prev")->getInteger());
				prev=new XRef();
				prev->parse(_stream_,false);
			}else
			{
			//	logfile.logStatus(__FILE__,__LINE__,
			//		"Es wird auf eine Referenztabelle verwiesen die nicht existiert",0);
				/**TODO:*/
			}
			if(greatestObjectnumber<prev->getGreatestObjectNumber())
				greatestObjectnumber=prev->getGreatestObjectNumber();
		}
		_stream_->setPosition(offset);
		trailer->setXRef(this);
		return;
	default:
		if( !StreamUtility::findNext(_stream_,"xref") )
		{
			PDFErrorHandlerFatal1(
				fErrorHandler,
				PDFCoreException,
				ErrorCodes::ERROR_FIND_KEY_XREF,
				_stream_->getURI()
			);
		}
	}

	vector<string> _line_elements_;
	pdflong _currentObjectNumber_=0;
	pdflong _size_=0,_entry_=0;
	XRefEntry _xrefentry_;
	memset(&_xrefentry_,0,sizeof(XRefEntry));
	while(_stream_->good())
	{
		while( lexer::isWhitespace(_stream_->peek()) )
			_stream_->get();

		if(_stream_->peek() == 't')
		{
			_read_trailer_(_stream_, validator);
			trailer->setXRef(this);
			return;
		}

		_line_elements_=lexer::divide(lexer::getLine(_stream_));

		if(!_stream_->good()){
			PDFErrorHandlerFatal1(
				fErrorHandler,
				PDFCoreException,
				ErrorCodes::ERROR_UNEXPECTED_EOF_XREF,
				_stream_->getURI()
			);
		}


		switch(_line_elements_.size())
		{
		case 0:
			break;
		case 1:
			if(_line_elements_[0].compare("xref")==0)
			{
				continue;
			}
			else if(_line_elements_[0][0]=='%')
			{
				
			}else
			{
				PDFErrorHandlerFatal2(
					fErrorHandler,
					PDFCoreException,
					ErrorCodes::ERROR_UNEXPECTED_EOF_XREF,
					_stream_->getURI(),
					PDFString::toString(_stream_->getPosition()).c_str()
				);
			}
			break;
		case 2:
			_currentObjectNumber_=pdf_atol(_line_elements_[0].c_str());
			_size_=pdf_atol(_line_elements_[1].c_str());			
			_entry_=0;
			break;
		case 3:
			if(_line_elements_[0].compare("xref")==0)
			{
				continue;
			}
			if(_size_<_entry_ && _size_>0)
				throw syntaxError(__FILE__,__LINE__,
					"Falsche Anzahl an Eintraegen in der Referenztabelle");

			_xrefentry_.offset=pdf_atol(_line_elements_[0].c_str());
			_xrefentry_.generationNumber=pdf_atol(_line_elements_[1].c_str());
			_xrefentry_.flag=_line_elements_[2][0];

			/*
			 * Siehe Call 25356 (ITEBO) 
			 */
			if (		_xrefentry_.offset == 0 
					&&  _xrefentry_.generationNumber == 65535 
					&&  _xrefentry_.flag == 'f' 
					&& _currentObjectNumber_ == 1) {
				_xrefentry_.objectNumber = 0;
			}
			else {
				_xrefentry_.objectNumber = _currentObjectNumber_++;
			}
			
			_entry_++;
			addEntry(_xrefentry_);
			memset(&_xrefentry_,0,sizeof(XRefEntry));
			break;
		default:
			string _msg_ = "Beschaedigte PDF-Datei an Offset " + PDFString::toString(_stream_->getPosition());
			throw syntaxError(__FILE__,__LINE__,_msg_);
			break;

		}
	}

	_stream_->setPosition(offset);
	throw syntaxError(__FILE__,__LINE__,"Syntax Error: Beschaedigte PDF-Datei");
}

const XRefEntry*	XRef::getEntry(const pdflong _objectNumber_) const
{
	XRefEntry* entry = fXRefMap->getEntry( _objectNumber_ );
	if( prev && !entry )
		return prev->getEntry(_objectNumber_);
	return entry;
}

char		XRef::getFlag(const pdflong _objectNumber_) const
{
	XRefEntry* entry = fXRefMap->getEntry( _objectNumber_ );
	if( !prev && !entry )
		return prev->getFlag(_objectNumber_);
	return 'f';
}

Object*		XRef::getRoot() const
{
	if(trailer == NULL)
		throw internalError(__FILE__,__LINE__,"Kein Trailer vorhanden");
	Object* _ret_=trailer->getDictionary_M()->lookUp_M("/Root");
	if(_ret_==NULL)
		throw IOError(__FILE__,__LINE__,
			"Das Stammobjekt wurde nicht gefunden.");
	return _ret_;
}

pdflong	XRef::getGreatestObjectNumber() const
{
	return greatestObjectnumber;
}

void		XRef::write(const XRefEntry* _entry_,OutputStream* _stream_) const
{

	char 	_buffer_[64];

	pdf_ltoa(_buffer_,_entry_->offset);
	for(unsigned int i=0;i<10-strlen(_buffer_) && 10-strlen(_buffer_)<=10;i++)
		StreamUtility::write(_stream_,"0");

	StreamUtility::write(_stream_,_entry_->offset);
	StreamUtility::write(_stream_," ");

	pdf_ltoa(_buffer_,_entry_->generationNumber);
	for(unsigned int i=0;i<5-strlen(_buffer_);i++)
		StreamUtility::write(_stream_,"0");
	StreamUtility::write(_stream_,_entry_->generationNumber);
	StreamUtility::write(_stream_," ");

	_stream_->write(&_entry_->flag,1);

	/*
	 * Nach der PDF Dokumentation:
	 *
	 * PDF Reference, sixth edition, Adobe® Portable Document Format, Version 1.7, 
	 * November 2006, Seite 94, Abschn. 3.4.3 Cross-Reference Table
	 *
	 *  "eol is a 2-character end-of-line sequence"
	 *
	 * Ist am Ende eines jeden Eintrages in der Referenztabelle ein zwei Zeichen umfassendes 
	 * Zeilenende Kennzeichen.
	 */
	StreamUtility::write(_stream_,"\r\n");
}

void		XRef::write(OutputStream* _file_)
{
/*	if(!_file_->good())
		throw IOError(__FILE__,__LINE__,
				"Referenztabelle kann nicht geschrieben werden");
*/
	pdflong _startxref_=_file_->getPosition();

	if(0 == fXRefMap->getEntry(0) )
	{
		XRefEntry _entry_;
		memset(&_entry_,0,sizeof(XRefEntry));
		_entry_.objectNumber=0;
		_entry_.generationNumber=65535;
		_entry_.flag='f';
		_entry_.offset=0;

		addEntry(_entry_);
	}

	_file_->write("xref\n",string("xref\n").length());
	fXRefMap->write( _file_ );

	if(trailer == NULL)
		throw internalError(__FILE__,__LINE__,"XRef hat keinen Trailer");

	if(!trailer->isDictionaryObject())
		throw internalError(__FILE__,__LINE__,"Trailer ist kein Woerterbuch");

	if(trailer->getDictionary()->lookUp("/Root")==NULL)
		throw internalError(__FILE__,__LINE__,"/Root fehlt im Trailer");


	trailer->getDictionary_M()->set("/Size",getGreatestObjectNumber()+1);

	if(fICOMMetadataObjNum > 0){
		trailer->getDictionary_M()->set("/ICOMMetadata",fICOMMetadataObjNum,0);
	}

	ICF_uint8* uuid = 0;
	int len = ICOM::Framework::ICOMFramework::fgUUID->create(&uuid);
	char* id = ICOM::Framework::UUID::uuidToChar(uuid, len, true);
	std::string idstr = std::string(id);

	Object* objidstr1 = new Object();
	objidstr1->initHexStringObject(this, idstr);

	Object* objidstr2 = new Object();
	objidstr2->initHexStringObject(this, idstr);

	Object* arr = new Object();
	arr->initArrayObject(this);
	arr->getArray_M()->push_back(objidstr1);
	arr->getArray_M()->push_back(objidstr2);

	trailer->getDictionary_M()->set("/ID", arr);

	StreamUtility::write(_file_,"trailer\n");
	trailer->write(_file_);
	StreamUtility::write(_file_,"\nstartxref\n");
	StreamUtility::write(_file_,_startxref_);
	StreamUtility::write(_file_,"\n%%EOF");
}

pdflong		XRef::size() const
{
	if(prev)
		return fXRefMap->getTotalSize() + prev->size();
	else
		return fXRefMap->getTotalSize();
}

void		XRef::findStart(InputStream* _stream_)
{
	if(_stream_==NULL)
		throw IOError(__FILE__,__LINE__,"Kein Datenstrom definiert");

	_stream_->setPosition(0);

	while(_stream_->good())
	{
		if(StreamUtility::findNext(_stream_," obj"))
			break;
	}
	if(!_stream_->good())
	{
		PDFErrorHandlerFatal1(
			fErrorHandler,
			PDFCoreException,
			ErrorCodes::ERROR_INVALID_STREAM,
			_stream_->getURI()
		);
	}

	_stream_->setPosition(_stream_->getPosition()+4);

	Object* _firstObject_=new Object();
	try{
		Parser	p(_stream_);
		p.getObject(_firstObject_,NULL,this);
		const Object* _linearObject_=NULL;
		if(_firstObject_->isDictionaryObject())
			_linearObject_=_firstObject_->getDictionary()->lookUp("/Linearized");
		if(_linearObject_)
		{
			const Object* _L_=_firstObject_->getDictionary()->lookUp("/L");
			if(_L_->getInteger()==(pdflong)_stream_->getLength())
			{
				if(_firstObject_) delete _firstObject_;
				_setLinearizedObject(1);				
				return;
			}
		}
	}catch(Exception&){}
	if(_firstObject_) delete _firstObject_;

	pdfulong bytes_go_back = 4096;
	bool stratxref_found = false;
	while( !stratxref_found && _stream_->getLength() > bytes_go_back ){
		_stream_->setPosition(_stream_->getLength() - bytes_go_back);
		stratxref_found = StreamUtility::findNext(_stream_,"startxref");
		bytes_go_back += 4096;

		/*
		 * Ist das Schlüsselwort nicht in den letzten 4 MB zu finden, dann wird die Schleife beendet
		 * Bei großen PDF-Dateien, wo das Schlüsselwort stratxref fehlt, dauert das Suchen sonst ewig
		 */
		if (bytes_go_back > 4 * 1024 * 1024) {
			break;
		}
	}
	if(!stratxref_found){
		_stream_->setPosition(0);
		stratxref_found = StreamUtility::findNext(_stream_,"startxref");
		bytes_go_back += 4096;
	}
	if(!stratxref_found)
	{
		PDFErrorHandlerFatal1(
			fErrorHandler,
			PDFCoreException,
			ErrorCodes::ERROR_KEYWORD_STARTXREF_NOT_FOUND,
			_stream_->getURI()
		);
	}

	pdflong	xrefoffset = _stream_->getPosition();
	_stream_->get();
	while(StreamUtility::findNext(_stream_,"startxref") && _stream_->good()){
		xrefoffset = _stream_->getPosition();
		_stream_->get();
	}
	_stream_->setPosition(xrefoffset);

	char _offset_[128];
	memset(_offset_,0,128);
	if(_stream_->read(_offset_,128)<128)
		_stream_->clear();
	vector<string> _tmp_=lexer::divide(string(_offset_));
	for(unsigned int i=0;i<_tmp_.size();i++)
	{
		if(_tmp_[i]=="startxref")
		{
			if(i+1<_tmp_.size())
			{
				_stream_->clear();
				pdfulong offset = (pdfulong)pdf_atol(_tmp_[i+1].c_str());
				if(offset >= _stream_->getLength()){
					PDFErrorHandlerFatal2(
						fErrorHandler,
						PDFCoreException,
						ErrorCodes::ERROR_XREF_POS_TOO_LARGE,
						_stream_->getURI(),
						_tmp_[i+1].c_str()
					);
				}
				_stream_->setPosition(offset);

				/*
				 *  Gehe zurueck zum Anfang des Token,
				 *  falls man sich mittendrin befindet.
				 */
				while(_stream_->good())
				{
					_stream_->setPosition(_stream_->getPosition()-1);
					if( lexer::isWhitespace(_stream_->peek()) )
					{
						_stream_->get();
						break;
					}
				}
				return;
			}
		}
	}
	throw IOError(__FILE__,__LINE__,"Kann Referenztabelle nicht finden");
}

bool		XRef::getObject(Object* _obj_,pdflong _objNum_) const
{
	if(_obj_==NULL)
		throw internalError(__FILE__,__LINE__,"Nullobjekt empfangen.");

	if(stream==NULL){
		PDFErrorHandlerFatal(
				fErrorHandler,
				PDFCoreException,
				ErrorCodes::ERROR_XREF_STREAM_NOT_SET
			);
	}

	pdflong _curPos_=stream->getPosition();

	const XRefEntry* _xrefentry_ = getEntry(_objNum_);
	if(_xrefentry_ == NULL)
		return false;

	if(_xrefentry_->objStm.objectNumber==0)
	{		
		pdflong _offset_=_xrefentry_->offset;
		if(_xrefentry_->flag=='f')
			_offset_ = 0;
		if(_offset_==0)
		{
			_obj_->initNullObject();
			return false;
		}

		stream->setPosition(_offset_);
		if (!stream->good())
		{
			std::string msg = "Beim springen an Position ";
			msg += PDFString::toString(_offset_);
			msg += " wurde das Dateihandle beschaedigt.";
			throw IOError(__FILE__, __LINE__, msg);
		}
		while (!lexer::isWhitespace(stream->peek()) && _offset_ > 0) {
			_offset_ = _offset_ - 1;
			stream->setPosition(_offset_);
			if (!stream->good())
			{
				std::string msg = "Beim springen an Position ";
				msg += PDFString::toString(_offset_);
				msg += " wurde das Dateihandle beschaedigt.";
				throw IOError(__FILE__, __LINE__, msg);
			}
		}
		
		while(lexer::isWhitespace(stream->peek())) stream->get();
		vector<string> _div_=lexer::divide(lexer::getLine(stream));

		if(_div_.size()==1)
		{
			while(lexer::isWhitespace(stream->peek())) stream->get();
			std::string objtag;
			while(!lexer::isWhitespace(stream->peek())) 
				objtag+=stream->get();
			_div_.push_back(objtag);
		}

		if(_div_.size()==2)
		{
			while(lexer::isWhitespace(stream->peek())) stream->get();
			std::string objtag;
			while(!lexer::isWhitespace(stream->peek())) 
				objtag+=stream->get();
			_div_.push_back(objtag);
		}

		if(_div_.size()<3)
		{
			std::string msg = "Eintrag in der Referenztabelle zum Objekt ";
			msg += PDFString::toString(_objNum_);
			msg += " verweist nicht auf das Objekt.";
			throw IOError(__FILE__,__LINE__,msg);
		}

		if(_div_[2].substr(0,3)!="obj"){
			PDFErrorHandlerFatal2(
				fErrorHandler,
				PDFCoreException,
				ErrorCodes::ERROR_XREF_OBJ_EXPECTED,
				stream->getURI(),
				PDFString::toString(_offset_).c_str()

			);
		}

		/*
		 * Extrahiere nur die letzten Ziffern zb %%EOF11 -> 11 
		 */
		for (int i = _div_[0].size() - 1; i >= 0; i--) {
			if ('0' <= _div_[0][i] && _div_[0][i] <= '9') {

			}
			else if(i < _div_[0].size() - 1){

				for (int j = 0; j < _div_[0].size() - i; j++) {
					_div_[0][j] = _div_[0][i + j + 1];
				}
				_div_[0][_div_[0].size() - i] = 0;
				break;
			}
			else {
				break;
			}
		}

		if(atoi(_div_[0].c_str())!=_objNum_)
			throw IOError(__FILE__,__LINE__,
				"Eintrag in Referenztabelle verweist nicht auf das angegebene Objekt.");

		stream->setPosition(_offset_);
		while(stream->get()!='j' && stream->good());

		Parser p(stream);
		p.getObject(_obj_,NULL,this);
		stream->setPosition(_curPos_);
	}else
	{
		Object* _objStm_ = new Object();
		if(!getObject(_objStm_,_xrefentry_->objStm.objectNumber))
		{
			throw syntaxError(__FILE__,__LINE__,"Objekt wurde nicht gefunden");
		}
		if(_objStm_->getStream()->getDictionary()->lookUp("/Type") == NULL)
			throw syntaxError(__FILE__,__LINE__,"/Typ Eintrag fehlt.");

		if(_objStm_->getStream()->getDictionary()->lookUp("/Type")->getName()!="/ObjStm")
			throw syntaxError(__FILE__,__LINE__,"Falscher Objekttyp.");

		if(_objStm_->getStream()->getDictionary()->lookUp("/N") == NULL)
			throw syntaxError(__FILE__,__LINE__,"/N Eintrag fehlt.");

		if(_objStm_->getStream()->getDictionary()->lookUp("/First") == NULL)
			throw syntaxError(__FILE__,__LINE__,"/First Eintrag fehlt.");



		pdflong _N_ = _objStm_->getStream()->getDictionary()->lookUp("/N")->getInteger();
		pdflong _first_ = _objStm_->getStream()->getDictionary()->lookUp("/First")->getInteger();


		MemoryStream	content;
		StreamFilterD filter;
		_objStm_->getStream_M()->write(&content,&filter);
		content.setPosition(0);
		getObject(_obj_,_objNum_,&content ,_N_,_first_);
		delete _objStm_;
	}

	return true;
}

bool		XRef::getObject(Object* _obj_,pdflong _objNum_,InputStream* _stream_,pdflong _n_,pdflong _first_) const
{
	string	_num_;
	string	_offset_;
	while(_stream_->good() && pdf_atol(_num_.c_str())!=_objNum_)
	{
		_num_ = "";
		_offset_ = "";
		while(lexer::isWhitespace(_stream_->peek()) && _stream_->good()) _stream_->get();
		while(lexer::isNumber(_stream_->peek()) && _stream_->good()) _num_+=_stream_->get();
		while(lexer::isWhitespace(_stream_->peek()) && _stream_->good()) _stream_->get();
		while(lexer::isNumber(_stream_->peek()) && _stream_->good()) _offset_+=_stream_->get();
	}

	_stream_->setPosition(_first_+pdf_atol(_offset_.c_str()));
	Parser p(_stream_);
	p.getObject(_obj_,NULL,this);

	return true;
}

void		XRef::setPrev(pdflong _offset_)
{
	Object* _obj_=new Object();
	_obj_->initIntegerObject(this,_offset_);

	if(trailer==NULL)
	{
		trailer=new Object();
		trailer->initDictionaryObject(this);
	}
	trailer->getDictionary_M()->set("/Prev",_obj_);
}

void		XRef::setRoot(pdflong _objNum_,pdflong _genNum_)
{
	Object* _obj_=new Object();
	_obj_->initIndirectObject(this,_objNum_,_genNum_);

	if(trailer==NULL)
	{
		trailer=new Object();
		trailer->initDictionaryObject(this);
	}
	trailer->getDictionary_M()->set("/Root",_obj_);
}

void	XRef::setICOMMetadataObjNum(pdflong objnum)
{
	fICOMMetadataObjNum = objnum;
}

void		XRef::parse_xref_stream(InputStream* _stream_, PDFValidatorHandlerAbstract* validator)
{
	pdfulong pos = _stream_->getPosition();
	while(_stream_->peek()!='j' && _stream_->good())
		_stream_->get();
	if(!_stream_->good())
	{
		std::string msg = "Fehler beim einlesen der Referenztabelle an Position ";
		msg += PDFString::toString(pos);
		msg+=".";
		throw IOError(__FILE__,__LINE__,msg);
	}
	_stream_->get();
	Object* _obj_=new Object();
	Parser p(_stream_);
	p.getObject(_obj_,NULL,this);

	MemoryStream xrefStream;
	StreamFilterD filter;
	_obj_->getStream_M()->write(&xrefStream,&filter);
	xrefStream.setPosition(0);
	//_xrefStream_->clear();
	parseXRefStream(&xrefStream,_obj_->getStream()->getDictionary(), validator);
	delete _obj_;
}

void		XRef::parseXRefStream(InputStream* _stream_,const Dictionary* _dictionary_, PDFValidatorHandlerAbstract* validator)
{
	if(_stream_ == NULL){
        PDFErrorHandlerFatal(fErrorHandler,PDFCoreException,ErrorCodes::ERROR_NULLOBJECT_RECEIVED);
	}

	if(_dictionary_ == NULL){
        PDFErrorHandlerFatal(fErrorHandler,PDFCoreException,ErrorCodes::ERROR_NULLOBJECT_RECEIVED);
	}

	if(_dictionary_->lookUp("/Type") == NULL)
		throw syntaxError(__FILE__,__LINE__,"Objekt hat kein Typ");

	if(_dictionary_->lookUp("/Type")->getName() != "/XRef")
		throw syntaxError(__FILE__,__LINE__,"Objekt ist nicht vom Typ /XRef.");

	if(_dictionary_->lookUp("/Encrypt")){

		if (validator) {
			validator->setEncrypted(true);
		}

		/*
		 * Achtung: Als Parameter muss stream->getURI() stehen, statt _stream_->getURI(), da
		 * _stream_ auch ein MemoryStream sein kann und somit keine Referenz auf die Eingabedatei 
		 * existiert.
		 */
		PDFErrorHandlerFatal1(
			fErrorHandler,
			PDFCoreException,
			ErrorCodes::ERROR_DOCUMENT_ENCRYPTED,
			stream->getURI()
		);
	}

	if (validator) {
		validator->setEncrypted(false);
	}

	trailer = new Object();
	trailer->initDictionaryObject(this);

	pdflong _size_ = 0;

	if(_dictionary_->lookUp("/Size") == NULL)
		throw syntaxError(__FILE__,__LINE__,"/Size fehlt im Objekt");

	_size_ = _dictionary_->lookUp("/Size")->getInteger();
	trailer->getDictionary_M()->push_back("/Size",_size_);

	if(_dictionary_->lookUp("/Prev") != NULL)
	{
		trailer->getDictionary_M()->push_back("/Prev",_dictionary_->lookUp("/Prev")->getInteger());
	}

	if(_dictionary_->lookUp("/Root") != NULL)
	{
		Object* _root_ = new Object();
		_root_->initIndirectObject(this,_dictionary_->lookUp("/Root")->getIndirectObject()->objectNumber,
			_dictionary_->lookUp("/Root")->getIndirectObject()->generationNumber);
		trailer->getDictionary_M()->push_back("/Root",_root_);
	}

	if(_dictionary_->lookUp("/Info") != NULL)
	{
		Object* _info_ = new Object();
		_info_->initIndirectObject(this,_dictionary_->lookUp("/Info")->getIndirectObject()->objectNumber,
			_dictionary_->lookUp("/Info")->getIndirectObject()->generationNumber);
		trailer->getDictionary_M()->push_back("/Info",_info_);
	}

	if(_dictionary_->lookUp("/ID") != NULL)
	{
		Object* _id_ = new Object();
		_id_->initArrayObject(this);
		for(unsigned int i=0;i<_dictionary_->lookUp("/ID")->getArray()->size();i++)
		{
			Object* _tmp_ = new Object();
			_tmp_->initStringObject(this,_dictionary_->lookUp("/ID")->getArray()->operator [](i)->getString());
			_id_->getArray_M()->push_back(_tmp_);
		}
		trailer->getDictionary_M()->push_back("/ID",_id_);
	}

	vector<pdflong>	_objNum_;
	vector<pdflong>	_refSize_;
	pdflong _currentObjectNumber_ = 0;
	if(_dictionary_->lookUp("/Index") != NULL)
	{
		if(!_dictionary_->lookUp("/Index")->isArrayObject())
			throw syntaxError(__FILE__,__LINE__,"/Index ist kein Array");

		const Array* _indexArray_ = _dictionary_->lookUp("/Index")->getArray();
		for(unsigned int i=0;i<_indexArray_->size();)
		{
			_objNum_.push_back(_indexArray_->operator [](i++)->getInteger());
			_refSize_.push_back(_indexArray_->operator [](i++)->getInteger());
		}
		_currentObjectNumber_ = _objNum_[0];
	}else
		_currentObjectNumber_ = 0;

	if(_dictionary_->lookUp("/W") == NULL)
		throw syntaxError(__FILE__,__LINE__,"/W fehlt im Objekt");

	const Array* _w_ = _dictionary_->lookUp("/W")->getArray();
	if(_w_->size()!=3)
		throw internalError(__FILE__,__LINE__,"/W hat nicht genau 3 Eintraege.");

	pdflong _chunkSize_[3];
	_chunkSize_[0] = _w_->operator [](0)->getInteger();
	_chunkSize_[1] = _w_->operator [](1)->getInteger();
	_chunkSize_[2] = _w_->operator [](2)->getInteger();

	vector< vector<unsigned char> >	_chunks_;
	vector<unsigned char>			_tmp_;

	pdflong nEntries = 0;
	pdflong currentIndex = 0;
	while(_stream_->good())
	{
		if( !_refSize_.empty())
		{
			if(nEntries>=_refSize_[(int)currentIndex])
			{
				if (currentIndex < _refSize_.size() - 1) {
					_currentObjectNumber_ = _objNum_[(int)++currentIndex];
				}
				else {
					return;
				}
				nEntries=0;
			}
		}
		_chunks_.clear();
		for(int i=0;i<3;i++)
		{
			_tmp_.clear();
			for(int j=0;j<_chunkSize_[i];j++)
			{
				_tmp_.push_back(_stream_->get());
			}
			_chunks_.push_back(_tmp_);
		}
		interpretXRefStreamEntry(_chunks_,_currentObjectNumber_++);
		nEntries++;
	}
}

void		XRef::interpretXRefStreamEntry(const vector< vector<unsigned char> >& _entry_,pdflong _objNum_)
{
	if(_entry_[0].size()!=1)
		throw syntaxError(__FILE__,__LINE__,"Typ-Eintrag im XRef-Stream hat falsche Groesse.");

	XRefEntry	_objEntry_;
	memset(&_objEntry_,0,sizeof(_objEntry_));
	switch(_entry_[0][0])
	{
	case 0:
		_objEntry_.objectNumber=hexToDec(_entry_[1]);
		_objEntry_.generationNumber=hexToDec(_entry_[2]);
		_objEntry_.flag = 'f';
		_objEntry_.offset = 0;
		addEntry(_objEntry_);
		break;
	case 1:
		_objEntry_.objectNumber=_objNum_;
		_objEntry_.generationNumber=hexToDec(_entry_[2]);
		_objEntry_.flag = 'n';
		_objEntry_.offset = hexToDec(_entry_[1]);
		addEntry(_objEntry_);
		break;
	case 2:
		_objEntry_.objectNumber=_objNum_;
		_objEntry_.generationNumber=0;
		_objEntry_.flag = 'n';
		_objEntry_.offset = 0;
		_objEntry_.objStm.objectNumber = hexToDec(_entry_[1]);
		_objEntry_.objStm.index = hexToDec(_entry_[2]);
		addEntry(_objEntry_);
		break;
	default:
		throw syntaxError(__FILE__,__LINE__,"unbekannter Typ im XRef-Stream.");
	}
}

pdflong		XRef::hexToDec(const vector<unsigned char>& _hex_)
{
	pdflong _ret_ = 0;
	for(unsigned int i=0;i<_hex_.size();i++)
		_ret_=_ret_*256+(unsigned int)_hex_[i];

	return _ret_;
}

void		XRef::checkTrailer(Object* _trailer_, PDFValidatorHandlerAbstract* validator)
{
	if(_trailer_->getDictionary()->lookUp("/Encrypt")){

		if (validator) {
			validator->setEncrypted(true);
		}

		PDFErrorHandlerFatal1(
			fErrorHandler,
			PDFCoreException,
			ErrorCodes::ERROR_DOCUMENT_ENCRYPTED,
			stream->getURI()
		);
	}

	if (validator) {
		validator->setEncrypted(false);
	}
}

void		XRef::getRootPages(Object* _obj_) const
{
	if(_obj_ == NULL) throw nullPointer(__FILE__,__LINE__);
	
	const Object* _root_=getRoot();
	if(_root_->isIndirectObject())
	{
		Object* _directRoot_ = new Object();
		getObject(_directRoot_,_root_->getIndirectObject()->objectNumber);
		if(!_directRoot_->isDictionaryObject())
			throw syntaxError(__FILE__,__LINE__,"Root ist kein Woerterbuch");
		if(_directRoot_->getDictionary()->lookUp("/Pages") == NULL)
			throw syntaxError(__FILE__,__LINE__,"Page-Root existiert nicht");
		if(!_directRoot_->getDictionary()->lookUp("/Pages")->isIndirectObject())
			throw syntaxError(__FILE__,__LINE__,"Page-Root muss ein Indirektes Object sein.");
		getObject(_obj_,_directRoot_->getDictionary()->lookUp("/Pages")->getIndirectObject()->objectNumber);
		delete _directRoot_;
	}else
		throw syntaxError(__FILE__,__LINE__,"/Root ist kein indirektes Objekt");
}

IndirectObject		XRef::getRootPages() const
{
	IndirectObject	_ret_;
	const Object* _root_=getRoot();
	if(_root_->isIndirectObject())
	{
		Object* _directRoot_ = new Object();
		getObject(_directRoot_,_root_->getIndirectObject()->objectNumber);
		if(!_directRoot_->isDictionaryObject())
			throw syntaxError(__FILE__,__LINE__,"Root ist kein Woerterbuch");
		if(_directRoot_->getDictionary()->lookUp("/Pages") == NULL)
			throw syntaxError(__FILE__,__LINE__,"Page-Root existiert nicht");
		if(!_directRoot_->getDictionary()->lookUp("/Pages")->isIndirectObject())
			throw syntaxError(__FILE__,__LINE__,"Page-Root muss ein Indirektes Object sein.");
		_ret_ = *(_directRoot_->getDictionary()->lookUp("/Pages")->getIndirectObject() );
		delete _directRoot_;
		return _ret_;
	}else
		throw internalError(__FILE__,__LINE__,"/Root ist kein indirektes Objekt");
}

const Object*		XRef::getTrailer() const
{
	if(trailer == NULL)
		throw internalError(__FILE__,__LINE__,"Trailer ist Nullobjekt");

	return trailer;
}

Object*		XRef::getTrailer_M() const
{
	if(trailer == NULL)
		throw internalError(__FILE__,__LINE__,"Trailer ist Nullobjekt");

	return trailer;
}

void		XRef::cacheFont(pdfulong objNum,pdfulong genNum,Object* obj)
{
	if(fonts.find(objNum) == fonts.end())
		fonts[objNum] = new Font(obj,objNum,genNum);
}

const Font*	XRef::getFont(pdfulong objNum) const
{
	std::map<pdfulong,Font*>::const_iterator it;
	it=fonts.find(objNum);
	if(it==fonts.end())
		return NULL;

	return it->second;
}

bool		XRef::addEntry(pdflong objnum,pdflong gennum,pdflong offset)
{
	bool ret = false;
	XRefEntry entry;
	memset(&entry,0,sizeof(XRefEntry));
	entry.objectNumber = objnum;
	entry.generationNumber = gennum;
	entry.offset = offset;
	entry.flag = 'n';
	ret = fXRefMap->pushBack( entry );

	if(greatestObjectnumber<entry.objectNumber)
		greatestObjectnumber=entry.objectNumber;

	if( entry.objectNumber == 0)
		return true;

	if(ret == false)
		throw IOError(__FILE__,__LINE__,"Doppeltes Objekt in der Referenztabelle gefunden.");

	return ret;
}

void		XRef::setOffset(pdflong objnum,pdflong gennum,pdflong offset)
{
	XRefEntry* entry = fXRefMap->getEntry( objnum );
	if(0 == entry)
	{
		prev->setOffset(objnum,gennum,offset);
		return;
	}

	entry->offset = offset;
}

pdflong		XRef::getICOMMetadataObjNum()
{
	if(this->getTrailer()){
		const Object* obj = this->getTrailer()->getDictionary()->lookUp("/ICOMMetadata");
		if(obj){
			return obj->getIndirectObject()->objectNumber;
		}
	}
	return 0;
}

void	XRef::setAllowSubsections(bool val)
{
	fXRefMap->setWriteSubsections( val );
	if( prev ){
		prev->setAllowSubsections( val );
	}
}

 
