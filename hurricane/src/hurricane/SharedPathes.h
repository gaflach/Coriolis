// ****************************************************************************************************
// File: SharedPathes.h
// Authors: R. Escassut
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************

#ifndef HURRICANE_SHARED_PATHES
#define HURRICANE_SHARED_PATHES

#include "Collection.h"

namespace Hurricane {

class SharedPath;



// ****************************************************************************************************
// SharedPathes declaration
// ****************************************************************************************************

typedef GenericCollection<SharedPath*> SharedPathes;



// ****************************************************************************************************
// SharedPathLocator declaration
// ****************************************************************************************************

typedef GenericLocator<SharedPath*> SharedPathLocator;



// ****************************************************************************************************
// SharedPathFilter declaration
// ****************************************************************************************************

typedef GenericFilter<SharedPath*> SharedPathFilter;



// ****************************************************************************************************
// for_each_shared_path declaration
// ****************************************************************************************************

#define for_each_shared_path(sharedPath, sharedPathes)\
/*****************************************************/\
{\
	SharedPathLocator _locator = sharedPathes.GetLocator();\
	while (_locator.IsValid()) {\
		SharedPath* sharedPath = _locator.GetElement();\
		_locator.Progress();



} // End of Hurricane namespace.

#endif // HURRICANE_SHARED_PATHES

// ****************************************************************************************************
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************