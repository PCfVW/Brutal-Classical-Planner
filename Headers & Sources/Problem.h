// ================================================================================================ Beginning of file "Problem.h"
// Copyright (c) 2012-2019, Eric Jacopin, ejacopin@ymail.com
// ------------------------------------------------------------------------------------------------
#ifndef _SPHG_PDDL_PROBLEM_H
#define _SPHG_PDDL_PROBLEM_H

#if defined(_MSC_VER)	// _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#pragma once		// When compiling, this file will be included (opened) only once by the Microsoft C++ compiler 
#endif					// _MSC_VER

// ------------------------------------------------------------------------------------------------ Inclusion of files
// STD and STL files
#include <map>			// STL Store house for the constants of the domain + this Problem
#include <vector>		// STL Store house for the predicates and the searches for this Problem

// Project files
#ifndef _SPHG_PDDL_H
	#include "PDDL.h"
#endif

#ifndef _SPHG_PLANNER_PREDICATE_H
	#include "Predicate.h"
#endif

#ifndef _SPHG_SEARCH_FACADE_H
	#include "Search.h"
#endif

#ifndef _SPHG_PLANNER_STATES_H
	#include "States.h"
#endif

// ------------------------------------------------------------------------------------------------ Forward declaration
class Domain;

// ------------------------------------------------------------------------------------------------ Class definition
class Problem : public PDDL {
	// ----- Properties
	protected:
		PDDL::Identifier						theName;						// Is not stored in theIdentifiers
		Domain*									theDomain;						// The Domain for this problem

		TemporaryState							thePositiveInitialState;		// Positive predicates of theInitialState declared for the problem (e.g. from a PDDL problem file)
		TemporaryState							theNegativeInitialState;		// Negative predicates of theInitialState declared for the problem (e.g. from a PDDL problem file)
		TemporaryState							thePositiveGoalState;			// Positive predicates of theGoalState declared for the problem (e.g. from a PDDL problem file)
		TemporaryState							theNegativeGoalState;			// Negative predicates of theGoalState declared for the problem (e.g. from a PDDL problem file)

		std::map<HIdentifier, PDDL::Identifier>	theConstants;					// The Constants from the domain of this problem PLUS those declared for this problem (e.g. from the :objects field of a PDDL problem)
		PDDL::HIdentifier						theCPosition;					// The key of the last identifier added to theConstants
		PDDL::HPredicate						thePPosition;					// The Number of all predicates declared in theDomain (e.g. in a PDDL domain file)
		std::vector<Predicate>					thePredicates;					// Predicates members of the initial or the goal states declared for the problem

		bool									theRequirements[PDDL::NUMBER_OF_REQUIREMENTS];	// A position is true when this Problem requires a REQUIREMENTS

		std::vector<Search*>					theSearches;	// Various kind of searches, registered for this problem


	// ----- Constructor
	public:
		Problem() {																// Automatically called when building a PDDL problem from a PDDL problem file
			for (PDDL::HRequirement i = 0; i < PDDL::NUMBER_OF_REQUIREMENTS; ++i)
				theRequirements[i] = false;
		}
		Problem(const PDDL::Identifier& aName, Domain& aDomain);						// Call it explicitely (see file "main.cpp" for examples) when building a PDDL problem from C++ Code (NEVER CALLED when building a problem from a PDDL Problem file)


	// ----- Accessors
	public:
		inline TemporaryState& GetthePositiveInitialState() { return thePositiveInitialState; }
		inline TemporaryState& GettheNegativeInitialState() { return theNegativeInitialState; }
		inline TemporaryState& GetthePositiveGoalState() { return thePositiveGoalState; }
		inline TemporaryState& GettheNegativeGoalState() { return theNegativeGoalState; }

		inline std::map<HIdentifier, PDDL::Identifier>& GettheConstants() { return theConstants; }

		HIdentifier AddConstant(const PDDL::Identifier& anIdentifier);

		inline PDDL::HPredicate AddInitialPositivePredicate(const Predicate& aPredicate) {
			PDDL::HPredicate p = (PDDL::HPredicate) thePredicates.size();	// As PDDL::HPredicate is optimized (i.e. 8-bit or 16-bit unsigned int), this might crash
			thePredicates.push_back(aPredicate);
			// theInitialState refers to thePredicates of theDomain joined to thePredicates of this Problem
			thePositiveInitialState.insert(p + thePPosition);
			return p;
		}

		inline PDDL::HPredicate AddInitialNegativePredicate(const Predicate& aPredicate) {
			PDDL::HPredicate p = (PDDL::HPredicate) thePredicates.size();	// As PDDL::HPredicate is optimized (i.e. 8-bit or 16-bit unsigned int), this might crash
			thePredicates.push_back(aPredicate);
			// theInitialState refers to thePredicates of theDomain joined to thePredicates of this Problem
			theNegativeInitialState.insert(p + thePPosition);
			return p;
		}

		inline PDDL::HPredicate AddGoalPositivePredicate(const Predicate& aPredicate) {
			PDDL::HPredicate p = (PDDL::HPredicate) thePredicates.size();	// As PDDL::HPredicate is optimized (i.e. 8-bit or 16-bit unsigned int), this might crash
			thePredicates.push_back(aPredicate);
			// theGoalState refers to thePredicates of theDomain joined to thePredicates of this Problem
			thePositiveGoalState.insert(p + thePPosition);
			return p;
		}

		inline PDDL::HPredicate AddGoalNegativePredicate(const Predicate& aPredicate) {
			PDDL::HPredicate p = (PDDL::HPredicate) thePredicates.size();	// As PDDL::HPredicate is optimized (i.e. 8-bit or 16-bit unsigned int), this might crash
			thePredicates.push_back(aPredicate);
			// theGoalState refers to thePredicates of theDomain joined to thePredicates of this Problem
			theNegativeGoalState.insert(p + thePPosition);
			return p;
		}

		inline PDDL::HPredicate AddGoalPositivePredicate(const PDDL::HPredicate p) {
			thePositiveGoalState.insert(p + thePPosition);
			return p;
		}

		inline PDDL::HPredicate AddGoalNegativePredicate(const PDDL::HPredicate p) {
			theNegativeGoalState.insert(p + thePPosition);
			return p;
		}

		inline void AddPredicateParameter(PDDL::HPredicate aPredicate, Predicate::HArity a, Operator::HArity aParameter) {
			thePredicates[aPredicate].AddParameter(a, aParameter);
		}

		inline std::vector<Predicate>& GetthePredicates() { return thePredicates; }

		inline void Register(Search* aSearch) {
			theSearches.push_back(aSearch);
		}


	// ----- Operations
	public:

		// Remember a requirement for this Problem
		inline void RegisterRequireKey(PDDL::RequireKey aRequireKey) {
			std::pair<PDDL::REQUIREMENTS, bool> ri = PDDL::GettheRequirementInfo(aRequireKey);
			Problem::theRequirements[ri.first] = ri.second;
		}

		size_t SizeOf(const PDDL::Identifier& aProblemFilename = "");

};

#endif	// _SPHG_PDDL_PROBLEM_H
// ================================================================================================ End of file "Problem.h"