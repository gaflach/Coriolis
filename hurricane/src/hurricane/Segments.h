// ****************************************************************************************************
// File: Segments.h
// Authors: R. Escassut
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************

#ifndef HURRICANE_SEGMENTS
#define HURRICANE_SEGMENTS

#include "Collection.h"

namespace Hurricane {

class Segment;



// ****************************************************************************************************
// Segments declaration
// ****************************************************************************************************

typedef GenericCollection<Segment*> Segments;



// ****************************************************************************************************
// SegmentLocator declaration
// ****************************************************************************************************

typedef GenericLocator<Segment*> SegmentLocator;



// ****************************************************************************************************
// SegmentFilter declaration
// ****************************************************************************************************

typedef GenericFilter<Segment*> SegmentFilter;



// ****************************************************************************************************
// for_each_segment declaration
// ****************************************************************************************************

#define for_each_segment(segment, segments)\
/******************************************/\
{\
	SegmentLocator _locator = segments.GetLocator();\
	while (_locator.IsValid()) {\
		Segment* segment = _locator.GetElement();\
		_locator.Progress();



} // End of Hurricane namespace.

#endif // HURRICANE_SEGMENTS

// ****************************************************************************************************
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************
