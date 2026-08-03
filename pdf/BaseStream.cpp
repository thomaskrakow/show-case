#include "BaseStream.h"

#include "Exception.h"
#include "stream/MemoryStream.h"
#include "stream/StreamUtility.h"

#include "filter/BaseFilter.h"
#include "filter/FlateFilterD.h"
#include "Janitor.h"
using namespace ICOM::pdflib;

#include "zlib.h"
#include "iclib.h"

#include <assert.h>

#include "allocator/pdflib_allocator.h"
#define new PDFLIB_ALLOCATOR

BaseStream::BaseStream(const XRef* _xref_)
{
	dictionary=new Dictionary(_xref_);
	fData=new MemoryStream();
	fXRef=_xref_;
	filter=NULL;
	setLength(-1);
}

BaseStream::~BaseStream()
{
	if(dictionary)
		delete dictionary;

	if(fData)
		delete fData;
}

void BaseStream::write(OutputStream* outputStream)
{
	MemoryStream* buffer = new MemoryStream();
	ICOM::Framework::Janitor<MemoryStream> jan__buf_(buffer);
	write(buffer, filter);
	buffer->setPosition(0);

	dictionary->set("/Length",buffer->getLength());
	dictionary->write(outputStream);
	StreamUtility::write(outputStream,"\nstream\n");
	StreamUtility::write(outputStream,buffer);
	StreamUtility::write(outputStream,"\nendstream");
}

void	BaseStream::readRawData(InputStream* _stream_,pdfulong _getLength_)
{
	if(_stream_==NULL)
		throw IOError(__FILE__,__LINE__,"Ungueltiger Datenstrom");

	if(_getLength_<0)
		throw IOError(__FILE__,__LINE__,"Ungueltige Datenstromlaenge");

	fData->clear();

	char* _buf_=new char[(unsigned int)(_getLength_)*sizeof(char)];
	pdflong _ret_=_stream_->read(_buf_,_getLength_);
	fData->write(_buf_,_ret_);
	setLength(_ret_);
	fData->setPosition(0);
	delete[] _buf_;
}

pdflong	BaseStream::getLength()
{
	if(length<0)
	{
		const Object* _lenObj_=dictionary->lookUp("/Length");
		if(_lenObj_==NULL)
			throw syntaxError(__FILE__,__LINE__,"Schluesselwort /Length nicht im Woerterbuch vorhanden");

		if(_lenObj_->isIndirectObject())
		{
			Object* _next_=new Object();
			if(fXRef==NULL)
			{
				delete _next_;
				throw IOError(__FILE__,__LINE__,"Keine Referenztabelle eingelesen.");
			}
			if(!fXRef->getObject(_next_,_lenObj_->getIndirectObject()->objectNumber))
			{
				delete _next_;
				throw IOError(__FILE__,__LINE__,"Objekt gibt es nicht");
			}

			if(!_next_->isIntegerObject())
			{
				delete _next_;
				throw IOError(__FILE__,__LINE__,
					"Eine indirekte Referenz verweist auf eine weitere indirekte Referenz");
			}
			setLength(_next_->getInteger());
			delete _next_;
		}else
		{
			setLength(_lenObj_->getInteger());
		}
	}

	return length;
}

void	BaseStream::write(OutputStream* _stream_,BaseFilter* _filter_)
{
	assert(_stream_ != NULL);

	//fData->clear();
	fData->setPosition(0);

	if(_filter_)
		_filter_->apply(fData,_stream_,dictionary);
	else
		StreamUtility::write(_stream_,fData);
}

void	BaseStream::copy(Object* _dst_) const
{
	_dst_->initStreamObject( getDictionary()->getXRef() );
	getDictionary()->copy(_dst_);
	fData->setPosition(0);
	fData->clear();
	_dst_->getStream_M()->readRawData(fData,fData->getLength());
}

std::string		BaseStream::getMD5Checksum(unsigned char* md5)
{
	iclib_md5_ctx ctx;
	unsigned char	buffer[4096] ={0};

	fData->rewind();
	pdfulong length = fData->read(reinterpret_cast<char*>(buffer),4096);

	iclib_md5init (&ctx);
	while(length > 0)
	{		
		iclib_md5update (&ctx, buffer, (unsigned int)length);
		length = fData->read(reinterpret_cast<char*>(buffer),4096);
	}
	iclib_md5final (&ctx);
	fData->rewind();

	unsigned char md5Dest[16] = {0};
	unsigned char md5Char[33] = {0};

	memcpy(md5,ctx.digest,16);
	iclib_char_to_hex(reinterpret_cast<char*>(md5Char),md5,16,ICLIB_FALSE);
	return string(reinterpret_cast<char*>(md5Char));
}

 
