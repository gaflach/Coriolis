// ****************************************************************************************************
// File: Name.h
// Authors: R. Escassut
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************

#ifndef HURRICANE_NAME
#define HURRICANE_NAME

#include "Commons.h"
#include "Names.h"

namespace Hurricane {

class SharedName;



// ****************************************************************************************************
// Name declaration
// ****************************************************************************************************

class Name {
// *******

// Attributes
// **********

	private: SharedName* _sharedName;

// Constructors
// ************

	public: Name();

	public: Name(const char* c);
	public: Name(const string& s);

	public: Name(const Name& name);

// Destructor
// **********

	public: ~Name();

// Operators
// *********

	public: Name& operator=(const Name& name);

	public: bool operator==(const Name& name) const;
	public: bool operator!=(const Name& name) const;
	public: bool operator<(const Name& name) const;
	public: bool operator<=(const Name& name) const;
	public: bool operator>(const Name& name) const;
	public: bool operator>=(const Name& name) const;

	public: char operator[](unsigned index) const;

// Predicates
// **********

	public: bool IsEmpty() const;

// Others
// ******

    public: string _GetTypeName() const { return _TName("Name"); };
	public: string _GetString() const;
	public: Record* _GetRecord() const;
	public: SharedName* _GetSharedName() const {return _sharedName;};

};


} // End of Hurricane namespace.


ValueIOStreamSupport(Hurricane::Name)


bool Scan ( const string& s, H::Name& name );

#endif // HURRICANE_NAME

// ****************************************************************************************************
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************