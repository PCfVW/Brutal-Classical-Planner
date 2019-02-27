// ================================================================================================ Beginning of file ConditionalCompiling.h
// Copyright (c) 2012-2019, Eric Jacopin, ejacopin@ymail.com
// ------------------------------------------------------------------------------------------------
#ifndef _SPHG_PLANNER_CONDITIONAL_COMPILING_H
#define _SPHG_PLANNER_CONDITIONAL_COMPILING_H

#if defined(_MSC_VER)	// _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#pragma once		// When compiling, this file will be included (opened) only once by the Microsoft C++ compiler 
#endif					// _MSC_VER


/////////////////////////////////////////////////////////////////////////////////////////////////// File Content
//
//	There are four sections in this file:
//		- Section 0 (starts line 42)  defines 1 Windows-only constant so as to accelerate project generation
//		- Section 1 (starts line 55)  defines 2 macros used to produce strings for #pragma messages
//		- Section 2 (starts line 63)  defines 6 constants for the various memory schemes of this project
//		- Section 3 (starts line 161) defines 2 constants for the configuration type of this project
//
//	The 9 following constants (alphabetical order) are #define'd in this file:
//		- CLASSIC						// (defined in Section 2) Value for MEMORY_ALLOCATION
//		- DL_MALLOC						// (defined in Section 2) Value for MEMORY_ALLOCATION
//		- HPHA							// (defined in Section 2) Value for MEMORY_ALLOCATION
//		- MAXIMUM_NUMBER_OF_PREDICATES	// (defined in section 3) User defined: How many predicates does this application need?
//		- MEMORY_ALLOCATION				// (defined in Section 2) Automatically defined to one element of the set {CLASSIC, DL_MALLOC, HPHA, STL}, according to _DEBUG and _WIN32
//		- MEMORY_ALLOCATION_STR			// (defined in Section 2) String value of MEMORY_ALLOCATION, used in #pragma messages
//		- NUMBER_OF_SLOTS_IN_A_USET		// (defined in section 3) Automatically computed from MAXIMUM_NUMBER_OF_PREDICATES and SLOT_SIZE_IN_A_USET
//		- SLOT_SIZE_IN_A_USET			// (defined in section 3) (cf. comments line 174) How many bits are used to implement a SLOT?
//		- STL							// (defined in Section 2) Value for MEMORY_ALLOCATION
//
//	_DEBUG and _WIN32 are C/C++ Preprocessor commands defined by Microsft's Visual Studio.
//
//  In Release mode, SPHG_USE_CLASSIC or else SPHG_USE_DL_MALLOC or else SPHG_USE_HPHA must be defined as C/C++
//		preprocessor commands according to the needs of this project (cf. comments in Section 2 below).
//
//  ONLY the value of MAXIMUM_NUMBER_OF_PREDICATES can be changed to suit the needs of this project
//		(cf. line 165 below).
//
///////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////// Section 0
//
//	Accelerating the generation of this project on Windows platforms:
//
#if defined(_WIN32)
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
#endif
//
/////////////////////////////////////////////////////////////////////////////////////////////////// End of Section 0


/////////////////////////////////////////////////////////////////////////////////////////////////// Section 1
//
#define STRING2(x)			#x
#define STRING(x)			STRING2(x)
//
/////////////////////////////////////////////////////////////////////////////////////////////////// End of Section 1


