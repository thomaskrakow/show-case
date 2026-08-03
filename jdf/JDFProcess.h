

#if !defined( ICOMJDF_JDFPROCESS_H )
#define ICOMJDF_JDFPROCESS_H

#include "XMemory.h"
#include "JDFNode.h"
#include "AbstractResource.h"
#include "CombinedProcess.h"
#include "CustomerInfo.h"



namespace ICOM{
	namespace JDF{

		class JDFProcess : public JDFNode {

			friend class CombinedProcess;

		public:

	
			/** @name Konstruktoren und Destruktor */
			//@{
			JDFProcess(JDFNode* parent = 0);
			virtual ~JDFProcess();
			//@}
	
			/** @name Getter Methoden */
			//@{
			virtual const XMLCh*	getProcessName() const = 0;
			virtual		  NodeType	getNodeType() const;

			//@}

			/*
			 * Fügt dem Prozess eine Eingabe Ressource (resource) hinzu. Ist der Ressourcentyp zweier Eingaberssourcen
			 * gleich (z.b. beide RunList), aber die Ressourcen unterschiedliche Bedeutung haben, so wird diese Bedeutung
			 * durch processUsage unterschieden. Ein Beispiel ist der Prozess LayoutPreparation, welche zwei unterschieldiche
			 * Ressourcen vom Typ RunList entgegen nehmen kann, die sich allerdings vom ProcessUsage unterscheiden müssen.
			 */
			virtual ResourceLink*							addInput(AbstractResource* resource,ResourceLink::ProcessUsage processUsage);		

			virtual void									OnExtendDOM(xercesc::DOMDocument* document,xercesc::DOMElement* element);

			virtual CustomerInfo*							createInputCustomerInfo();

		protected:

			/*
			 * Erzeugt eine Ressource vom Typ Ressource, welcher eine Ausgaberessource von process und eine Eingaberessource von
			 * diesem Prozess ist. Ist der Elternknoten ein kombinierter Prozess (also von CombinedProcess abgeleitet), dann wird 
			 * den Ressourcen das Attribut PipeID und PipeProtocol="internal" hinzugefügt.
			 *
			 * Bemerkung: Ist der Elternknoten kein kombinierter Prozess so muss diese Funktion nochmal erweitert werden.
			 */
			template<class ResourceType>	ResourceType*	addInput(JDFProcess* process,ResourceLink::ProcessUsage processUsage);


			/*
			 * Fügt dem Prozess als Eingabe die Ausgabe eines anderen Prozesses hinzu.
			 *
			 * Diese Funktion soll auch protected bleiben, da die Verknüpfung zwischen zwei Prozesses der jeweilige JDF-Knoten vornehmen soll.
			 */
			virtual void									addInput(JDFProcess* process);


			/*
			 * Jeder Prozess generiert aus Eingaberessourcen eine oder mehrere Ausgaberessourcen. Meistens hat ein Prozess
			 * nur eine Ausgaberessource, diese würde dann mit dieser Funktion erzeugt werden. Manche Prozesse haben 
			 * allerdings auch mehrere Ausgaberessourcen (z.b. Approval, Delivery, QualityControl, ResourceDefinition, Split,
			 * Verification, ColorSpaceConversion, DieDesign, DieLayoutProduction, DigitalDelivery, Interpreting, LayoutElementProduction,
			 * LayoutPreparation, Stripping, ConventionalPrinting, Cutting, DieMaking, Feeding, ShapeCutting, ShapeDefProduction). Welche 
			 * Ausgaberessource erzeugt wird, hängt auch vom Prozess ab, der diese Ausgaberessource als Eingaberessource nutzt, daher wird
			 * dieser Funktion der Prozess mitgegeben, welcher diese Ressource als Eingabe nutzt.
			 * Diese Funktion erzeugt nun eine Standardasugaberessource für den Prozess process, der als Eingaberessourcen für diesen Prozess
			 * dient.
			 */
			virtual AbstractResource*						createOutput(JDFProcess* process) = 0;
			
			/*
			 * Diese Funktion gibt true zurück, wenn resource eine Eingaberessource von diesem Prozess sein darf. Es darf nämlich nicht jede
			 * Ressource als Eingabe eines Prozesses dienen (z.b. darf RenderingParams keine Eingabe von Interpreting sein). Ob eine Ressource
			 * als Eingabe erlaubt ist ergibt sich aus der Beschreibung der Prozesse (Abschnitt 6 in der JDF Reference Version 1.5).
			 */
			virtual bool	canHandleInput(AbstractResource* resource,ResourceLink::ProcessUsage processUsage = ResourceLink::ProcessUsage_NotSet);

			/*
			 * Ein Prozess kann nur Ausgaberessourcen von einem bestimmten Typ generieren. Diese Funktion gibt true zurück, wenn dieser Prozess
			 * eine Ausagbe vom Typ resource erzeugen kann.
			 */
			virtual bool	canCreateOutput(AbstractResource* resource,ResourceLink::ProcessUsage processUsage = ResourceLink::ProcessUsage_NotSet) = 0;

			/*
			 * Überprüft, ob der übergebene JDFProcess bereits als Eingabeprozess vorhanden ist. Es werden dabei auch die
			 * Eingabeprozesse von fInputProcesses usw.
			 */
			virtual bool	testProcess(const JDFProcess* process) const;


			virtual	void	addCombinedProcessIndex(ICF_uint8 index);

