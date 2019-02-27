// ================================================================================================ Beginning of file "FilesRoutines.h"
// Copyright (c) 2012-2019, Eric Jacopin, ejacopin@ymail.com
// ------------------------------------------------------------------------------------------------
#ifndef _SPHG_PDDL_FILES_ROUTINES
#define _SPHG_PDDL_FILES_ROUTINES
//
/////////////////////////////////////////////////////////////////////////////////////////////////// Inclusion of files
//
#if defined(_MSC_VER)	// _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#pragma once		// When compiling, this file will be included (opened) only once by the Microsoft C++ compiler 
#endif					// _MSC_VER

// Project files
#ifndef _INTERNAL_TYPES_H
	#include "InternalTypes.h"		// Visibility for u8, u16 and f32
#endif

#ifndef _SPHG_PDDL_H
	#include "PDDL.h"				// Visibility for PDDL::Identifier
#endif

#ifndef _SPHG_PLANNER_PREDICATE_H
	#include "Predicate.h"			// Visibility for Predicate::HArity
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////// File Content
//
//		 GetNextToken/6 --- Get the next identifier or PDDL Keyword
//			 GetArity/7 --- (GetNextToken/6-based) Get the number of parameters of a predicate or a function
//	  ValidPDDLNumber/2 --- Convert a token made of '0' to '9' digit characters and '.' into a number
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
enum PDDL_Token : INTERNAL_TYPES::u8 {	// PDDL tokens of interest for parsing PDDL domain and problem files (alphabetical order)
										DASH,		// '-'
										DOT,		// '.'
										END_OF_FILE,// Special character detected, file size reached or no PDDL info could be read
										EQUAL,		// '='
										IDENTIFIER,	// One alphabetical followed by a sequence of alpha-numericals, '-' or '.'
										KEYWORD,	// begins with ':' followed by lower case alphabeticals or '-'
										LEFT_PAR,	// '('
										NUMBER,		// e.g. 123 or 123.456; a 4-byte float
										QUERY,		// '?'
										RIGHT_PAR	// ')'
									 };

PDDL_Token GetNextToken(char* b, long s, INTERNAL_TYPES::u16& p, INTERNAL_TYPES::u16& l, PDDL::Identifier& t, short& pl);

Predicate::HArity GetArity(PDDL_Token& t, char* b, long s, INTERNAL_TYPES::u16& p, INTERNAL_TYPES::u16& l, PDDL::Identifier& t_s, short& pl);

void ValidPDDLNumber(PDDL::Identifier& t, PDDL::Number& f);

#endif					// _SPHG_PDDL_FILES_ROUTINES
// ================================================================================================ End of file "FilesRoutines.h"