// ================================================================================================ Beginning of file "uset.h"
// Copyright (c) 2012-2018, Eric Jacopin, ejacopin@ymail.com
// ================================================================================================ File Content
//
// This file contains the following sections and subsections:
//	- File History					(Line 12)
//  - Inclusion of files			(Line 66)
//	- Namespace declaration			(Line 74)
//	- Comments						(Line 79)
//	- Class declaration				(Line 138)
//
// ================================================================================================ File history
//
// [Author, Created, Last modification] = [Éric JACOPIN, 27/SEP/2011, 16/SEP/2018]
//	Compilers:	MSVC++ 2008, 2010, 2012, 2013 and 2017 (NEITHER TESTED WITH 2003 NOR 2005)
//
//      - (dev 6): VS2017 and comments ------------------------------------------------------------ 16/SEP/2018
//			.Comments improved and corrected
//			.Compiled with MSVC++ 2017 (i.e. compliant with C++11)
//		- (dev 5): 64 bits ------------------------------------------------------------------------ 18/JUL/2013
//			.static const 32 and 5 back to #define
//			.type uint32_t (32 bit unsigned integer) added for correct code under 64 bit compilation
//			.type uint32_t is exported when compiling with VS 2008 (tested) or lower (not tested)
//			.References to unsigned long removed in comments
//		- (dev 4): Manual iterator (bug fixing) --------------------------------------------------- 31/MAY/2013
//			.Comments improved and corrected (e.g. wrong line number in comments)
//			.Bugs fixed in next/1
//				.Due to a missing return statement in next/1, the computation of the next element
//					after the last element of the uset returned a random unsigned integer, possibly
//					causing a loop when enumerating the elements of the uset.
//				.IF you use last/0 to end the scan of all elements of the uset with next/1 THEN keep
//					in mind next(last) returns (++last).
//				.Keep in mind next(element) does not check whether element indeed is a part of the
//					uset.
//			.parameter of next/1 is no longer const
//		- (dev 3): Manual iterator ---------------------------------------------------------------- 06/SEP/2012
//			.Added next/1 which goes from one uset element to the next
//				.This allows to jump from one element to another in the set and not scan all the set
//					.Don't use last/0 but size/0 when scanning all the elements in the set
//		- (dev 2): Exported types ----------------------------------------------------------------- 29/APR/2012
//			.Comments improved
//			.Exporting public type uset::size_type
//				.Consequently, size/0 is now of type uset::size_type
//		- (dev 1): Sets of Unsigned Integers ------------------------------------------------------ 09/JAN/2012
//			.Comments added
//			.Basic operations for handling sets of unsigned integers:
//				.template implementation
//				.unsigned integer set range is [0, (cardinal * 32) - 1]
//					(cardinal is declared as an unsigned char: its maximum value is 255;
//						so the maximum upper bound is (255 * 32) - 1 == 8159)
//					(potential improvement: lower bound > 0)
//				.inlined operations
//					.smallest and greatest elements of the set
//					.number of elements in the set
//					.inserting and removing elements
//					.membership testing
//					.no checking of the set range when calling erase/1, find/1 or insert/1
// ------------------------------------------------------------------------------------------------
#ifndef _SPHG_TOOLS_USET_H
#define _SPHG_TOOLS_USET_H

#if defined(_MSC_VER)	// _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#pragma once		// When compiling, this file will be included (opened) only once by the Microsoft C++ compiler 
#endif					// _MSC_VER

