

#include "CombinedProcess.h"
#include "ICOMJDFException.h"
#include "ICOMJDFErrorCodes.h"
#include "JDFKeywords.h"
#include "Janitor.h"
#include "JDFProcess.h"

#include <xercesc/util/Janitor.hpp>


using namespace ICOM::JDF;

CombinedProcess::CombinedProcess(JDFNode* parent) :
	JDFNode(parent),
	fLastProcess(0)
{
	fResourcePool.setType( ResourcePool::Type_CombinedProcess );
	this->setType( JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_JDF_TYPE_VALUE_COMBINED) );	
}


CombinedProcess::~CombinedProcess()
{
	//while( !fCombinedProcessList.empty() ){
	//	JDFProcess* p = *fCombinedProcessList.begin();
	//	fCombinedProcessList.erase(p);
	//	ICOM_DELETE_VARIABLE(p);		
	//}
}

void	CombinedProcess::OnPreExtendDOM()
{
	/*
	 * Zuerst müssen wir das Attribut Types setzen. Hier sind alle Prozesse aufgelistet, welcher dieser kombinierte
	 * Prozess beschreibt. Die Reihenfolge der Prozesse sollte dabei so angegeben werden wie das Gerät diese üblicherweise
	 * abarbeitet. 
	 *
	 * JDF Reference, 1.5, Abschnitt 3.4.3, Seite 108 im PDF
	 */
	fCombinedProcessList = this->createCombinedProcessList();


	/*
	 * Nicht alle erzeugten Prozesse wurden in die Prozessliste hinzugefügt (combine).
	 */
	assert(fCombinedProcessList.size() == fCombinedProcessSet.size() );

	JDFNode::OnPreExtendDOM();

	for( std::list<JDFProcess*>::iterator it = fCombinedProcessList.begin();it != fCombinedProcessList.end();it++){
		(*it)->OnPreExtendDOM();
	}
}

void	CombinedProcess::OnExtendDOM(xercesc::DOMDocument* document,xercesc::DOMElement* element)
{
	assert(element);
	assert(document);

	

	XMLCh* attr_types = (XMLCh*)ICOM::Framework::ICOMFramework::fgMemoryManager->allocate( 4096 * sizeof(XMLCh*) );
	ICOM::Framework::Janitor<XMLCh> jan_attr_types(attr_types);
	*attr_types = 0;
	for(std::list<JDFProcess*>::iterator it = fCombinedProcessList.begin();it != fCombinedProcessList.end();it++){

		/*
		 * kleine Prüfung, ob der String auch nicht zu lang wird. In der Regel sollten 4k an Puffergröße ausreichen.
		 */
		if( xercesc::XMLString::stringLen(attr_types) + xercesc::XMLString::stringLen((*it)->getProcessName()) + 2 >= 4096 ){ 
			ThrowICOMException1(ICOMJDFErrorCodes::ERROR_BUFFER_TOO_SMALL,ICOM::Framework::ICOMString::toString(4096).c_str());
		}

		xercesc::XMLString::catString( attr_types, (*it)->getProcessName() );
		xercesc::XMLString::catString( attr_types, L" " );
	}
	attr_types[ xercesc::XMLString::stringLen(attr_types) - 1 ] = 0; /* das letzte Leerzeichen wollen wir nicht haben */
	
	/*
	 * Nun erzeugen wir die Ausgaberessource des letzten Prozesses. Gibt es keinen Elternknoten, dann ist dieser 
	 * Prozess sogar das Endprodukt.
	 */
	AbstractResource* outputResource = fLastProcess->createOutput(0);
	ResourceLink* resourceLink = fResourceLinkPool.addResource(outputResource);
	resourceLink->setResourceIsInput(false);

	/*
	 * Da wir den konkreten CombinedProcessIndex erst dann kennen, wenn wir die JDF-Datei auch wirklich erzeugen wollen,
	 * m�ssen wir den CombinedProcessIndex aller Resourcen l�schen um auch keinen fehlerhaften Code zu erzeugen.
	 * Jede Resource enth�lt dann die CombinedProcessIndizes des JDFProcess zu denen diese geh�ren 
	 * 
	 * JDF Reference, 1.5, Abschnitt 3.4.3, Seite im PDF 109
	 */
	ICF_int32 combinedProcessIndex = 0;
	for( std::list<JDFProcess*>::iterator it = fCombinedProcessList.begin();it != fCombinedProcessList.end();it++,combinedProcessIndex++){
		(*it)->addCombinedProcessIndex(combinedProcessIndex);
	}
	assert(combinedProcessIndex > 0);
	resourceLink->addCombinedProcessIndex(combinedProcessIndex - 1);

	JDFNode::OnExtendDOM(document,element);
	element->setAttribute( JDFKeywords::getJDFKeyword(JDFKeywords::ID_ATTRIBUTE_TYPES), attr_types );
}

