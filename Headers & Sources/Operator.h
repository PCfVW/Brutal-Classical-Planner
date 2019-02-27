// ================================================================================================ Beginning of file "Operator.h"
// Copyright (c) 2012-2019, Eric Jacopin, ejacopin@ymail.com
// ------------------------------------------------------------------------------------------------
#ifndef _SPHG_PLANNER_OPERATOR_H
#define _SPHG_PLANNER_OPERATOR_H

#if defined(_MSC_VER)	// _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#pragma once		// When compiling, this file will be included (opened) only once by the Microsoft C++ compiler 
#endif					// _MSC_VER

// ------------------------------------------------------------------------------------------------ Inclusion of files
// STD and STL files
#if defined(_MSC_VER)			// _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#include <unordered_map>	// Use C++11 STL hashed structures
#else
	#include <map>				// Use a classic associative STL container when the compiler is not Microsoft's
#endif							// _MSC_VER
#include <valarray>		// Store house for instantiated theParameters of an operator and (when (STL == MEMORY_ALLOCATION)) STL store house for thePredicates
#include <vector>		// STL Store house for theParameters of an operator

// Project files
#ifndef _SPHG_PLANNER_CONDITIONAL_COMPILING_H
	#include "ConditionalCompiling.h"	// Visibility for MEMORY_ALLOCATION, STL, HPHA and CLASSIC
#endif

#ifndef _INTERNAL_TYPES_H
	#include "InternalTypes.h"			// Visibility for u8 and f32
#endif

#ifndef _SPHG_PDDL_H
	#include "PDDL.h"					// Visibility for HIdentifier, Parameter, HPredicate and SORT
#endif

// ------------------------------------------------------------------------------------------------ Class definition
class Operator {
	// ----- Types
	public:
		typedef		INTERNAL_TYPES::u8								HArity;					// [0, (2^8) - 1 = 255] An Operator possess at most 256 parameters
		typedef		INTERNAL_TYPES::u8								HPredicate;				// [0, (2^8) - 1 = 255] An Operator possess at most 256 distinct predicates (as precondition, addition and deletions predicates)

		typedef		std::valarray<PDDL::HPredicate>					Predicates;				// (when (STL == MEMORY_ALLOCATION)) The set of (at most 256 distinct) predicates used by this operator

		typedef		std::valarray<PDDL::HIdentifier>				OperatorSignature;		// An instantiated Operator: a pointer to its name (last position of the valarray) and the valarray of (integer) pointers to its instantiated parameters
		typedef		std::vector<OperatorSignature>					OperatorSignatures;		// OperatorSignatures shared by plans with a common sequence of actions

		typedef		std::vector<OperatorSignature>::size_type		HOperatorSignature;		// (integer) Pointers to a shared OperatorSignature
		typedef		std::vector<Operator::HOperatorSignature>		HOperatorSignatures;	// Totally ordered set of (integer) pointers to (shared) OperatorSignatures

		typedef		std::vector<PDDL::Parameter>					Parameters;				// PDDL::Parameters == std::vector<PDDL::Parameter>& == Operator::Parameters&

		typedef		std::pair<HPredicate, HArity>					Position;				// Where the value of a Parameter can be found, in terms of precondition number and (one of this precondition's) parameter number, when this Operator's preconditions are tested (unified) against the predicates of a State
		typedef		std::vector<std::vector<Position>>				Positions;				// Where the values of all the parameters of this Operator can be found, when this Operator's preconditions are tested (unified) against the predicates of a State

	// ----- Properties
	private:
		PDDL::HIdentifier					theIdentifier;				// The name of the operator as declared in a PDDL domain, for instance used to build a (<name>, <parameters>) signature in a totally ordered plan

#if (STL == MEMORY_ALLOCATION)
		Operator::Parameters				theParameters;				// A parameter is a pair whose first tells whether the parameter is a constant or else a variable and whose second points to the string identifier of the parameter
		Operator::Predicates				thePredicates;				// Array (of size HP_d) of HPredicates representing the HP_d predicates used by this Operator
		Operator::Positions					thePositions;
#elif (DL_MALLOC == MEMORY_ALLOCATION) || (HPHA == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION)
		PDDL::Parameters					theParameters;				// A parameter is a pair whose first tells whether the parameter is a constant or else a variable and whose second points to the string identifier of the parameter
		PDDL::HPredicate*					thePredicates;				// Array (of size HP_d) of HPredicates representing the HP_d predicates used by this operator
		Operator::Positions					thePositions;
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif

