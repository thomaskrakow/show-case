

#include "JDFURI.h"
#include "ICOMJDFException.h"
#include "ICOMJDFErrorCodes.h"
#include "JDFKeywords.h"
#include "ICOMMacros.h"

#include <xercesc/util/XMLString.hpp>
#include <xercesc/validators/datatype/AnyURIDatatypeValidator.hpp>

using namespace ICOM::JDF;

JDFURI::JDFURI(const char* path,Sheme sheme) :
	fSheme(sheme)
{
	XMLCh* pathW = xercesc::XMLString::transcode( path );
	xercesc::Janitor<XMLCh> jan_pathW(pathW);

	XMLSize_t len = xercesc::XMLString::stringLen(pathW);
	for(XMLSize_t i = 0; i < len;i++ ){

		/*
		 * \ werden durch / ersetzt.
		 */
		if( *(pathW + i) == 0x005C ){
			*(pathW + i) = 0x002F;
		}
	}

	switch(sheme){
	case Sheme_File:
		len += xercesc::XMLString::stringLen( JDFKeywords::getJDFKeyword(JDFKeywords::ID_URI_SCHEME_FILE) );
		break;
	default:
		assert(false);
		break;
	}

	len += 2;
	fURI = (XMLCh*)ICOM::Framework::ICOMFramework::fgMemoryManager->allocate( len * sizeof(XMLCh) );
	memset(fURI,0,len * sizeof(XMLCh));
	xercesc::XMLString::catString(fURI,JDFKeywords::getJDFKeyword(JDFKeywords::ID_URI_SCHEME_FILE));

	XMLCh slash[] = {0x002F,0x0000};
	xercesc::XMLString::catString(fURI,slash);

	xercesc::XMLString::catString(fURI,pathW);


	xercesc::AnyURIDatatypeValidator* validatorAnyURI = new xercesc::AnyURIDatatypeValidator;
	xercesc::Janitor<xercesc::AnyURIDatatypeValidator> jan_validatorAnyURI(validatorAnyURI);
	try{
		validatorAnyURI->validate(fURI);
	}catch(xercesc::XMLException& exc){

		const char* uri = xercesc::XMLString::transcode(fURI);
		xercesc::Janitor<const char> jan_uri(uri);

		const char* msg = xercesc::XMLString::transcode(exc.getMessage());
		xercesc::Janitor<const char> jan_msg(msg);

		ThrowICOMException2(ICOMJDFErrorCodes::ERROR_MALFORMED_URI,uri,msg);
	}	
}

JDFURI::JDFURI() :
	fURI(0),
	fSheme(Sheme_File)
{
	
}

JDFURI::~JDFURI()
{
	ICOM_DELETE_VARIABLE(fURI);
}

const XMLCh*	JDFURI::getURI() const
{
	return fURI;
}

JDFURI*		JDFURI::duplicate() const
{
	JDFURI* uri = new JDFURI;
	int len = xercesc::XMLString::stringLen(fURI);
	uri->fURI = (XMLCh*)ICOM::Framework::ICOMFramework::fgMemoryManager->allocate( (len + 1) * sizeof(XMLCh) );
	memset(uri->fURI,0,(len + 1) * sizeof(XMLCh));
	memcpy(uri->fURI,fURI,len * sizeof(XMLCh));
	uri->fSheme = fSheme;
	return uri;
}