/////////////////////////////////////////////////////////////////////////////////////////////////// Inclusion of files
//
#include <stdlib.h>								// Visibility for operation div/2 and type div_t
#if !defined(_MSC_VER) || (_MSC_VER >= 1600)	// At least Microsoft Visual Studio 2010 or something else (e.g. g++)
	#include <cstdint>							// Visibility for int32_t
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////// Namespace declaration
//
namespace SPHG		{	// Simple Planning, Happy Gaming!


/////////////////////////////////////////////////////////////////////////////////////////////////// Comments
//
// The purpose of this file is to provide a template class implementing sets of unsigned integers
// where:
//		- the left bound of these sets is 0
//		- the right bound of these sets is a multiple of 32 minus 1 (e.g. 31, 63, 95, ...)
//		- the largest possible right bound is 8159 (== (32 * 255) - 1) as the cardinal of a set
//			is declared as an unsigned char (i.e. 255 is the maximum value of an unsigned char)
//
//	.As an example, assume the following declaration:
//
//			SPHG::uset<short, 14>   my_uset;
//
//		Where:
//			- SPHG is the namespace provided with this file
//			- type short ranges from -32768 to 32767 (Microsoft Visual Studio C++)
//			- uset<short, 14> implements the following set range: [0, 447 (== (32 * 14) - 1)]
//			- my_uset is a subset of the range [-32768, 32767] based on the type short
//
// A set of unsigned integers is here implemented as 2-parameter template class:
//
//			template <class N, unsigned char cardinal>
//
//	.The first parameter, N (Natural Numbers), is the (positive) integer class for the elements of the sets
//	.The second parameter, cardinal, is the minimum number of slots in order to store a set:
//		A slot is the number of 32 bit integers used to implement the set.
//
//		A set of integers can be implemented as an array of booleans:
//			true, the integer corresponding to the position in the array is an element of the set,
//			false, the integer corresponding to the position in the array is not element of the set.
//
//		However, we here implement a set as an array of unsigned 32 bit integers thus allowing blocks of
//	    32 elements:
//			The first (unsigned) integer stores the integers from 0 to 31,
//			The second (unsigned) integer stores the integers from 32 to 63,
//			The third (unsigned) integer stores the integers from 64 to 95,
//			...
//			The cardinal-th (unsigned) integer stores the integers from ((cardinal-1) * 32) to ((cardinal) * 32) - 1.
//
//		The value of the unsigned integers tells about the elements of the set. For instance,
//		when the value of an unsigned integer is 0, the set of consecutive integers represented by
//		this unsigned integer is empty: for instance, if the value of the second unsigned integer is 0
//		then integers from 32 to 63 do not belong to the set.
//
//		Otherwise the bits representing the value of the unsigned integer tells us about the elements
//		of the set: for instance, let the first unsigned integer be equal to 4 196 385; we have:
//			4 196 385 = (1 * 2^0) + (1 * 2^5) + (1 * 2^11) + (1 * 2^22) 
//		which tells us that 0, 5, 11 and 22 are elements of the set. If 4 196 384 is the second
//		unsigned integer of the set, then (32+0=) 32, (32+5=) 37, (32+11=) 43 and (32+22=) 54 are
//		elements of the set. And so on.
//
//		Each unsigned integer of the array allows for various exponent values and thus a greater
//		number of elements. For instance, 8 unsigned integers allow to represent sets of 8 * 32 = 256 
//		elements. Declared as an unsigned char, the maximum value for cardinal is 255; consequently,
//		the maximum number of elements in a uset is 255 * 32 = 8160.
//
// ------------------------------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////////////////////////// Class declaration
//
template <class N, unsigned char cardinal> class uset {
	// ----- Constants
	// Changing the value of one of the following IMPLIES changing the value of the other
	#define	NUMBER_OF_BITS_PER_UNSIGNED_INTEGER		32		// == (number of bits of uint32_t) == 32
	#define EXPONENT								5		// NUMBER_OF_BITS_PER_UNSIGNED_INTEGER == 32 == 2^5 == 2^EXPONENT


	// ----- Types
	public:
		typedef	N		size_type;				// N (whose max value may be much smaller than 8160, e.g. 255 when N is unsigned char) is better than any type able to represent range [0,8160] (see above comments)

#if defined(_MSC_VER) && (_MSC_VER < 1600)		// At most Microsoft Visual Studio 2008
		typedef (unsigned __int32)	uint32_t;
#endif			

	// ----- Properties
	private:
		uint32_t		theSet[cardinal];		// sizeof(uint32_t) == (32 bits)


	// ----- Constructors
	public:
		uset() {
			// A set is created empty
			for(unsigned char i = 0; i < cardinal; ++i)
				theSet[i] = 0;
		}
		uset(const uset& right) {
			// 32 bit unsigned integer to 32 bit unsigned integer copy
			for(unsigned char i = 0; i < cardinal; ++i)
				theSet[i] = right.theSet[i];
		};


	// ----- Operations
	public:
		inline N first() const {
			// Move from position zero until a non zero bit is found and return the corresponding position
			for (unsigned char s = 0; s < cardinal; ++s)
				// Skip empty parts of the set
				if (0 != theSet[s])
					for (unsigned char p = 0; p < NUMBER_OF_BITS_PER_UNSIGNED_INTEGER; ++p)
						if ((theSet[s] >> p) & 1)
							return ((s << EXPONENT /* == s * NUMBER_OF_BITS_PER_UNSIGNED_INTEGER */) + p);

			// IF the set is empty THEN
			//		return an impossible value; here, return (upper bound + 1)
			//		please note that upper bound == cardinal * sizeof(unsigned 32 bit integer) == cardinal * 32
			// IF the set is empty AND class N cannot represent (upper bound + 1) THEN
			//		first/0's returned value is wrong (bug here)
			//		Consequently:
			//			- either check whether upper bound + 1 can be represented
			//			- or else use size/0 to test whether the set is empty
			return (N) (cardinal << EXPONENT /* == cardinal * NUMBER_OF_BITS_PER_UNSIGNED_INTEGER */);
		}
		inline N last() const {
			// Move from largest position until a non zero bit is found and return the corresponding position
			for (char s = (cardinal - 1); s > -1; --s)
				// Skip empty parts of the set
				if (0 != theSet[s])
					for (short p = (NUMBER_OF_BITS_PER_UNSIGNED_INTEGER - 1); p > -1; --p)
						if ((theSet[s] >> p) & 1)
							return ((s << EXPONENT /* == s * NUMBER_OF_BITS_PER_UNSIGNED_INTEGER */) + p);
			// Return (0) when this set is empty
			return (0);
		}
		inline N next(N element) const {
			// !!! next/1 doesn't check whether element is part of this set !!!
			// Move to element's position to next member in this set, as long as (element < last); return (++last) otherwise:

			// Compute both the corresponding unsigned integer and the corresponding bit
			div_t r = div(++element, NUMBER_OF_BITS_PER_UNSIGNED_INTEGER);

			// First check the rest of theSet[r.quot]
			unsigned char s = r.quot;
			if (0 != theSet[s])
				for (unsigned char p = r.rem; p < NUMBER_OF_BITS_PER_UNSIGNED_INTEGER; ++p)
					if ((theSet[s] >> p) & 1)
						return ((s << EXPONENT /* == s * NUMBER_OF_BITS_PER_UNSIGNED_INTEGER */) + p);
			// Scan the rest of theSet for the next element
			for (++s; s < cardinal; ++s)
				// Skip empty parts of the set
				if (0 != theSet[s])
					for (unsigned char p = 0; p < NUMBER_OF_BITS_PER_UNSIGNED_INTEGER; ++p)
						if ((theSet[s] >> p) & 1)
							return ((s << EXPONENT /* == s * NUMBER_OF_BITS_PER_UNSIGNED_INTEGER */) + p);
			// Return (++element -- element was incremented in line 211 above) when no next element can be found
			return (element);
		}
		inline size_type size() const {
			// Compute the number of elements in this set
			size_type theSize = 0;
			for (unsigned char s = 0; s < cardinal; ++s)
				// Skip empty parts of the set
				if (0 != theSet[s])
					for (unsigned char p = 0; p < NUMBER_OF_BITS_PER_UNSIGNED_INTEGER; ++p)
						theSize += ((theSet[s] >> p) & 1);

			return theSize;
		}
		inline bool find(const N element) const {
			// !!! find/1 doesn't check whether element is part of this set !!!
			// Compute both the corresponding unsigned integer and the corresponding bit
			div_t r = div(element, NUMBER_OF_BITS_PER_UNSIGNED_INTEGER);
			// Return the value of the bit corresponding to element
			return ((theSet[r.quot] >> r.rem) & 1);
		}
		inline void insert(const N element) {
			// !!! insert/1 doesn't check whether element is part of this set !!!
			// Compute both the corresponding unsigned integer and the corresponding bit
			div_t r = div(element, NUMBER_OF_BITS_PER_UNSIGNED_INTEGER);
			// Set the position of element to 1
			theSet[r.quot] |= (1 << r.rem);
		}
		inline void erase(const N element) {
			// !!! erase/1 doesn't check whether element is part of this set !!!
			// Compute both the corresponding unsigned integer and the corresponding bit
			div_t r = div(element, NUMBER_OF_BITS_PER_UNSIGNED_INTEGER);
			// Set the position of element to 0
			theSet[r.quot] &= ~(1 << r.rem);
		}
};

}						// End of Namespace SPHG (Simple Planning, Happy Gaming)
#endif					// _SPHG_TOOLS_USET_H
// ================================================================================================ End of file "uset.h"