		Operator::HArity					theNumberOfParameters;
		// From a programming point of view, the semantic of a predicate can be understood as a
		// function returning true or false; for instance: if color is a predicate identifier,
		// then color(red) could return true or false if we're indeed dealing with the red color.
		//
		// The STRIPS (STanford Research Institute Problem Solver) semantic for a predicate is that
		// of membership: a predicate is true in a state, also called "situation", when it belongs
		// to the set of predicates representing this situation and it is false otherwise.
		// The STRIPS semantic is here used.
		//
		// A PDDL operator is made of two sets of predicates of predicates: the preconditions and
		// the effects.
		//
		// The effects are separated into two disjoint sets: negative and positive effects whose
		// intersection thus is empty. For instance, in PDDL Lisp_like notation, (color red) can
		// be a positive effect; negative effects are predicates prefixed by the symbole "not":
		// for instance, (not (color red)).
		//
		// A precondition is called a negative precondition when it is represented by a negative
		// predicate; it is called a positive precondition otherwise, that is when it is represented
		// by a predicate: (color red) can be a positive precondition while (not (color red)) can be
		// a negative precondition.
		//
		// When all the preconditions of an operator belong to a situation, this operator can be
		// "executed" in this situation. When a precondition of an operator is represented by a
		// negative predicate, the semantic is that the predicate does not belong to the situation
		// where the operator is executed. For instance, when (not (color red)) is a precondition of
		// an operator, (color red) must not belong to the situation where this operator is executed.
		//
		// When an operator is executed (because its preconditions are true) in a situation, its
		// negative effects are removed (set difference) from the set representing the situation
		// while its positive effects are added (set-union) to the set representing the situation.
		//
		// Following a good old STRIPS naming convention, negative effects are here called deletions
		// and positive effects are here called additions; and the deletions of an operator are its 
		// delete list while its additions are its add list.
		//
		// Here are our naming conventions:
		//  - negative precondition is a predicate prefixed by the negation (i.e. "not") whereas a
		//		positive precondition is just a predicate (no prefix).
		//	- (pure) Deletions are predicates which only appear in the deletions,
		//	- Deleted preconditions are precondition predicates which also appear in the deletions:
		//		that is, if (p) is a precondition then (not (p)) is a -- negative -- effect,
		//	- (pure) Preconditions are predicates which only appear as preconditions,
		//	- Added preconditions are negative precondition predicates which also appear in the additions:
		//		that is, if (not (p)) is a -- negative -- precondition then (p) is a -- positive -- effect,
		//	- (pure) Additions are predicates which only appear in the additions.
		//
		// Note that if (not (p)) is a precondition then the (negative) effect (not (p)) is rather useless;
		// in the same spirit, if (p) is a precondition then the (positive) effect (p) is rather useless
		// as well. Both cases are correct but entail computational expenses and thus should be avoided.
		//
		// Remind that the deletions and the additions have no common predicate: they are two disjoint sets;
		// i.e. their intersection is empty.
		//
		// Here, the predicates are not stored in a STRIPS operator but in the PDDL domain predicate store.
		// (see attribute thePredicateStore of the Domain class in file "Domain.h").
		//
		// In order to remember which predicates are used by an operator, we use an array of integers where
		// each integer corresponds to the position of a predicate in the predicate store of the PDDL domain.
		//
		// Although a predicate (a deleted precondition or else an added negative precondition) can (at most)
		// belong to 2 sets of predicates of an operator, in this implementation, the array of the predicates 
		// of an operator is arranged so that any given predicate occurs only once, as follows:
		//
		//		[0, ..., HP_a, ..., HP_b, ..., HP_c, ..., HP_d, ..., HP_e, ..., HP_f]
		//
		// The following ranges describe the content of the array:
		//	- [0, HP_a - 1]    is the range of (positive) preconditions
		//	- [HP_a, HP_b - 1] is the range of deleted (positive) preconditions
		//	- [0, HP_b - 1]    is the range of (positive) preconditions 
		//	- [HP_b, HP_c - 1] is the range of deletions
		//	- [HP_c, HP_d - 1] is the range of additions
		//	- [HP_d, HP_e - 1] is the range of added (negative) preconditions
		//	- [HP_e, HP_f]     is the range of negative preconditions
		//
		// Consequently:
		//	- the preconditions are in the range [0, HP_b - 1] and in the range [HP_d, HP_f]
		//	- the deletions are in the range [HP_a, HP_c - 1]
		//	- the additions are in the range [HP_c, HP_e - 1]
		//
		Operator::HPredicate				thePositionOfDeletedPreconditions;	// == HP_a
		Operator::HPredicate				thePositionOfDeletions;				// == HP_b
		Operator::HPredicate				thePositionOfAdditions;				// == HP_c
		Operator::HPredicate				thePositionOfAddedPreconditions;	// == HP_d
		Operator::HPredicate				thePositionOfNegativePreconditions;	// == HP_e
		Operator::HPredicate				theNumberOfPredicates;				// == (HP_f + 1)


