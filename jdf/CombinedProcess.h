

#if !defined( ICOMJDF_COMBINEDPROCESS_H )
#define ICOMJDF_COMBINEDPROCESS_H

#include "XMemory.h"
#include "JDFNode.h"
#include "Component.h"



#include <set>
#include <list>


namespace ICOM{
	namespace JDF{

		class JDFProcess;

		/*
		 * Knoten vom Typ Combined sind Blätter und können deshalb keine weiteren JDF Unterknoten enthalten
		 * (JDF Reference 1.5, Abschnitt 3.4.3, Seite im PDF 109)
		 */

		class CombinedProcess : public JDFNode {

			friend class Job;

		public:


	
			/** @name Konstruktoren und Destruktor */
			//@{
			CombinedProcess(JDFNode* parent = 0);

			virtual ~CombinedProcess();
			//@}
	
			/** @name Getter Methoden */
			//@{
			virtual		  NodeType	getNodeType() const;

			//@}			

			virtual void	OnExtendDOM(xercesc::DOMDocument* document,xercesc::DOMElement* element);
			virtual void	OnPreExtendDOM();
			/*
			 * Verknüpft zwei Prozesse miteinander, so dass der Prozess input als Eingabeprozess des Prozesses process dient.
			 */
			virtual void	combine(JDFProcess* input,JDFProcess* process);

			/*
			 * Erzeugt einen Unterprozess des kombinierten Prozesses.
			 */
			template<class ProcessType> ProcessType*	createProcess();


		protected:

			virtual void	OnExtendDOMResoucePool(xercesc::DOMDocument* document,xercesc::DOMElement* element);
			virtual void	OnExtendDOMResouceLinkPool(xercesc::DOMDocument* document,xercesc::DOMElement* element);

			/*
			 * Erzeugt eine Liste an Prozesses aus fCombinedProcessList, so wie diese in das Attribut Types hinterlegt werden.
			 */
			std::list<JDFProcess*>	createCombinedProcessList() const;

	
		private:

			std::set<JDFProcess*>	fCombinedProcessSet; /* Hier werden die Prozesse drin gesammelt */
			std::list<JDFProcess*>	fCombinedProcessList; /* Später werden die gesammelten Prozesse in die richtige Reihenfolge sortiert */

			/*
			 * Hier wird der letzte Prozess in der Prozesskette hinterlegt.
			 */
			JDFProcess*			fLastProcess;

			struct _ProcessDistance{
				JDFProcess*		process;
				int				distance;
			};
			void	traverse(JDFProcess* process,std::list<_ProcessDistance>& distanceList,int distance) const;
	
		};

		template<class ProcessType>	ProcessType* CombinedProcess::createProcess()
		{
			ProcessType* process = new ProcessType(this);
			fCombinedProcessSet.insert(process);
			return process;
		}



}

}

#endif /*ICOMJDF_COMBINEDPROCESS_H*/


 
