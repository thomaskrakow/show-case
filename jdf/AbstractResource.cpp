

#include "AbstractResource.h"
#include "ICOMJDFException.h"
#include "ICOMJDFErrorCodes.h"
#include "JDFKeywords.h"
#include "ResourceLink.h"
#include "ResourcePool.h"
#include "JDFNode.h"

#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/Janitor.hpp>

using namespace ICOM::JDF;

AbstractResource::AbstractResource(Class cl,AbstractResourcePool* resourcePool) :
	fClass(cl),
	fID(0),
	fStatus(Status_Available), /* Ressourcen sind immer verf�gbar */
	AbstractResourcePool(AbstractResourcePool::Type_Resource,resourcePool),
	fPipeProtocol(PipeProtocol_NotSet),
	fRefCount(0)
{
	assert(resourcePool);
}



AbstractResource::~AbstractResource()
{
	xercesc::XMLString::release( &fID );
}


void	AbstractResource::OnExtendDOM(xercesc::DOMDocument* document,xercesc::DOMElement* element)
{
	assert( document );
	assert( element );

	AbstractResourcePool::OnExtendDOM(document,element);
	
	/*
	 * Für ein genaueres Verständnis dieser Bedingung siehe Kommentar in der Funktion
	 * AbstractResourcePool::OnExtendDOM
	 */
	if( dynamic_cast<ResourcePool*>(fParent) && fRefCount != 1 ){

		/*
		 * Das Attribut Class soll nur im Wurzelknoten der Ressourcen gesetzt werden.
		 */
		switch( fClass ){
		case Class_Parameter:
			element->setAttribute( JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_CLASS), JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_CLASS_VALUE_PARAMETER) );
			break;
		case Class_Quantity:
			element->setAttribute( JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_CLASS), JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_CLASS_VALUE_QUANTITY) );
			break;
		case Class_Intent:
			element->setAttribute( JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_CLASS), JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_CLASS_VALUE_INTENT) );
			break;
		case Class_Consumable:
			element->setAttribute( JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_CLASS), JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_CLASS_VALUE_CONSUMABLE) );
			break;
		default:
			assert(false);
			break;
		}


		/*
		 * Attribut ID soll nur im Wurzelknoten definiert sein.
		 */
		element->setAttribute( JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_ID), this->getID() );

		/*
		 * Attribut Status soll nur im Wurzelknoten definiert sein (Kann aber auch im Blattknoten definiert werden).
		 */
		switch(fStatus){
		case Status_Available:
			element->setAttribute( JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_ABSTRACTRESOURCE_STATUS), JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_ABSTRACTRESOURCE_STATUS_AVAILABLE) );
			break;
		case Status_Unavailable:
			element->setAttribute( JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_ABSTRACTRESOURCE_STATUS), JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_ABSTRACTRESOURCE_STATUS_UNAVAILABLE) );
			break;
		default:
			break;
		}
	}

	switch(fPipeProtocol){
	case PipeProtocol_NotSet:
		break;
	case PipeProtocol_Internal:
		element->setAttribute( JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_PIPEPROTOCOL), JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_PIPEPROTOCOL_VALUE_INTERNAL) );
		break;
	case PipeProtocol_JMF:
		element->setAttribute( JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_PIPEPROTOCOL), JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_PIPEPROTOCOL_VALUE_JMF) );
		break;
	case PipeProtocol_JMFPush:
		element->setAttribute( JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_PIPEPROTOCOL), JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_PIPEPROTOCOL_VALUE_JMFPUSH) );
		break;
	case PipeProtocol_JMFPull:
		element->setAttribute( JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_PIPEPROTOCOL), JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_PIPEPROTOCOL_VALUE_JMFPULL) );
		break;
	case PipeProtocol_None:
		element->setAttribute( JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_PIPEPROTOCOL), JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_PIPEPROTOCOL_VALUE_NONE) );
		break;
	default:
		assert(false);
		break;
	}

	/*
	 * Ist die Ressource eine Pipe, dann müssen wir auch eine PipeID vergeben.
	 */
	if( fPipeProtocol != PipeProtocol_NotSet ){
		XMLCh* pipeID = JDFNode::createID();
		element->setAttribute( JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_PIPEID), pipeID );
		xercesc::XMLString::release( &pipeID );
	}

}

const XMLCh*	AbstractResource::getID()
{
	if( fID == 0 ){
		fID = JDFNode::createID();
	}
	return fID;
}

void	AbstractResource::setStatus(Status status)
{
	fStatus = status;
}

void	AbstractResource::setPipeProtocol(PipeProtocol pipeProtocol)
{
	fPipeProtocol = pipeProtocol;
}
