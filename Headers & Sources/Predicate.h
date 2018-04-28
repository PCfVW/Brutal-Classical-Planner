// ================================================================================================ Beginning of file "Predicate.h"
// Copyright (c) 2012-2018, Eric Jacopin, ejacopin@ymail.com
// ------------------------------------------------------------------------------------------------
#ifndef _SPHG_PLANNER_PREDICATE_H
#define _SPHG_PLANNER_PREDICATE_H

#if defined(_MSC_VER)	// _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#pragma once		// When compiling, this file will be included (opened) only once by the Microsoft C++ compiler 
#endif					// _MSC_VER

// ------------------------------------------------------------------------------------------------ Inclusion of files
// STD and STL files
#if (STL == MEMORY_ALLOCATION)
	#include <valarray>		// STL Store House for the parameters of a predicate
#endif

// Project files
#ifndef _INTERNAL_TYPES_H
	#include "InternalTypes.h"
#endif

#ifndef _SPHG_PDDL_H
	#include "PDDL.h"
#endif

#ifndef _SPHG_PLANNER_OPERATOR_H
	#include "Operator.h"
#endif

// ------------------------------------------------------------------------------------------------ Class definition
class Predicate {
	// ----- Types
	public:
		typedef		INTERNAL_TYPES::u8										HArity;				// [0, (2^8) - 1 = 255] A Predicate possess at most 256 parameters
		typedef		std::pair<PDDL::HIdentifier, Predicate::HArity>			PredicateSignature;	// An integer pointer to the identifier of the predicate and an unsigned integer value of its number of parameters (its so-called arity)
#if defined(_MSC_VER)	// Use Microsoft's STL extensions which provides hashed structures
		typedef		std::unordered_map<PDDL::Identifier, PredicateSignature>	PredicatesStore;	// Built from the :predicates section of a PDDL Domain file; Predicate identifiers to their arities and identifiers for quick check of predicates when getting PDDL data (domain/problem) after the :predicates section of the PDDL Domain file
#else
		typedef		std::map<PDDL::Identifier, PredicateSignature>			PredicatesStore;	// Built from the :predicates section of a PDDL Domain file; Predicate identifiers to their arities and identifiers for quick check of predicates when getting PDDL data (domain/problem) after the :predicates section of the PDDL Domain file
#endif					// _MSC_VER


	// ----- Properties
	private:
#if (STL == MEMORY_ALLOCATION)
		std::valarray<PDDL::HIdentifier>	theParameters;		// The array of PDDL::HIdentifier representing the parameters (integer pointers) of this predicate
#elif (DL_MALLOC == MEMORY_ALLOCATION) || (HPHA == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION)
		Operator::HArity*					theParameters;		// The array of Operator::HArity representing the parameters (integer pointers) of this predicate
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif

		PDDL::HIdentifier		theIdentifier;			// The name of this predicate		
		Predicate::HArity		theArity;				// The number of parameters of this predicate (i.e. the size of theParameters)


	// ----- Constructors
	public:
		Predicate() {}
		Predicate(PDDL::HIdentifier anIdentifier, Predicate::HArity anArity) : theIdentifier(anIdentifier), theArity(anArity) {
			// Allocate the exact number of HParameters
#if (STL == MEMORY_ALLOCATION)
			theParameters = std::valarray<PDDL::HIdentifier>(anArity);
#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
			theParameters = (Operator::HArity*) SPHG_ALLOC(sizeof(Operator::HArity) * theArity);
#elif (HPHA == MEMORY_ALLOCATION)
			theParameters = (Operator::HArity*) SPHG_ALLOC(sizeof(Operator::HArity) * theArity, sizeof(Operator::HArity));
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif
		}
		Predicate(const Predicate& aPredicate) : theIdentifier(aPredicate.theIdentifier), theArity(aPredicate.theArity) {
#if (STL == MEMORY_ALLOCATION)
			theParameters = std::valarray<PDDL::HIdentifier>(theArity);
#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
			theParameters = (Operator::HArity*) SPHG_ALLOC(sizeof(Operator::HArity) * theArity);
#elif (HPHA == MEMORY_ALLOCATION)
			theParameters = (Operator::HArity*) SPHG_ALLOC(sizeof(Operator::HArity) * theArity, sizeof(Operator::HArity));
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif
			for (Predicate::HArity a = 0; a < theArity; ++a)
				theParameters[a] = aPredicate.theParameters[a];
		}


	// ----- Destructor
		~Predicate() {
#if (STL == MEMORY_ALLOCATION)
			// Nothing to do here: the STL takes care of the memory management for valarrays
#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
			SPHG_FREE(theParameters);
#elif (HPHA == MEMORY_ALLOCATION)
			SPHG_FREE((Operator::HArity*) theParameters, sizeof(Operator::HArity) * theArity, sizeof(Operator::HArity));
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif
		}


	// ----- Accessors
	public:
		inline PDDL::HIdentifier GettheIdentifier() const { return theIdentifier; }
		inline Predicate::HArity GettheArity() const { return theArity; }
		inline PDDL::HIdentifier GettheParameter(Predicate::HArity aParameter) const { return theParameters[aParameter]; }
		inline void AddParameter(Predicate::HArity a, Operator::HArity aParameter) {
			theParameters[a] = aParameter;
		}


	// ----- Operations
	public:
		inline bool operator < (const Predicate& apredicate) const {	// Comparing predicates in order to insert them into an std::map [domainfile.cpp(495), problemfile.cpp(297), statespaceforwardchaining.h(95)]
			if (theIdentifier == apredicate.GettheIdentifier())
				if (theArity == apredicate.GettheArity())
				{
					// check the parameters when the identifiers and arities of both predicates are equal
					for (Predicate::HArity a = 0; a < theArity; ++a)
					{
						if (theParameters[a] < apredicate.GettheParameter(a))
							return true;
						else if (theParameters[a] > apredicate.GettheParameter(a))
							return false;

						// when (theparameters[a] == apredicate.theparameters[a]), increase a by 1 and compare the next parameters
					}

					// identifiers, arities and all hpredicate parameters of both predicates are equal:
					return false;
				}
				else
					// identifiers of both predicates are equal, return which arity is less than the other
					return (theArity < apredicate.GettheArity());
			else
				// return which identifier is less than the other
				return (theIdentifier < apredicate.GettheIdentifier());
		}

		bool Unify(Predicate& aPredicate, PDDL::Parameters P);	// Unify this Predicate with aPredicate under the constraints from P (the parameters of an Operator where this Predicate appears)
		bool Match(Predicate& aPredicate, PDDL::Parameters P);	// Pattern matching of this Predicate with aPredicate under the constraints from P (the parameters of Operator where this Predicate appears)

		Predicate& UpdatetheParameters(PDDL::Parameters OperatorParameters);	// theParameters of this Predicate get their values from the parameters of an Operator
		
		size_t SizeOf() const;	// Computing the memory size for this Predicate
};

#endif	// _SPHG_PLANNER_PREDICATE_H
// ================================================================================================ End of file "Predicate.h"