	// ----- Constructors
	public:
		Operator() {}
		Operator(PDDL::HIdentifier anIdentifier,
					Operator::HArity NumberOfParameters,
					Operator::HPredicate NumberOfDeletedPreconditions,			// == (HP_b - HP_a)
					Operator::HPredicate NumberOfPositivePreconditions,			// == HP_b
					Operator::HPredicate NumberOfDeletions,						// == (HP_c - HP_b)
					Operator::HPredicate NumberOfAdditions,						// == (HP_e - HP_c)
					Operator::HPredicate NumberOfNegativePreconditions,			// == (HP_f + 1 - HP_e)
					Operator::HPredicate NumberOfAddedPreconditions) :			// == (HP_e - HP_d)
			theIdentifier(anIdentifier),

			theNumberOfParameters(NumberOfParameters),
			
			thePositionOfDeletedPreconditions(NumberOfPositivePreconditions - NumberOfDeletedPreconditions),
			thePositionOfDeletions(NumberOfPositivePreconditions),
			thePositionOfAdditions(NumberOfPositivePreconditions + NumberOfDeletions),
			thePositionOfAddedPreconditions(NumberOfPositivePreconditions + NumberOfDeletions + NumberOfAdditions - NumberOfAddedPreconditions),
			thePositionOfNegativePreconditions(NumberOfPositivePreconditions + NumberOfDeletions + NumberOfAdditions),
			theNumberOfPredicates(NumberOfPositivePreconditions + NumberOfDeletions + NumberOfAdditions - NumberOfAddedPreconditions + NumberOfNegativePreconditions) {

			// Allocate the parameters and the exact number of predicates (preconditions, additions and deletions) used by this operator
#if (STL == MEMORY_ALLOCATION)
			theParameters = Operator::Parameters(theNumberOfParameters);
			thePredicates = Operator::Predicates(theNumberOfPredicates);
#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
			theParameters = (PDDL::Parameters) SPHG_ALLOC(theNumberOfParameters * sizeof(PDDL::Parameter));
			thePredicates = (PDDL::HPredicate*) SPHG_ALLOC(theNumberOfPredicates * sizeof(PDDL::HPredicate));
#elif (HPHA == MEMORY_ALLOCATION)
			theParameters = (PDDL::Parameters) SPHG_ALLOC(theNumberOfParameters * sizeof(PDDL::Parameter), sizeof(PDDL::Parameter));
			thePredicates = (PDDL::HPredicate*) SPHG_ALLOC(theNumberOfPredicates * sizeof(PDDL::HPredicate), sizeof(PDDL::HPredicate));
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif

			thePositions = Positions(theNumberOfParameters);
		}

