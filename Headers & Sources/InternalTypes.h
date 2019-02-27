// ================================================================================================ Beginning of file "InternalTypes.h"
// Copyright (c) 2012-2019, Eric Jacopin, ejacopin@ymail.com
// ------------------------------------------------------------------------------------------------
#ifndef _INTERNAL_TYPES_H
#define _INTERNAL_TYPES_H

#if defined(_MSC_VER)	// _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#pragma once		// When compiling, this file will be included (opened) only once by the Microsoft C++ compiler 
#endif					// _MSC_VER

#if !defined(_MSC_VER) || (_MSC_VER >= 1600)	// At least Microsoft Visual Studio 2010 or something else (e.g. g++)
	#include <cstdint>							// Visibility for std::int16_t and std::int32_t
#endif

namespace INTERNAL_TYPES	{
// ------------------------------------------------------------------------------------------------
//	NOTE: In order to optimize the memory footprint, PDDL::HPredicate is either an u8 or else
//	an u16, according to the value of MAXIMUM_NUMBER_OF_PREDICATES (cf. file "PDDL.h" for details)
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
//
// Application types based on the following unsigned integer u8:
//
//			Domain::HProblem		;; at most 256 problems for the same PDDL domain
//			Operator::HArity		;; at most 256 parameters for one operator
//			Operator::HPredicate	;; at most 256 predicates in one operator
//			PDDL::HIdentifier		;; at most 256 shared (predicate, operator, parameter) identifiers
//			PDDL::HPredicate		;; at most 256 shared predicates (declared in the :predicates section of a PDDL domain) + (shared) instantiated predicates during search
//			PDDL::HRequirement		;; at most 256 PDDL requirements
//			Plan::length_type		;; at most 256 operators in a plan
//			Predicate::HArity		;; at most 256 parameters for one predicate
//
typedef		unsigned char			u8;			// [0, (2^8) - 1 = 255] At most 256 distinct values

// ------------------------------------------------------------------------------------------------
//
// Application types based on the following unsigned integer u16:
//
//			PDDL::HPredicate,		;; at most 65536 shared predicates (declared in the :predicates section of a PDDL domain) + (shared) instantiated predicates during search
//
typedef		std::uint16_t			u16;		// [0, (2^16) - 1 = 65535] At most 65536 distinct values

// ------------------------------------------------------------------------------------------------
//
// Application types based on the following unsigned integer u32:
//
//			StateSpaceForwardChaining::IterationNumber	;; at most 4294967296 search iterations
//			StateSpaceForwardChaining::size_type		;; at most 4294967296 search memory bytes
//
typedef		std::uint32_t			u32;		// [0, (2^32) - 1 = 4294967295] At most 4294967296 distinct values

// ------------------------------------------------------------------------------------------------
//
// Application types based on the following unsigned integer f32:
//
//			PDDL::Number			;; Numeric costs when :action-costs is required
//
typedef		float					f32;		// 3.4E +/- 38 (7 digits)

};		// INTERNAL_TYPES

#endif	// _INTERNAL_TYPES_H
// ================================================================================================ End of file "InternalTypes.h"