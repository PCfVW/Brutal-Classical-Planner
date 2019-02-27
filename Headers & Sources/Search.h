// ================================================================================================ Beginning of file "Search.h"
// Copyright (c) 2012-2019, Eric Jacopin, ejacopin@ymail.com
// ------------------------------------------------------------------------------------------------
#ifndef _SPHG_SEARCH_FACADE_H
#define _SPHG_SEARCH_FACADE_H

#if defined(_MSC_VER)	// _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#pragma once		// When compiling, this file will be included (opened) only once by the Microsoft C++ compiler 
#endif					// _MSC_VER

// ------------------------------------------------------------------------------------------------ Inclusion of files
// STD and STL files
#include <stddef.h>		// Visibility for size_t


// ------------------------------------------------------------------------------------------------ Class definition
class Search {
	public:
		virtual size_t SizeOf() = 0;
};

#endif	// _SPHG_SEARCH_FACADE_H
// ================================================================================================ End of file "Search.h"