/////////////////////////////////////////////////////////////////////////////////////////////////// Section 2
//
//	Automatic decision of a MEMORY_ALLOCATION scheme with respect to the target Operating System:
//		- WINDOWS:
//			- In _DEBUG mode: STL structures
//			- NOT in _DEBUG mode: C-Style arrays through a High Performance Heap Allocator
//		- NOT WINDOWS (e.g. LINUX):
//			- In _DEBUG mode: STL structures
//			- NOT in _DEBUG mode: C-Style arrays through C-Style malloc, free, etc.
//
//	Currently, the choice (C-style arrays OR ELSE STL structures) concerns memory allocation for
//	the following structures (the structures listed below are arrays of (unsigned) integers):
//		- predicate parameters
//		- operator parameters 
//		- (integer pointers to) the predicates (preconditions, added and deleted) of an operator;
//			all the predicates of an operator are thus stored as an array of integers
//		- (C) building a mapping from the identifier of a precondition predicate of an operator to
//			the set of (integer pointers to) predicates (of the current situation where we try
//			to apply this operator)	with the same identifier
//		- (D) a set of (integer pointers to) predicates from the current situation which can unify
//			with the preconditions of the operator we try to apply
//		- (P) an array of integer values pointing to (integer pointers to) predicates
//
//	C, D and P are local variables declared in StateSpaceForwardChaining::BreadthExpand/0 and 
//  CostExpand/0 (cf. file StateSpaceForwardChaining.cpp)
//
//	WHEN this project is in _DEBUG mode
//		STL structures are automatically included so we can spy the values they store, such as:
//		the parameters of a predicate, the parameters and the predicates of an operator, C, D and P.
//
//	WHEN this project is in _WIN32 mode (and NOT in _DEBUG mode)
//		A memory allocator is automatically included and used; the purpose of such an allocator is
//		performance. Two allocators are currently provided (currently, only (2) is possible due to copyrights):
//			(1) Dimitar Lazarov's High Performance Heap Allocator (HPHA)
//			(2) Doug Lea's DL_Malloc.
//		By defining either SPHG_USE_HPHA or else SPHG_USE_DL_MALLOC, allocator (1) or else (2) is used, respectively.
//		You still can use malloc/free instead of the provided allocators by defining SPHG_USE_CLASSIC.
//
//	WHEN this project is neither in _DEBUG nor _WIN32 mode
//		Classical malloc/free routines are used; debugging is difficult but performance is better
//		than STL.
//
#define CLASSIC							2001		// C-style arrays are managed through CLASSICal routines to allocate and free memory: malloc and free.
#define	DL_MALLOC						2002		// C-style arrays are managed through Doug Lea's High Performance Heap Allocator (cf. files DL_Malloc.h and DL_Malloc.cpp)
#define HPHA							2003		// UNAVAILABLE DUE TO COPYRIGHTS // C-style arrays are managed through Dimitar Lazarov's High Performance Heap Allocator (cf. files hpha.h and hpha.cpp)
#define STL								2004		// STL vectors, valarrays, ... ==> STL Allocators (no special Allocator is provided)

#if defined(_DEBUG)
	#define MEMORY_ALLOCATION			STL			// When debugging this project, whatever the Operating System, STL code is included and used
	#define SPHG_ALLOC					malloc		// Used to allocate memory in DomainFile.cpp(91) and ProblemFile.cpp(90)
	#define SPHG_FREE					free		// Used to free memory in DomainFile.cpp(1268) and ProblemFile.cpp(802)
#elif defined(_WIN32)
	// Either SPHG_USE_HPHA or else SPHG_USE_CLASSIC or else SPHG_USE_DL_MALLOC must be defined; otherwise trigger an #error (cf. line 135)
	#if defined(SPHG_USE_HPHA) && !defined(SPHG_USE_CLASSIC) && !defined(SPHG_USE_DL_MALLOC)
		#define MEMORY_ALLOCATION		HPHA		// When the Operating System IS Microsoft Windows, High Performance Heap Allocator code is included and used
		#include "hpha.h"							// Dimitar Lazarov's High Performance Heap Allocator for Operators and Predicates
		static hpha::allocator			gAllocator;	// Store house for arrays of integers acting as pointers to positions in arrays of PDDL planning data (such as identifiers, predicates, operators, parameters, ...)
		#define SPHG_ALLOC				gAllocator.alloc
		#define SPHG_FREE				gAllocator.free
		#define SPHG_REALLOC			gAllocator.realloc
	#elif defined(SPHG_USE_DL_MALLOC) && !defined(SPHG_USE_CLASSIC) && !defined(SPHG_USE_HPHA)
		#define MEMORY_ALLOCATION		DL_MALLOC	// When the Operating System IS Microsoft Windows, and HPHA is not wanted, then Doug Lea's Allocator's code is included and used
		#include "DL_Malloc_2_8_6.h"				// Doug Lea's malloc/free for Operators and Predicates and ...
		#define SPHG_ALLOC				dlmalloc
		#define SPHG_FREE				dlfree
		#define SPHG_REALLOC			dlrealloc
	#elif defined(SPHG_USE_CLASSIC) && !defined(SPHG_USE_HPHA) && !defined(SPHG_USE_DL_MALLOC)
		#define MEMORY_ALLOCATION		CLASSIC		// When the Operating System IS Microsft Windows and neither DL's Malloc nor HPHA is used, then use classic Malloc/Free operations
		#define SPHG_ALLOC				malloc
		#define SPHG_FREE				free
		#define SPHG_REALLOC			realloc
	#else											// The three following: SPHG_USE_HPHA, SPHG_USE_DL_MALLOC and SPHG_USE_CLASSIC are undefined; what do you want to do?
		#pragma message (__FILE__ "(" STRING(__LINE__) "):  >> MEMORY_ALLOCATION = UNDEFINED (Either SPHG_USE_CLASSIC or else SPHG_USE_HPHA or else SPHG_USE_DL_MALLOC can be defined on this Windows version of BCP.")
		#error MEMORY_ALLOCATION
	#endif
