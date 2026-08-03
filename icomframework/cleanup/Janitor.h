

#if !defined( _ICOM_FRAMEWORK_JANITOR_H_ )
#define _ICOM_FRAMEWORK_JANITOR_H_

#include "XMemory.h"
#include <list>

namespace ICOM{
	namespace Framework{
	


	template <class T> class Janitor : public XMemory
	{
	public  :
		// -----------------------------------------------------------------------
		//  Constructors and Destructor
		// -----------------------------------------------------------------------
		Janitor(T* const toDelete);
		~Janitor();

		// -----------------------------------------------------------------------
		//  Public, non-virtual methods
		// -----------------------------------------------------------------------
		void orphan();

		//  small amount of auto_ptr compatibility
		T& operator*() const;
		T* operator->() const;
		T* get() const;
		T* release();
		void reset(T* p = 0);
		bool isDataNull();

	private :
		// -----------------------------------------------------------------------
		//  Unimplemented constructors and operators
		// -----------------------------------------------------------------------
		Janitor();
		Janitor(const Janitor<T>&);
		Janitor<T>& operator=(const Janitor<T>&);

		// -----------------------------------------------------------------------
		//  Private data members
		//
		//  fData
		//      This is the pointer to the object or structure that must be
		//      destroyed when this object is destroyed.
		// -----------------------------------------------------------------------
		T*  fData;
	};



	template <class T> class ArrayJanitor : public XMemory
	{
	public  :
		// -----------------------------------------------------------------------
		//  Constructors and Destructor
		// -----------------------------------------------------------------------
		ArrayJanitor(T* const toDelete);
		ArrayJanitor(T* const toDelete, MemoryManager* const manager);
		~ArrayJanitor();


		// -----------------------------------------------------------------------
		//  Public, non-virtual methods
		// -----------------------------------------------------------------------
		void orphan();

		//	small amount of auto_ptr compatibility
		T&	operator[](int index) const;
		T*	get() const;
		T*	release();
		void reset(T* p = 0);
		void reset(T* p, MemoryManager* const manager);

	private :
		// -----------------------------------------------------------------------
		//  Unimplemented constructors and operators
		// -----------------------------------------------------------------------
		ArrayJanitor();
		ArrayJanitor(const ArrayJanitor<T>& copy);
		ArrayJanitor<T>& operator=(const ArrayJanitor<T>& copy);

		// -----------------------------------------------------------------------
		//  Private data members
		//
		//  fData
		//      This is the pointer to the object or structure that must be
		//      destroyed when this object is destroyed.
		// -----------------------------------------------------------------------
		T*  fData;
		MemoryManager* fMemoryManager;
	};

	template <class T> class JanitorList : public XMemory
	{
	public  :
		// -----------------------------------------------------------------------
		//  Constructors and Destructor
		// -----------------------------------------------------------------------
		JanitorList(std::list<T*>* toDelete);
		~JanitorList();

		// -----------------------------------------------------------------------
		//  Public, non-virtual methods
		// -----------------------------------------------------------------------
		void orphan();

		void reset(std::list<T*>* p = 0);

	private :
		// -----------------------------------------------------------------------
		//  Unimplemented constructors and operators
		// -----------------------------------------------------------------------
		JanitorList();
		JanitorList(const JanitorList<T>&);
		JanitorList<T>& operator=(const JanitorList<T>&);

		// -----------------------------------------------------------------------
		//  Private data members
		//
		//  fData
		//      This is the pointer to the object or structure that must be
		//      destroyed when this object is destroyed.
		// -----------------------------------------------------------------------
		std::list<T*>*  fDataList;
	};
	
	
	// ---------------------------------------------------------------------------
	//  Janitor: Constructors and Destructor
	// ---------------------------------------------------------------------------
	template <class T> Janitor<T>::Janitor(T* const toDelete) :
		fData(toDelete)
	{
	}


	template <class T> Janitor<T>::~Janitor()
	{
		reset();
	}


	// ---------------------------------------------------------------------------
	//  Janitor: Public, non-virtual methods
	// ---------------------------------------------------------------------------
	template <class T> void
	Janitor<T>::orphan()
	{
	   release();
	}


	template <class T> T&
	Janitor<T>::operator*() const
	{
		return *fData;
	}


	template <class T> T*
	Janitor<T>::operator->() const
	{
		return fData;
	}


	template <class T> T*
	Janitor<T>::get() const
	{
		return fData;
	}


	template <class T> T*
	Janitor<T>::release()
	{
		T* p = fData;
		fData = 0;
		return p;
	}


	template <class T> void Janitor<T>::reset(T* p)
	{
		if (fData)
			delete fData;

		fData = p;
	}

	template <class T> bool Janitor<T>::isDataNull()
	{
		return (fData == 0);
	}


	// ---------------------------------------------------------------------------
	//  JanitorList: Public, non-virtual methods
	// ---------------------------------------------------------------------------

	template <class T> void
	JanitorList<T>::orphan()
	{
	   fDataList = 0;
	}

	template <class T> void JanitorList<T>::reset(std::list<T*>* p)
	{
		if( fDataList ){
			ICOM_DELETE_LIST( (*fDataList) );
		}
		fDataList = p;
	}

	// ---------------------------------------------------------------------------
	//  JanitorList: Constructors and Destructor
	// ---------------------------------------------------------------------------
	template <class T> JanitorList<T>::JanitorList(std::list<T*>* toDelete) :
		fDataList(toDelete)
	{
	}


	template <class T> JanitorList<T>::~JanitorList()
	{
		if( fDataList ){
			ICOM_DELETE_LIST( (*fDataList) );
		}
	}


	// -----------------------------------------------------------------------
	//  ArrayJanitor: Constructors and Destructor
	// -----------------------------------------------------------------------
	template <class T> ArrayJanitor<T>::ArrayJanitor(T* const toDelete) :
		fData(toDelete)
		, fMemoryManager(0)
	{
	}

	template <class T>
	ArrayJanitor<T>::ArrayJanitor(T* const toDelete,
								  MemoryManager* const manager) :
		fData(toDelete)
		, fMemoryManager(manager)
	{
	}


	template <class T> ArrayJanitor<T>::~ArrayJanitor()
	{
		reset();
	}


	// -----------------------------------------------------------------------
	//  ArrayJanitor: Public, non-virtual methods
	// -----------------------------------------------------------------------
	template <class T> void
	ArrayJanitor<T>::orphan()
	{
	   release();
	}


	//	Look, Ma! No hands! Don't call this with null data!
	template <class T> T&
	ArrayJanitor<T>::operator[](int index) const
	{
		//	TODO: Add appropriate exception
		return fData[index];
	}


	template <class T> T*
	ArrayJanitor<T>::get() const
	{
		return fData;
	}


	template <class T> T*
	ArrayJanitor<T>::release()
	{
		T* p = fData;
		fData = 0;
		return p;
	}


	template <class T> void
	ArrayJanitor<T>::reset(T* p)
	{
		if (fData) {

			if (fMemoryManager)
				fMemoryManager->deallocate((void*)fData);
			else
				delete [] fData;
		}

		fData = p;
		fMemoryManager = 0;
	}

	template <class T> void
	ArrayJanitor<T>::reset(T* p, MemoryManager* const manager)
	{
		if (fData) {

			if (fMemoryManager)
				fMemoryManager->deallocate((void*)fData);
			else
				delete [] fData;
		}

		fData = p;
		fMemoryManager = manager;
	}


	
}

}



#endif /*_ICOM_FRAMEWORK_JANITOR_H_*/


 
