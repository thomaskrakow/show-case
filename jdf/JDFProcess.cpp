

#include "JDFProcess.h"
#include "ICOMJDFException.h"
#include "ICOMJDFErrorCodes.h"
#include "JDFKeywords.h"
#include "AbstractResource.h"
#include "Janitor.h"
#include "Component.h"
#include "ResourceLink.h"

#include <assert.h>

using namespace ICOM::JDF;

JDFProcess::JDFProcess(JDFNode* parent) :
	JDFNode(parent),
	fCustomerInfo(0)
{
	fResourcePool.setType( ResourcePool::Type_Process );	
}



JDFProcess::~JDFProcess()
{

}



bool	JDFProcess::testProcess(const JDFProcess* process) const
{
	assert(process);

	for(std::set<JDFProcess*>::const_iterator it = fInputProcesses.begin();it != fInputProcesses.end();it++){
		if( *it == process ){
			return true;
		}
		if( (*it)->testProcess(process) ){
			return true;
		}
	}

	return false;
}

void	JDFProcess::OnExtendDOM(xercesc::DOMDocument* document,xercesc::DOMElement* element)
{
	this->setType( this->getProcessName() );

	JDFNode::OnExtendDOM(document,element);
}

JDFNode::NodeType	JDFProcess::getNodeType() const
{
	return JDFNode::NodeType_Process;
}

void	JDFProcess::addCombinedProcessIndex(ICF_uint8 index)
{
	fResourceLinkPool.addCombinedProcessIndex(index);
}


ResourceLink*	JDFProcess::addInput(AbstractResource* resource,ResourceLink::ProcessUsage processUsage)
{
	assert(resource);

	/*
	 * Zuerst wird �berpr�ft, ob die Ressource tats�chlich als Eingabe f�r diesen Prozess
	 * erlaubt ist.
	 */
	bool bOk = this->canHandleInput(resource,processUsage);
	assert( bOk );

	/*
	 * Ist die Ressource als Eingaberessource erlaubt, dann f�gen wir einen Link dem ResourceLinkPool hinzu.
	 */
	ResourceLink* resourceLink = this->linkResource(resource);
	resourceLink->setResourceIsInput(true);

	/*
	 * Da Eingaberessourcen vom Typ gleich sein k�nnen (z.b. mehere RunList ressourcen) aber von der fachlichen Bedeutung
	 * unterschiedlich sind, so m�ssen diese durch ProcessUsage unterschieden werden.
	 *
	 *
	 * JDF Reference, 1.5, Abschnitt 3.10.3, Seite 131 im PDF
	 */
	resourceLink->setProcessUsage(processUsage);

	return resourceLink;
}

void	JDFProcess::addInput(JDFProcess* process)
{
	assert(process);

	/*
	 * Wir erlauben erstmal keine Zirkelprozesse, obwohl es hier Szenarien gibt, wo Zirkelprozesse Sinn machen (z.b. Closed Loop).
	 */
	assert( !this->testProcess(process) );	

	if( fInputProcesses.size() ==  1 ){
		JDFProcess* p = *fInputProcesses.begin();
		this->createOutput(p);
	}
	if( fInputProcesses.size() > 0 ){
		this->createOutput(process);
	}

	fInputProcesses.insert(process);
}

bool	JDFProcess::canHandleInput(AbstractResource* resource,ResourceLink::ProcessUsage processUsage)
{	
	PROCESS_CAN_HANDLE_INPUT_ONCE(CustomerInfo,fCustomerInfo);
	return false;
}

CustomerInfo*	JDFProcess::createInputCustomerInfo()
{
	CREATE_AND_RETURN_INPUT_RESOURCE(CustomerInfo,ResourceLink::ProcessUsage_NotSet);
}