#else												// (!defined(_DEBUG) && !defined(_WIN32)) When it is NOT debug mode and it is NOT windows
	#define MEMORY_ALLOCATION			CLASSIC		// Whatever the Operating System BUT Microsoft Windows, use the classical routines to allocate and free memory: malloc and free
#endif

// ------------------------------------------------------------------------------------------------
//		Assign a string accordingly with the MEMORY_ALLOCATION in order to inform the user
#if (CLASSIC == MEMORY_ALLOCATION)
	#define MEMORY_ALLOCATION_STR	"CLASSIC"
#elif (DL_MALLOC == MEMORY_ALLOCATION)
	#define MEMORY_ALLOCATION_STR	"DL_MALLOC"
#elif (HPHA == MEMORY_ALLOCATION)
	#define MEMORY_ALLOCATION_STR	"HPHA"
#elif (STL == MEMORY_ALLOCATION)
	#define MEMORY_ALLOCATION_STR	"STL"
#else // (CLASSIC != MEMORY_ALLOCATION) && (HPHA != MEMORY_ALLOCATION) && (STL != MEMORY_ALLOCATION)
	#pragma message (__FILE__ "(" STRING(__LINE__) "):  >> MEMORY_ALLOCATION = UNKNOWN")
	#error MEMORY_ALLOCATION
#endif
#pragma message ("  >> MEMORY_ALLOCATION = " MEMORY_ALLOCATION_STR)	// Report on the chosen memory allocation scheme
//
/////////////////////////////////////////////////////////////////////////////////////////////////// End of Section 2


/////////////////////////////////////////////////////////////////////////////////////////////////// Section 3
//
// Change the value of MAXIMUM_NUMBER_OF_PREDICATES at your convenience (integer value > 0)
//
#define	MAXIMUM_NUMBER_OF_PREDICATES	200														// (Predicates declared in the PDDL files) PLUS (predicates created during search when applying an operator)
#if (MAXIMUM_NUMBER_OF_PREDICATES <= 0)
	#pragma message (__FILE__ "(" STRING(__LINE__) "): (Error: Negative value) MAXIMUM_NUMBER_OF_PREDICATES must be an integral value strictly greater than 0.")
	#error MAXIMUM_NUMBER_OF_PREDICATES
#endif

// ------------------------------------------------------------------------------------------------
//
// Do NOT change any of the following:
//
#define SLOT_SIZE_IN_A_USET				32														// Why 32? Because a slot is implemented with unsigned 32-bit integers and sizeof(uint32_t) == 32; (cf. uset.h)
#define NUMBER_OF_SLOTS_IN_A_USET		(MAXIMUM_NUMBER_OF_PREDICATES / SLOT_SIZE_IN_A_USET)	// Compute the number of slots for the template class uset

#if (SLOT_SIZE_IN_A_USET < 0)
	#pragma message (__FILE__ "(" STRING(__LINE__) "): (Error: Negative value) SLOT_SIZE_IN_A_USET (" STRING(NUMBER_OF_SLOTS_IN_A_USET) ") cannot be negative.")
	#error SLOT_SIZE_IN_A_USET
#elif (0 == NUMBER_OF_SLOTS_IN_A_USET)															// Happens when (0 < MAXIMUM_NUMBER_OF_PREDICATES < SLOT_SIZE_IN_A_USET)
	#undef	NUMBER_OF_SLOTS_IN_A_USET
	#define NUMBER_OF_SLOTS_IN_A_USET	1														// The minimum value to store less than 32 elements in a set
	#pragma message (__FILE__ "(" STRING(__LINE__) "): (Warning) SLOT_SIZE_IN_A_USET(" STRING(SLOT_SIZE_IN_A_USET) ") may be too large for MAXIMUM_NUMBER_OF_PREDICATES(" STRING(MAXIMUM_NUMBER_OF_PREDICATES) ")")
#endif
//
/////////////////////////////////////////////////////////////////////////////////////////////////// End of Section 3

#endif  // _SPHG_PLANNER_CONDITIONAL_COMPILING_H
// ================================================================================================ End of file ConditionalCompiling.h