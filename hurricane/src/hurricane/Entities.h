// ****************************************************************************************************
// File: Entities.h
// Authors: R. Escassut
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************

#ifndef HURRICANE_ENTITIES
#define HURRICANE_ENTITIES

#include "Collection.h"

namespace Hurricane {

class Entity;



// ****************************************************************************************************
// Entities declaration
// ****************************************************************************************************

typedef GenericCollection<Entity*> Entities;



// ****************************************************************************************************
// EntityLocator declaration
// ****************************************************************************************************

typedef GenericLocator<Entity*> EntityLocator;



// ****************************************************************************************************
// EntityFilter declaration
// ****************************************************************************************************

typedef GenericFilter<Entity*> EntityFilter;



// ****************************************************************************************************
// for_each_entity declaration
// ****************************************************************************************************

#define for_each_entity(entity, entities)\
/****************************************/\
{\
	EntityLocator _locator = entities.GetLocator();\
	while (_locator.IsValid()) {\
		Entity* entity = _locator.GetElement();\
		_locator.Progress();



} // End of Hurricane namespace.

#endif // HURRICANE_ENTITIES

// ****************************************************************************************************
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************