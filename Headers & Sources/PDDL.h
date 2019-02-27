// ================================================================================================ Beginning of file "PDDL.h"
// Copyright (c) 2012-2019, Eric Jacopin, ejacopin@ymail.com
// ------------------------------------------------------------------------------------------------
#ifndef _SPHG_PDDL_H
#define _SPHG_PDDL_H

#if defined(_MSC_VER)	// _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#pragma once		// When compiling, this file will be included (opened) only once by the Microsoft C++ compiler 
#endif					// _MSC_VER

// ------------------------------------------------------------------------------------------------ Inclusion of files
// STD and STL files
#include <map>			// The requirement store
#include <string>		// Any PDDL Identifier + Require Keys
#include <vector>		// STL Store house for various structures (Identifiers, Predicates, etc)

// Project files
#ifndef _SPHG_PLANNER_CONDITIONAL_COMPILING_H
	#include "ConditionalCompiling.h"	// Visibility for STRING
#endif

#ifndef _INTERNAL_TYPES_H
	#include "InternalTypes.h"			// Visibility for u8 and u16
#endif

// ------------------------------------------------------------------------------------------------ Forward declaration
class Domain;

// ------------------------------------------------------------------------------------------------ Class definition
class PDDL {
	// ----- Types for all planning seasons
	protected:
		typedef			INTERNAL_TYPES::u8		HRequirement;					// [0, (2^8) - 1 = 255] At most 256 PDDL requirements (PDDL 3.1 defines 20 distinct requirements; see section "Requirements" below)
		typedef			std::string				RequireKey;						// Requirements appear in the :requirements section of a PDDL Domain or Problem file (cf. the PDDL 3.1 document)

		enum	REQUIREMENTS													// The 21 PDDL 3.1 requirements ordered as in the PDDL 3.1 document (see this document for their meaning)
#if (_MSC_VER >= 1500)															// Tested with MSVC++ 2008, 2010 and 2012
						: HRequirement	/* < 256 distinct requirements */
#endif
						{strips, typing, negative_preconditions, disjunctive_preconditions,
							equality, existential_preconditions, universal_preconditions,
							quantified_preconditions, conditional_effects,
							fluents, object_fluents, numeric_fluents,
							adl, durative_actions, duration_inequalities, continuous_effects,
							derived_predicates, timed_initial_literals, preferences, constraints,
							action_costs};

	public:
		typedef			std::string				Identifier;						// Used for predicate parameters, operator parameters and string values (i.e. any string constants)
		typedef			INTERNAL_TYPES::u8		HIdentifier;					// [0, (2^8) - 1 = 255] A domain possess at most 256 identifiers
		typedef			INTERNAL_TYPES::f32		Number;							// <number> from PDDL 3.1

		enum			SORT													// (Predicate or Operator) Parameter qualification: is it a constant OR can it be given a value?
#if (_MSC_VER >= 1500)															// Optimizing the memory footprint of parameters (of predicates and of actions) sorts
							: INTERNAL_TYPES::u8								// At most 256 distinct SORTs
#endif
							{STRING_VALUE, VARIABLE};							

		typedef			std::pair<PDDL::SORT, PDDL::HIdentifier>	Parameter;	// One parameter of an Operator or a Predicate
#if (STL == MEMORY_ALLOCATION)
		typedef			std::vector<PDDL::Parameter>&				Parameters;	// Data Store for the parameters of an Operator or a predicate
#elif (DL_MALLOC == MEMORY_ALLOCATION) || (HPHA == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION)
		typedef			PDDL::Parameter*							Parameters;	// Data Store for the parameters of an Operator or a predicate
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif

// Optimizing the memory footprint of integer pointers to Predicates
#if defined(MAXIMUM_NUMBER_OF_PREDICATES)  /* defined in "ConditionalCompiling.h" */
	#if (MAXIMUM_NUMBER_OF_PREDICATES <= 0)
		#pragma message (__FILE__ "(" STRING(__LINE__) "): Numeric value <" STRING(MAXIMUM_NUMBER_OF_PREDICATES) "> for MAXIMUM_NUMBER_OF_PREDICATES must be strictly positive.")
		#pragma message (__FILE__ "(" STRING(__LINE__) "): Type PDDL::HPredicate is undefined.")
		#error MAXIMUM_NUMBER_OF_PREDICATES
	#elif ((0 < MAXIMUM_NUMBER_OF_PREDICATES) && (MAXIMUM_NUMBER_OF_PREDICATES <= 255))
			// There shall be at most 255 predicates in a state (Why 255 and not 256? Because ++i == 0 when i == 255 thus entailing an infinite loop: u8 i ALWAYS is less than 2^8 == 256)
			typedef		INTERNAL_TYPES::u8			HPredicate;						// An integer pointer to at most 254 Predicates
			#pragma message ("  >> MAXIMUM_NUMBER_OF_PREDICATES = " STRING(MAXIMUM_NUMBER_OF_PREDICATES) " (=> HPredicate is 8 bits)")
	#elif ((255 < MAXIMUM_NUMBER_OF_PREDICATES) && (MAXIMUM_NUMBER_OF_PREDICATES <= 8160))
			// Due to the implementation of the templace class uset, there can be at most 255 * 32 = 8160 elements in a set; we can't declare u13 (2^13 == 8192) so we use u16 below:
			typedef		INTERNAL_TYPES::u16			HPredicate;						// An integer pointer to at most 65536 Predicates
			#pragma message ("  >> MAXIMUM_NUMBER_OF_PREDICATES = " STRING(MAXIMUM_NUMBER_OF_PREDICATES) " (=> HPredicate is 16 bits)")
	#else // (8160 < MAXIMUM_NUMBER_OF_PREDICATES)
		#pragma message (__FILE__ "(" STRING(__LINE__) "): Numeric value <" STRING(MAXIMUM_NUMBER_OF_PREDICATES) "> for MAXIMUM_NUMBER_OF_PREDICATES is too big.")
		#pragma message (__FILE__ "(" STRING(__LINE__) "): Type PDDL::HPredicate is undefined.")
		#error MAXIMUM_NUMBER_OF_PREDICATES
	#endif
#else	// MAXIMUM_NUMBER_OF_PREDICATES is undefined
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MAXIMUM_NUMBER_OF_PREDICATES) "> for MAXIMUM_NUMBER_OF_PREDICATES (cf. ConditionalCompiling.h)).")
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Type PDDL::HPredicate is undefined.")
	#error MAXIMUM_NUMBER_OF_PREDICATES 