		Operator (const Operator& anOperator) :
				theIdentifier(anOperator.theIdentifier),
				theNumberOfParameters(anOperator.theNumberOfParameters),
				thePositionOfDeletedPreconditions(anOperator.thePositionOfDeletedPreconditions),
				thePositionOfDeletions(anOperator.thePositionOfDeletions),
				thePositionOfAdditions(anOperator.thePositionOfAdditions),
				thePositionOfAddedPreconditions(anOperator.thePositionOfAddedPreconditions),
				thePositionOfNegativePreconditions(anOperator.thePositionOfNegativePreconditions),
				theNumberOfPredicates(anOperator.theNumberOfPredicates) {
			
			// Allocate the parameters and the exact number of preconditions, additions and deletions (that is, deleted preconditions only are allocated once as preconditions)
#if (STL == MEMORY_ALLOCATION)
			theParameters = anOperator.theParameters;
			thePredicates = anOperator.thePredicates;

#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
			theParameters = (PDDL::Parameters) SPHG_ALLOC(theNumberOfParameters * sizeof(PDDL::Parameter));
			PDDL::Parameters TempParameters = anOperator.theParameters;
			for (Operator::HArity i = 0; i < theNumberOfParameters; ++i)
				theParameters[i] = TempParameters[i];

			thePredicates = (PDDL::HPredicate*) SPHG_ALLOC(theNumberOfPredicates * sizeof(PDDL::HPredicate));
			PDDL::HPredicate* TempPredicates = anOperator.thePredicates;
			for (Operator::HArity i = 0; i < theNumberOfPredicates; ++i)
				thePredicates[i] = TempPredicates[i];

#elif (HPHA == MEMORY_ALLOCATION)
			theParameters = (PDDL::Parameters) SPHG_ALLOC(theNumberOfParameters * sizeof(PDDL::Parameter), sizeof(PDDL::Parameter));
			PDDL::Parameters TempParameters = anOperator.theParameters;
			for (Operator::HArity i = 0; i < theNumberOfParameters; ++i)
				theParameters[i] = TempParameters[i];

			thePredicates = (PDDL::HPredicate*) SPHG_ALLOC(theNumberOfPredicates * sizeof(PDDL::HPredicate), sizeof(PDDL::HPredicate));
			PDDL::HPredicate* TempPredicates = anOperator.thePredicates;
			for (Operator::HArity i = 0; i < theNumberOfPredicates; ++i)
				thePredicates[i] = TempPredicates[i];

#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif
			thePositions = anOperator.thePositions;

		}


	// ----- Destructor
	public:
		~Operator() {
#if (STL == MEMORY_ALLOCATION)
			// Nothing to do here: the STL takes care of the memory management for valarrays
#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
			SPHG_FREE(theParameters);
			SPHG_FREE(thePredicates);
#elif (HPHA == MEMORY_ALLOCATION)
			SPHG_FREE((PDDL::Parameters) theParameters, sizeof(PDDL::Parameter) * theNumberOfParameters, sizeof(PDDL::Parameter));
			SPHG_FREE((PDDL::HPredicate*) thePredicates, sizeof(PDDL::HPredicate) * theNumberOfPredicates, sizeof(PDDL::HPredicate));
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif
		}


	// ----- Accessors
	public:
		inline PDDL::HIdentifier GettheIdentifier() const { return theIdentifier; }
#if (STL == MEMORY_ALLOCATION)
		inline Operator::Parameters GettheParameters() const { return theParameters; }
#elif (DL_MALLOC == MEMORY_ALLOCATION) || (HPHA == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION)
		inline PDDL::Parameters GettheParameters() const { return theParameters; }
		inline void CopytheParametersInto(PDDL::Parameters P) const {
			for (Operator::HArity i = 0; i < theNumberOfParameters; ++i)
				P[i] = theParameters[i];
		}
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif

		inline Operator::HPredicate GetthePositionOfDeletedPreconditions() const { return thePositionOfDeletedPreconditions; }
		inline Operator::HPredicate GetthePositionOfDeletions() const { return thePositionOfDeletions; }
		inline Operator::HPredicate GetthePositionOfAdditions() const { return thePositionOfAdditions; }
		inline Operator::HPredicate GetthePositionOfNegativePreconditions() const { return thePositionOfNegativePreconditions; }
		inline Operator::HPredicate GetthePositionOfAddedPreconditions() const { return thePositionOfAddedPreconditions; }
		inline Operator::HPredicate GettheNumberOfPredicates() const { return theNumberOfPredicates; }
		
		inline Operator::HArity GettheNumberOfParameters() const { return theNumberOfParameters; }
		inline PDDL::SORT GettheSORTOfParameter(Operator::HArity a) const { return theParameters[a].first; }
		inline PDDL::HIdentifier GettheIdentifierOfParameter(Operator::HArity a) const { return theParameters[a].second; }
		inline void SettheParameter(Operator::HArity a, const PDDL::Parameter& aParameter) { theParameters[a] = aParameter; }
		inline void AddParameter(Operator::HArity a, const PDDL::Parameter& aParameter) {
			theParameters[a] = aParameter;
		}
		inline Operator::HPredicate GettheNumberOfPositivePreconditions() const { return thePositionOfDeletions; }
		inline PDDL::HPredicate GetthePredicate(Operator::HPredicate a) const { return thePredicates[a]; }
		inline void AddPredicate(Operator::HPredicate a, PDDL::HPredicate aPredicate) {
			thePredicates[a] = aPredicate;
		}
		inline void AddPrecondition(Operator::HPredicate a, PDDL::HPredicate aPredicate) {
			thePredicates[a] = aPredicate;
		}
		inline void AddAddition(Operator::HPredicate a, PDDL::HPredicate aPredicate) {
			thePredicates[a] = aPredicate;
		}
		inline void AddDeletion(Operator::HPredicate a, PDDL::HPredicate aPredicate) {
			thePredicates[a] = aPredicate;
		}
		inline void AddPositions(Operator::Positions& somePositions) {
			thePositions = somePositions;
		}
		inline Operator::Position GetthePosition(std::vector<std::vector<Position>>::size_type p1, std::vector<Position>::size_type p2) const {
			return thePositions[p1][p2];
		}

		inline std::vector<std::vector<Position>>::size_type GetthePositionsSize() const { return thePositions.size(); }

		inline std::vector<std::vector<Position>>::size_type GetthePositionsSize(std::vector<std::vector<Position>>::size_type p) const {
			return thePositions[p].size();
		}


	// ----- Operations
	public:
		inline void Resize(Operator::HArity a) {
			// Constants which appear as parameters of some predicates of an operator are declared as instantiated parameters of this parameter:
			// thus the need to increase the initial number of parameters, for an easy reading of the domain file
			theNumberOfParameters += a;
#if (STL == MEMORY_ALLOCATION)
			theParameters.resize(theNumberOfParameters);
#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
			theParameters = (PDDL::Parameters) SPHG_REALLOC(theParameters, sizeof(PDDL::Parameter) * theNumberOfParameters);
#elif (HPHA == MEMORY_ALLOCATION)
			theParameters = (PDDL::Parameters) SPHG_REALLOC(theParameters, sizeof(PDDL::Parameter) * theNumberOfParameters, sizeof(PDDL::Parameter));
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif
		}

		inline OperatorSignature MaketheOperatorSignature(PDDL::Parameters P) const {
			OperatorSignature InstantiatedParameters(1 + theNumberOfParameters);
			for (Operator::HArity a = 0; a < theNumberOfParameters; ++a)
				InstantiatedParameters[a] = P[a].second;
			InstantiatedParameters[theNumberOfParameters] = theIdentifier;

			return InstantiatedParameters;
		}

		size_t SizeOf() const;	// Computing the memory size of this Operator

};

#endif	// _SPHG_PLANNER_OPERATOR_H
// ================================================================================================ End of file "Operator.h"