void	CombinedProcess::combine(JDFProcess* input,JDFProcess* process)
{
	assert(input);
	assert(process);

	process->addInput(input);

	/*
	 * Wir pr�fen noch, ob der Prozess process Eingabe von fLastProcess ist. Ist dies der Fall, dann bleibt
	 * fLastProcess der letzte Prozess in der Prozesskette. Andernfalls wird process zu fLastProcess.
	 */
	if( fLastProcess ){
		if( !fLastProcess->testProcess(process) ){
			fLastProcess = process;
		}

	}else{
		fLastProcess = process;
	}
}

std::list<JDFProcess*>	CombinedProcess::createCombinedProcessList() const
{
	/*
	 * Bestimme erstmal zu jeden Prozess den Abstand zum letzten Prozess.
	 */
	std::list<CombinedProcess::_ProcessDistance> distanceList;
	this->traverse(fLastProcess,distanceList,0);

	/*
	 * Nun lege die Prozesse so in combinedProcessList ab, dass die Prozesse nach Abstand absteigend nach Abstand geordnet sind.
	 */
	std::list<JDFProcess*> combinedProcessList;
	for(int curDist = 0;;curDist++){

		int pushCount = 0;
		for(std::list<CombinedProcess::_ProcessDistance>::const_iterator it = distanceList.begin();it != distanceList.end();it++){
			if( curDist == it->distance ){
				combinedProcessList.push_front(it->process);
				pushCount++;
			}
		}
		if( pushCount == 0 ){
			break;
		}
	}

	return combinedProcessList;
}

void	CombinedProcess::traverse(JDFProcess* process,std::list<CombinedProcess::_ProcessDistance>& distanceList,int distance) const
{
	assert(process);

	CombinedProcess::_ProcessDistance distanceStruct;
	distanceStruct.process = process;
	distanceStruct.distance = distance;
	distanceList.push_back(distanceStruct);
	for( std::set<JDFProcess*>::const_iterator it = process->fInputProcesses.begin();it != process->fInputProcesses.end();it++){
		this->traverse(*it,distanceList,distance + 1 );
	}
}

JDFNode::NodeType	CombinedProcess::getNodeType() const
{
	return JDFNode::NodeType_CombinedProcess;
}

void	CombinedProcess::OnExtendDOMResouceLinkPool(xercesc::DOMDocument* document,xercesc::DOMElement* element)
{
	for( std::list<JDFProcess*>::iterator it = fCombinedProcessList.begin();it != fCombinedProcessList.end();it++){
		fResourceLinkPool.swallow( &(*it)->fResourceLinkPool );
	}
	JDFNode::OnExtendDOMResouceLinkPool(document,element);
}

void	CombinedProcess::OnExtendDOMResoucePool(xercesc::DOMDocument* document,xercesc::DOMElement* element)
{
	for( std::list<JDFProcess*>::iterator it = fCombinedProcessList.begin();it != fCombinedProcessList.end();it++){
		(*it)->fResourcePool.OnExtendDOM(document,element);
	}
	JDFNode::OnExtendDOMResoucePool(document,element);
}
