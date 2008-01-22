// ****************************************************************************************************
// File: IntrusiveSet.h
// Authors: R. Escassut
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************

#ifndef HURRICANE_INTRUSIVE_SET
#define HURRICANE_INTRUSIVE_SET

#include <algorithm>

#include "Collection.h"
#include "Error.h" // AD

namespace Hurricane {



// ****************************************************************************************************
// IntrusiveSet declaration
// ****************************************************************************************************

template<class Element> class IntrusiveSet : public NestedSlotAdapter {
// ******************************************************************

// Types
// *****

	class Elements : public Collection<Element*> {
	// *****************************************
		
	// Types
	// *****
	
		public: typedef Collection<Element*> Inherit;
	
		public: class Locator : public Hurricane::Locator<Element*> {
		// ********************************************************
	
		// Types
		// *****
	
			public: typedef Hurricane::Locator<Element*> Inherit;
	
		// Attributes
		// **********
	
			private: const IntrusiveSet* _set;
			private: unsigned _index;
			private: Element* _element;
	
		// Constructors
		// ************
	
			public: Locator(const IntrusiveSet* set = NULL)
			// ********************************************
			:	Inherit(),
				_set(set),
				_index(0),
				_element(NULL)
			{
				if (_set) {
					unsigned length = _set->_GetLength();
					do {
						_element = _set->_GetArray()[_index++];
					} while (!_element && (_index < length));
				}
			};

			public: Locator(const Locator& locator)
			// ************************************
			:	Inherit(),
				_set(locator._set),
				_index(locator._index),
				_element(locator._element)
			{
			};
	
		// Operators
		// *********
	
			public: Locator& operator=(const Locator& locator)
			// ***********************************************
			{
				_set = locator._set;
				_index = locator._index;
				_element = locator._element;
				return *this;
			};
	
		// Accessors
		// *********
	
			public: virtual Element* GetElement() const
			// ****************************************
			{
				return _element;
			};

			public: virtual Hurricane::Locator<Element*>* GetClone() const
			// ***********************************************************
			{
				return new Locator(_set);
			};
	
		// Predicates
		// **********
	
			public: virtual bool IsValid() const
			// *********************************
			{
				return (_element != NULL);
			};
	
		// Updators
		// ********
	
			public: virtual void Progress()
			// ****************************
			{
				if (_element) {
					_element = _set->_GetNextElement(_element);
					if (!_element) {
						unsigned length = _set->_GetLength();
						if (_index < length) {
							do {
								_element = _set->_GetArray()[_index++];
							} while (!_element && (_index < length));
						}
					}
				}
			};
	
		// Others
		// ******
	
            public: virtual string _GetTypeName() const
            // ****************************************
            {
              return _TName("IntrusiveSet::Elements::Locator");
            };

			public: virtual string _GetString() const
			// **************************************
			{
				string s = "<" + _GetTypeName();
				if (_set) s += " " + GetString(_set);
				s += ">";
				return s;
			};
	
		};
	
	// Attributes
	// **********
	
		private: const IntrusiveSet* _set;
	
	// Constructors
	// ************
	
		public: Elements(const IntrusiveSet* set = NULL)
		// *********************************************
		: 	Inherit(),
			_set(set)
		{
		};

		public: Elements(const Elements& elements)
		// ***************************************
		: 	Inherit(),
			_set(elements._set)
		{
		};
	
	// Operators
	// *********
	
		public: Elements& operator=(const Elements& elements)
		// **************************************************
		{
			_set = elements._set;
			return *this;
		};
	
	// Accessors
	// *********
	
		public: virtual Collection<Element*>* GetClone() const
		// ***************************************************
		{
			return new Elements(*this);
		};

		public: virtual Hurricane::Locator<Element*>* GetLocator() const
		// *************************************************************
		{
			return new Locator(_set);
		};
	
	// Others
	// ******

        public: virtual string _GetTypeName() const
    	// **************************************
        {
          return _TName("IntrusiveSet::Elements");
        };
	
		public: virtual string _GetString() const
		// **************************************
		{
			string s = "<" + _GetTypeName();
			if (_set) s += " " + GetString(_set);
			s += ">";
			return s;
		};
	
	};

// Attributes
// **********

	private: unsigned _size;
	private: unsigned _length;
	private: Element** _array;

// Constructors
// ************

	public: IntrusiveSet()
	// *******************
	:	_size(0),
		_length(1),
		_array(new Element*[1])
	{
		_array[0] = NULL;
	};

	private: IntrusiveSet(const IntrusiveSet& set); // not implemented to forbid copy

// Destructor
// **********

	public: virtual ~IntrusiveSet()
	// ****************************
	{
		for (unsigned index = 0; index < _length; index++) {
			Element* element = _array[index];
			while (element) {
				_array[index] = _GetNextElement(element);
				_SetNextElement(element, NULL);
				element = _array[index];
			}
			_array[index] = NULL;
		}
		delete[] _array;
	};

// Operators
// *********

	private: IntrusiveSet& operator=(const IntrusiveSet& set); // not implemented to forbid assignment

// Accessors
// *********

	public: Elements GetElements() const
	// *********************************
	{
		return Elements(this);
	};

// Predicates
// **********

	public: bool IsEmpty() const
	// *************************
	{
		return (_size == 0);
	};

// Overridables
// ************

	public: virtual unsigned _GetHashValue(Element* element) const = 0;

	// public: virtual Element* _GetNextElement(Element* element) const = 0; // AD
	public: virtual Element* _GetNextElement(Element* element) const
	// *************************************************************
	{
		throw Error(_TName("IntrusiveSet") + "::_GetNextElement(...) : should be overrided");
		return NULL;
	};

	// public: virtual void _SetNextElement(Element* element, Element* nextElement) const = 0; // AD
	public: virtual void _SetNextElement(Element* element, Element* nextElement) const
	// *******************************************************************************
	{
		throw Error(_TName("IntrusiveSet") + "::_SetNextElement(...) : should be overrided");
	};

// Others
// ******

    public: virtual string _GetTypeName() const
	// **************************************
    {
      return _TName("InstrusiveSet");
    };

	public: string _GetString() const
	// ******************************
	{
		if (IsEmpty())
			return "<" + _GetTypeName() + " empty>";
		else
			return "<" + _GetTypeName() + " " + GetString(_size) + ">";
	};

	public: Record* _GetRecord() const
	// *************************
	{
		Record* record = NULL;
		if (!IsEmpty()) {
			record = new Record(GetString(this));
			unsigned n = 1;
			for (unsigned index = 0; index < _length; index++) {
				/**/
				n = 1;
				/**/
				Element* element = _array[index];
				while (element) {
					// record->Add(GetSlot(GetString(n++), element));
					record->Add(GetSlot(GetString(index) + ":" + GetString(n++), element));
					/**/
					element = _GetNextElement(element);
				}
			}
		}
		return record;
	};

	public: unsigned _GetSize() const
	// ******************************
	{
		return _size;
	};

	public: unsigned _GetLength() const
	// ********************************
	{
		return _length;
	};

	public: Element** _GetArray() const
	// ********************************
	{
		return _array;
	};

	public: bool _Contains(Element* element) const
	// *******************************************
	{
		unsigned index = (_GetHashValue(element) / 8) % _length;
		Element* currentElement = _array[index];
		while (currentElement && (currentElement != element))
			currentElement = _GetNextElement(currentElement);
		return (currentElement != NULL);
	};

	public: void _Insert(Element* element)
	// ***********************************
	{
		if (!_Contains(element)) {
			unsigned index = (_GetHashValue(element) / 8) % _length;
			_SetNextElement(element, _array[index]);
			_array[index] = element;
  			_size++;
			_Resize();
		}
	};

	public: void _Remove(Element* element)
	// ***********************************
	{
		if (_Contains(element)) {
			unsigned index = (_GetHashValue(element) / 8) % _length;
			Element* currentElement = _array[index];
			if (currentElement) {
				if (currentElement == element) {
					_array[index] = _GetNextElement(element);
					_SetNextElement(element, NULL);
					_size--;
				}
				else {
					while (_GetNextElement(currentElement) && (_GetNextElement(currentElement) != element))
						currentElement = _GetNextElement(currentElement);
					if (currentElement && (_GetNextElement(currentElement) == element)) {
						_SetNextElement(currentElement, _GetNextElement(element));
						_SetNextElement(element, NULL);
						_size--;
					}
				}
			}
		}
	};

	public: void _Resize()
	// *******************
	{
		unsigned newLength = _length;
		double ratio = (double)_size / (double)_length;
		if (ratio < 3)
			newLength = max(_size / 8, (unsigned)1);
		else if (10 < ratio)
			newLength = min(_size / 5, (unsigned)512);

		if (newLength != _length) {
			// cerr << "Resizing: " << this << " " << _length << " " << newLength << endl;
			unsigned oldLength = _length;
			Element** oldArray = _array;
			_length = newLength;
			_array = new Element*[_length];
			memset(_array, 0, _length * sizeof(Element*));
			for (unsigned index = 0; index < oldLength; index++) {
				Element* element = oldArray[index];
				while (element) {
					Element* nextElement = _GetNextElement(element);
					unsigned newIndex = (_GetHashValue(element) / 8) % _length;
					_SetNextElement(element, _array[newIndex]);
					_array[newIndex] = element;
					element = nextElement;
				}
			}
			delete[] oldArray;
		}
	};

	public: void _Clear()
	// ******************
	{
		for (unsigned index = 0; index < _length; index++) {
			Element* element = _array[index];
			while (element) {
				_array[index] = _GetNextElement(element);
				_SetNextElement(element, NULL);
				element = _array[index];
			}
			_array[index] = NULL;
		}
		delete[] _array;

		_size = 0;
		_length = 1;
		_array = new Element*[1];
		_array[0] = NULL;
	};

};



} // End of Hurricane namespace.

#endif // HURRICANE_INTRUSIVE_SET

// ****************************************************************************************************
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************