#endif

		typedef			std::map<PDDL::Identifier, Domain*>		StoreHouse;		// Find a domain by its name


	// ----- Properties
	protected:
		static const	HRequirement			NUMBER_OF_REQUIREMENTS = 21;	// There are 21 requirements in PDDL 3.1

	private:
		std::map<PDDL::RequireKey, std::pair<PDDL::REQUIREMENTS, bool> > theRequirements;	// Data store for the PDDL :requirements


	// ----- Constructors
	public:
		PDDL() {	// Set theRequirements accepted by this implementation (An implemented requirement corresponds to /* true */)
			// The 21 :requirements of the PDDL 3.1 document, listed in alphabetical order:
/* true */	theRequirements[":action-costs"]				= std::pair<PDDL::REQUIREMENTS, bool>(action_costs, true);
			theRequirements[":adl"]							= std::pair<PDDL::REQUIREMENTS, bool>(adl, false);
			theRequirements[":conditional-effects"]			= std::pair<PDDL::REQUIREMENTS, bool>(conditional_effects, false);
			theRequirements[":constraints"]					= std::pair<PDDL::REQUIREMENTS, bool>(constraints, false);
			theRequirements[":continuous-effects"]			= std::pair<PDDL::REQUIREMENTS, bool>(continuous_effects, false);
			theRequirements[":derived-predicates"]			= std::pair<PDDL::REQUIREMENTS, bool>(derived_predicates, false);
			theRequirements[":disjunctive-preconditions"]	= std::pair<PDDL::REQUIREMENTS, bool>(disjunctive_preconditions, false);
			theRequirements[":durative-actions"]			= std::pair<PDDL::REQUIREMENTS, bool>(durative_actions, false);
			theRequirements[":duration-inequalities"]		= std::pair<PDDL::REQUIREMENTS, bool>(duration_inequalities, false);
			theRequirements[":equality"]					= std::pair<PDDL::REQUIREMENTS, bool>(equality, false);
			theRequirements[":existential-preconditions"]	= std::pair<PDDL::REQUIREMENTS, bool>(existential_preconditions, false);
			theRequirements[":fluents"]						= std::pair<PDDL::REQUIREMENTS, bool>(fluents, false);
/* true */	theRequirements[":negative-preconditions"]		= std::pair<PDDL::REQUIREMENTS, bool>(negative_preconditions, true);
			theRequirements[":numeric-fluents"]				= std::pair<PDDL::REQUIREMENTS, bool>(numeric_fluents, false);
			theRequirements[":object-fluents"]				= std::pair<PDDL::REQUIREMENTS, bool>(object_fluents, false);
			theRequirements[":preferences"]					= std::pair<PDDL::REQUIREMENTS, bool>(preferences, false);
			theRequirements[":quantified-preconditions"]	= std::pair<PDDL::REQUIREMENTS, bool>(quantified_preconditions, false);
/* true */	theRequirements[":strips"]						= std::pair<PDDL::REQUIREMENTS, bool>(strips, true);
			theRequirements[":timed-initial-literals"]		= std::pair<PDDL::REQUIREMENTS, bool>(timed_initial_literals, false);
/* true */	theRequirements[":typing"]						= std::pair<PDDL::REQUIREMENTS, bool>(typing, true);
			theRequirements[":universal-preconditions"]		= std::pair<PDDL::REQUIREMENTS, bool>(universal_preconditions, false);
		}


	// ----- Accessors
	protected:
		std::pair<PDDL::REQUIREMENTS, bool>& GettheRequirementInfo(PDDL::RequireKey aRequireKey) {	// aRequireKey is a PDDL 3.1 requirement; does this implementation accept this PDDL 3.1 requirement?
			return (theRequirements[aRequireKey]);
		}

		bool UnknownRequireKey(PDDL::RequireKey aRequireKey) const {	// Is aRequireKey a PDDL 3.1 requirement?
			return (theRequirements.end() == theRequirements.find(aRequireKey));
		}

};


#endif	// _SPHG_PDDL_H
// ================================================================================================ End of file "PDDL.h"