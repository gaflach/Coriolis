// ****************************************************************************************************
// File: Symbols.h
// Authors: R. Escassut
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************

#ifndef HURRICANE_SYMBOLS
#define HURRICANE_SYMBOLS

#include "Collection.h"

namespace Hurricane {

class Symbol;



// ****************************************************************************************************
// Symbols declaration
// ****************************************************************************************************

typedef GenericCollection<Symbol*> Symbols;



// ****************************************************************************************************
// SymbolLocator declaration
// ****************************************************************************************************

typedef GenericLocator<Symbol*> SymbolLocator;



// ****************************************************************************************************
// SymbolFilter declaration
// ****************************************************************************************************

typedef GenericFilter<Symbol*> SymbolFilter;



// ****************************************************************************************************
// for_each_symbol declaration
// ****************************************************************************************************

#define for_each_symbol(symbol, symbols)\
/***************************************/\
{\
	SymbolLocator _locator = symbols.GetLocator();\
	while (_locator.IsValid()) {\
		Symbol* symbol = _locator.GetElement();\
		_locator.Progress();



} // End of Hurricane namespace.

#endif // HURRICANE_SYMBOLS

// ****************************************************************************************************
// Copyright (c) BULL S.A. 2000-2004, All Rights Reserved
// ****************************************************************************************************
