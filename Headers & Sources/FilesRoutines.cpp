// ================================================================================================ Beginning of file "FilesRoutines.cpp"
// Copyright (c) 2012-2018, Eric Jacopin, ejacopin@ymail.com
/////////////////////////////////////////////////////////////////////////////////////////////////// File Content
//
//		 GetNextToken/6 --- Get the next identifier or PDDL Keyword
//			 GetArity/7 --- (GetNextToken/6-based) Get the number of parameters of a predicate or a function
//	  ValidPDDLNumber/2 --- Convert a token made of '0' to '9' digit characters + '.' into a number
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////// Inclusion of files
//
#if defined(_MSC_VER)	// _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#pragma once		// When compiling, this file will be included (opened) only once by the Microsoft C++ compiler 
#endif					// _MSC_VER

// Project files
#include "FilesRoutines.h"		// Prototypes visibility

/////////////////////////////////////////////////////////////////////////////////////////////////// GetNextToken/6
//
//	List of in (prefixed by '-') and out (prefixed by '+') parameters:
//
//	- b is the buffer where reading happens
//	- s is the size of the buffer b
//	+ p is the position in the buffer b where reading happens
//	+ l is the number of line which have been read (counting the number of '\10' on windows)
//	+ t is the token constructed by GetNextToken/6
//	+ pl is the parenthesis level at position p in the buffer b (should be 0 when the reading of the buffer b is over)
//
//	Returns the PDDL_Token read from b
//
PDDL_Token GetNextToken(char* b, long s, INTERNAL_TYPES::u16& p, INTERNAL_TYPES::u16& l, PDDL::Identifier& t, short& pl) {

#define ASCII_HORIZONTAL_TAB	 9
#define	ASCII_LINE_FEED			10
#define	ASCII_CARRIAGE_RETURN	13
#define	ASCII_SPACE				32
#define ASCII_SEMI_COLUMN		59

	// Can we get something more from the buffer b?
	if (s <= (long) p)
	{
		return END_OF_FILE;
	}

	// Skipping meaning-less characters
	do {
		while (ASCII_SPACE == b[p])				// Spaces
		{
			if (s <= (long) ++p)
				return END_OF_FILE;
		}
		if (ASCII_SEMI_COLUMN == b[p])			// Comments
			while (ASCII_LINE_FEED != b[p])
			{
				if (s <= (long) ++p)
					return END_OF_FILE;
			}
		while (ASCII_HORIZONTAL_TAB == b[p])	// (horizontal) Tabulations
		{
			if (s <= (long) ++p)
				return END_OF_FILE;
		}
		while ((ASCII_LINE_FEED == b[p]) || (ASCII_CARRIAGE_RETURN == b[p]))  // Newlines
		{
			++l;
			if (s <= (long) ++p)
				return END_OF_FILE;
		}
	}
	while ((ASCII_SPACE == b[p]) || (ASCII_HORIZONTAL_TAB == b[p]) || (ASCII_SEMI_COLUMN == b[p]));

	// Begin constructing next token; don't modify its current value when the end of the buffer b is encountered
	t = "";

	if (':' == b[p])
	{
		// Get a PDDL Keyword and put it in t
		do
		{
			t += b[p];
			if (s <= (long) ++p)
				return END_OF_FILE;
		}
		while ((('a' <= b[p]) && (b[p] <= 'z')) || ('-' == b[p]));

		return KEYWORD;
	}

	if ((('a' <= b[p]) && (b[p] <= 'z')) || (('A' <= b[p]) && (b[p] <= 'Z')))
	{
		// Get a PDDL Identifier and put it in t
		while ((('a' <= b[p]) && (b[p] <= 'z')) || (('A' <= b[p]) && (b[p] <= 'Z')) || (('0' <= b[p]) && (b[p] <= '9')) || ('-' == b[p]) || ('_' == b[p]) || ('.' == b[p]))
		{
			t += b[p];
			if (s <= (long) ++p)
				return END_OF_FILE;
		}

		return IDENTIFIER;
	}

	if (('0' <= b[p]) && (b[p] <= '9'))
	{
		// Get a PDDL Number and put it in t
		bool dot = false;
		while ((('0' <= b[p]) && (b[p] <= '9')) || ('.' == b[p]))
		{
			if ('.' == b[p])
			{
				if (dot)
					return NUMBER;
				else
					dot = true;
			}

			t += b[p];
			if (s <= (long) ++p)
				return END_OF_FILE;

		}

		return NUMBER;
	}

	// Next token t eventually is a meaningful PDDL character:
	t = b[p];	// Note that t was initialized to an empty string in line 75; so t is an empty string if we ever reach here.
	++p;
	// Get various meaningful PDDL characters
	if ("(" == t) { ++pl; return LEFT_PAR; }
	else if (")" == t)
		{ --pl; return RIGHT_PAR; }
	else if ("=" == t)
		{ return EQUAL; }
	else if ("?" == t)
		{ return QUERY; }
	else if ("-" == t)
		{ return DASH; }

	// None of any relevant PDDL information has been detected: there must be something wrong!
	return END_OF_FILE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////// GetArity/7
//
//	List of in (prefixed by '-') and out (prefixed by '+') parameters:
//
//	+ eof is set to END_OF_FILE when the end of file was encountered during reading the arity
//	- b is the buffer where reading happens
//	- s is the size of the buffer b
//	+ p is the position in the buffer b where reading happens
//	+ l is the number of line which have been read (couting the number of '\10' on windows)
//	+ t is the token constructed by GetNextToken/6
//	+ pl is the parenthesis level at position p in the buffer b (should be 0 when the reading of the buffer b is over)
//
// Returns the number of parameters of a predicate or a function
//
Predicate::HArity GetArity(PDDL_Token& eof, char* b, long s, INTERNAL_TYPES::u16& p, INTERNAL_TYPES::u16& l, PDDL::Identifier& t, short& pl) {
	// As types are not taken into account yet, the arity corresponds to the count of '?'
	Predicate::HArity arity = 0;
	do
	{
		// Any parameter is an identifier beginning with a '?'; increase the arity when '?' is detected and stop when reading ')'
		eof = GetNextToken(b, s, p, l, t, pl);
		if (QUERY == eof)
			++arity;
		else if (END_OF_FILE == eof)
			break;
	}
	while (RIGHT_PAR != eof);

	return arity;
}

/////////////////////////////////////////////////////////////////////////////////////////////////// ValidPDDLNumber/2
//
//	List of in (prefixed by '-') and out (prefixed by '+') parameters:
//
//	- t is the string of characters to be converted to a real number (e.g. "123", "123.456")
//	+ f is the real number converted from string t
//
void ValidPDDLNumber(PDDL::Identifier& t, PDDL::Number& f) {
	// Which character in string t are we reading?
	PDDL::Identifier::size_type i = 0;

	// 1. Compute the numerical value as long as no decimal part is encountered
	INTERNAL_TYPES::u16 integral_part = 0;
	bool decimal_part = false;
	for (; i < t.size(); ++i)	// iterator i gets initial value 0 in line 185
	{
		char c = t[i];
		if ('.' == c)
		{
			// The decimal part has just been detected
			decimal_part = true;
			// Goto to the computation of the decimal part
			break;
		}

		// (integral_part<<3)+(integral_part<<1) == (integral_part * 2^3) + (integral_part * 2^1) == (integral_part * 8) + (integral_part * 2) == integral_part * (8 + 2) == (integral_part * 10)
		integral_part = ((integral_part << 3) + (integral_part << 1)) + (c - 48);
	}

	f = integral_part;

	// 2. Compute the numerical value from the decimal part
	if (decimal_part)
	{
		INTERNAL_TYPES::u16 dp = 0;	// numerical value of the decimal part
		INTERNAL_TYPES::u16 diviseur = 1;
		for (++i; i < t.size(); ++i)	// let's reuse iterator i's value from the reading of the integral part 
		{
			// (x<<3) + (x<<1) == (x * 2^3) + (x * 2^1) == (x * 8) + (x * 2) == x * (8 + 2) == x * 10
			dp = ((dp << 3) + (dp << 1)) + (t[i] - 48);
			diviseur = ((diviseur << 3) + (diviseur << 1));
		}
		// Add the decimal part to the integral value
		f += (dp / (PDDL::Number) diviseur);
	}
}
// ================================================================================================ End of file "FilesRoutines.cpp"