			/*
			 * Ein Prozess kann zwar keinen anderen Prozess als Eingabe haben, trotzdem wird hier eine Liste an Eingabeprozessen angegeben. Es ist so, dass das
			 * Ausgabeprodukt (= Ressource) eines dieser Prozesse als Eingabe-Resource dieses Prozesses dient. Dem JDF interessiert es nicht was genau diese Ressource ist, daher
			 * wird diese Resource (von der man nicht genau weiß was es ist) als Component bezeichnet.
			 */
			std::set<JDFProcess*>			fInputProcesses;

			CustomerInfo*					fCustomerInfo;
		};

		template<class ResourceType>	ResourceType*	JDFProcess::addInput(JDFProcess* process,ResourceLink::ProcessUsage processUsage)
		{
			ResourceType* resource = 0;
			if( process ){

				/*
				 * Zuerst wird die Ausgaberessource des Prozesses process erzeugt und anschließend überprüft,
				 * ob diese Ressource tatsächlich als Ausgaberessource von process erzeugt werden kann.
				 */
				resource = process->createResource<ResourceType>();
				assert( process->canCreateOutput(resource,processUsage) );

				/*
				 * Da wir explizit eine Ausgaberessource mit einem Prozess verbinden, müssen wir auch eine Pipe
				 * erstellen.
				 */
				if( dynamic_cast<CombinedProcess*>(fParent) ){
					resource->setPipeProtocol(AbstractResource::PipeProtocol_Internal);
				}else{

					/*
					 * Es muss noch überprüft werden, was diese Funktion tun muss, wenn der Elternknoten kein kombinierter Prozess ist.
					 */
					assert(false);
				}

				/*
				 * Nun wird ein Link auf diese Ressource im ResourceLinkPool erstellt und diese Ressource explizit als
				 * Ausgaberessource gekennzeichnet.
				 */
				ResourceLink* link = process->linkResource(resource);
				link->setResourceIsInput(false);
				link->setProcessUsage(processUsage);

				/*
				 * Nun wird die Ausgaberessource als Eingabe des Prozesses hinzugefügt.
				 */
				this->addInput(resource,processUsage);
				return resource;
			}

			/*
			 * Ist der Elternknoten ein kombinierter Prozess und process == 0, dann müssen wir einfach
			 * nur die Ausagebressource des kombinierten Prozesses erstellen.
			 */
			if( dynamic_cast<CombinedProcess*>(fParent) ){
				return this->createResource<ResourceType>();
			}

			/*
			 * Es muss noch überprüft werden, was diese Funktion tun muss, wenn der Elternknoten kein kombinierter Prozess ist.
			 */
			assert(false);
			
			return 0;
		}

}

}

#define PROCESS_CAN_HANDLE_INPUT_PROCESSUSAGE_ONCE(ResourceType,resource_var,process_usage)\
	ResourceType* var##resource_var = dynamic_cast<ResourceType*>(resource);\
	if( var##resource_var && processUsage == process_usage){\
		assert( resource_var == 0 );\
		resource_var = var##resource_var;\
		return true;\
	}

#define PROCESS_CAN_HANDLE_INPUT_ONCE(ResourceType,resource_var)\
	PROCESS_CAN_HANDLE_INPUT_PROCESSUSAGE_ONCE(ResourceType,resource_var,ResourceLink::ProcessUsage_NotSet)

#define PROCESS_CAN_HANDLE_INPUT_PROCESSUSAGE_ONE_OR_MORE(ResourceType,resource_var,process_usage)\
	ResourceType* var##resource_var = dynamic_cast<ResourceType*>(resource);\
	if( var##resource_var && processUsage == process_usage){\
		resource_var.insert(var##resource_var);\
		return true;\
	}

#define PROCESS_CAN_HANDLE_INPUT_ONE_OR_MORE(ResourceType,resource_var)\
	PROCESS_CAN_HANDLE_INPUT_PROCESSUSAGE_ONE_OR_MORE(ResourceType,resource_var,ResourceLink::ProcessUsage_NotSet)


#define PROCESS_CAN_HANDLE_INPUT_PROCESSUSAGE_ZERO_OR_MORE(ResourceType)\
	ResourceType* var##ResourceType = dynamic_cast<ResourceType*>(resource);\
	if( var##ResourceType ){\
		return true;\
	}

#define CREATE_AND_RETURN_INPUT_RESOURCE(ResourceType,process_usage)\
	ResourceType* resource = this->createResource<ResourceType>();\
	this->addInput(resource,process_usage);\
	return resource;

#define CREATE_AND_RETURN_INPUT_COMPONENT(componenttype,process_usage)\
	Component* resource = this->createResource<Component>();\
	resource->addComponentType(componenttype);\
	this->addInput(resource,process_usage);\
	return resource;

#define CREATE_AND_RETURN_OUTPUT_COMPONENT(componenttype,process_usage)\
	Component* component = this->addInput<Component>(process,process_usage);\
	if( process == 0 && componenttype == Component::ComponentType_PartialProduct ){\
		component->addComponentType(Component::ComponentType_FinalProduct);\
	}else{\
		component->addComponentType(componenttype);\
	}\
	return component;

#define CAN_CREATE_OUTPUT(ResourceType,process_usage)\
	if( dynamic_cast<ResourceType*>(resource) && processUsage == process_usage ){\
		return true;\
	}



#endif /*ICOMJDF_JDFPROCESS_H*/


 
