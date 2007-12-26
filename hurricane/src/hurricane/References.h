// ****************************************************************************************************
// File: References.h
// Authors: J.-P. Chaput
// Copyright (c) LIP6/UPMC 2006-2006, All Rights Reserved
// ****************************************************************************************************

#ifndef HURRICANE_REFERENCES
#define HURRICANE_REFERENCES

#include "Collection.h"

namespace Hurricane {

class Reference;



// ****************************************************************************************************
// References declaration
// ****************************************************************************************************

typedef GenericCollection<Reference*> References;



// ****************************************************************************************************
// ReferenceLocator declaration
// ****************************************************************************************************

typedef GenericLocator<Reference*> ReferenceLocator;



// ****************************************************************************************************
// ReferenceFilter declaration
// ****************************************************************************************************

typedef GenericFilter<Reference*> ReferenceFilter;



// ****************************************************************************************************
// for_each_marker declaration
// ****************************************************************************************************

#define for_each_reference(reference, references)\
/***************************************/\
{\
	ReferenceLocator _locator = references.GetLocator();\
	while (_locator.IsValid()) {\
		Reference* reference = _locator.GetElement();\
		_locator.Progress();



} // End of Hurricane namespace.

#endif // HURRICANE_REFERENCES

// ****************************************************************************************************
// Copyright (c) LIP6/UPMC 2006-2006, All Rights Reserved
// ****************************************************************************************************
