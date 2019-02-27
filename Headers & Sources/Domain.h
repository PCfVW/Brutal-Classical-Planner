// ================================================================================================ Beginning of file "Domain.h"
// Copyright (c) 2012-2019, Eric Jacopin, ejacopin@ymail.com
// ------------------------------------------------------------------------------------------------
#ifndef _SPHG_PDDL_DOMAIN_H
#define _SPHG_PDDL_DOMAIN_H

#if defined(_MSC_VER)	// _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#pragma once		// When compiling, this file will be included (opened) only once by the Microsoft C++ compiler 
#endif					// _MSC_VER

// ------------------------------------------------------------------------------------------------ Inclusion of files
// STD and STL files
#if defined(_MSC_VER)			// _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#include <unordered_map>	// Use Microsoft's STL extensions which provides hashed structures
#endif							// _MSC_VER
#include <map>			// maps are needed whatever the compiler, but also instead of Microsoft's hash_maps when the compiler is not MSC++
#include <vector>		// STL Store house for the predicates, the operators, the problems, etc of this domain

// Project files
#ifndef _INTERNAL_TYPES_H
	#include "InternalTypes.h"			// Visibility for u8 and f32
#endif

#ifndef _SPHG_PLANNER_OPERATOR_H
	#include "Operator.h"
#endif

#ifndef _SPHG_PDDL_H
	#include "PDDL.h"
#endif

#ifndef _SPHG_PLANNER_PREDICATE_H
	#include "Predicate.h"
#endif

#ifndef _SPHG_PDDL_PROBLEM_H
	#include "Problem.h"
#endif

// ------------------------------------------------------------------------------------------------ Class definition
class Domain : public PDDL {
	// ----- Types
	public:
		typedef		INTERNAL_TYPES::u8						HProblem;						// [0, (2^8) - 1 = 255] A domain possess at most 256 problems

		typedef		std::vector<Operator>::size_type		HOperator;						// Integer pointer to an Operator


	// ----- Properties
	protected:
		PDDL::Identifier								theName;									// Stored explicitely in this attribute instead of in theIdentifiers
		std::vector<PDDL::Identifier>					theIdentifiers;								// theIdentifiers::size_type == HIdentifier
		std::map<PDDL::Identifier, PDDL::HIdentifier>	theConstants;								// Identifiers to their positions in theIdentifiers for a quick access
		std::vector<Predicate>							thePredicates;								// Predicates declared in the :actions of this Domain (e.g. read from a PDDL Domain file)
		std::vector<Operator>							theOperators;								// the set of :actions declared for this Domain (e.g. read from a PDDL Domain file)
		std::vector<PDDL::Number>						theCosts;									// the costs of theOperators as declared when :action-costs is required
		std::vector<Problem*>							theProblems;								// Problems declared for this Domain (e.g. read from PDDL problem files)
		Predicate::PredicatesStore						thePredicatesStore;							// Built from the :predicates section of a PDDL Domain file; Predicate identifiers to their arities and identifiers for quick check of predicates when getting PDDL data (domain/problem) after the :predicates section of the PDDL Domain file
		Predicate::PredicatesStore						theFunctionsStore;							// Built from the :functions section of a PDDL Domain file; Functions identifiers to their arities and identifiers for quick check of predicates when getting PDDL data (domain/problem) after the :functions section of the PDDL Domain file
		bool											theRequirements[PDDL::NUMBER_OF_REQUIREMENTS];	// A position is true when this Domain requires a PDDL::REQUIREMENTS


	// ----- Constructors
	public:
		Domain () {
			for (PDDL::HRequirement i = 0; i < PDDL::NUMBER_OF_REQUIREMENTS; ++i)				// Automatically called when building a PDDL Domain from a PDDL Domain file
				theRequirements[i] = false;
		}
		Domain(const PDDL::Identifier& aName) : theName(aName) {								// Call this constructor explicitely (see file "main.cpp" for examples) when building a PDDL Domain from C++ Code (NEVER CALLED when building a domain from a PDDL Domain file)
			for (PDDL::HRequirement i = 0; i < PDDL::NUMBER_OF_REQUIREMENTS; ++i)
				theRequirements[i] = false;
		}


	// ----- Accessors
	public:
		inline PDDL::Identifier GettheName() const {
			return theName;
		}
		inline Predicate::PredicatesStore::size_type GetthePredicatesStoreSize() {
			return thePredicatesStore.size();
		}
		inline bool GetRequirement(PDDL::HRequirement r) const {
			return theRequirements[r];
		}
		inline PDDL::HIdentifier AddIdentifier(const PDDL::Identifier& anIdentifier) {
			PDDL::HIdentifier p = (PDDL::HIdentifier) theIdentifiers.size();
			theIdentifiers.push_back(anIdentifier);
			theConstants[anIdentifier] = p;
			return p;
		}
		inline PDDL::Identifier& GettheIdentifier(PDDL::HIdentifier anIdentifier) {
			return theIdentifiers[anIdentifier];
		}
		inline std::map<PDDL::Identifier, PDDL::HIdentifier>& GettheConstants() {
			return theConstants;
		}
		inline std::vector<Predicate>& GetthePredicates() {
			return thePredicates;
		}
		inline PDDL::HPredicate AddPredicate(const Predicate aPredicate) {
			// This version of AddPredicate/1 is only called when building PDDL predicates from C++ (and NEVER called when from PDDL Domain files)

			// So, first, fill thePredicateStore as if we're reading the :predicates section of the PDDL Domain
			PDDL::HIdentifier i = aPredicate.GettheIdentifier();
			thePredicatesStore[theIdentifiers[i]] = Predicate::PredicateSignature(i, aPredicate.GettheArity());
			// Then, second, do the usual recording of aPredicate in thePredicates store
			PDDL::HPredicate p = (PDDL::HPredicate) thePredicates.size();
			thePredicates.push_back(aPredicate);
			return p;
		}
		inline PDDL::HPredicate AddPredicate(const PDDL::Identifier& anIdentifier, Predicate::HArity anArity) {
			PDDL::HPredicate p = (PDDL::HPredicate) thePredicates.size();
			Domain::thePredicates.push_back(Predicate(AddIdentifier(anIdentifier), anArity));
			return p;
		}
		inline PDDL::HPredicate AddPredicate(PDDL::HIdentifier anIdentifier, Predicate::HArity anArity) {
			PDDL::HPredicate p = (PDDL::HPredicate) thePredicates.size();
			Domain::thePredicates.push_back(Predicate(anIdentifier, anArity));
			return p;
		}
		inline void AddPredicateParameter(PDDL::HPredicate aPredicate, Predicate::HArity a, Operator::HArity aParameter) {
			thePredicates[aPredicate].AddParameter(a, aParameter);
		}
		inline std::vector<Operator>::size_type GettheNumberOfOperators() const {
			return theOperators.size();
		}
		inline const Operator* GettheOperator(Domain::HOperator Op) const {
			return &theOperators[Op];
		}
		inline Domain::HOperator AddOperator(const Operator& anOperator) {
			Domain::HOperator o = theOperators.size();
			theOperators.push_back(anOperator);
			return o;
		}
		inline void AddOperatorParameter(Domain::HOperator anOperator, Operator::HArity a, const PDDL::Parameter& aParameter) {
			theOperators[anOperator].AddParameter(a, aParameter);
		}
		inline void AddOperatorPositions(Domain::HOperator anOperator, Operator::Positions& somePositions) {
			theOperators[anOperator].AddPositions(somePositions);
		}
		inline void AddPrecondition(Domain::HOperator anOperator, Operator::HPredicate a, PDDL::HPredicate aPredicate) {
			theOperators[anOperator].AddPrecondition(a, aPredicate);
		}
		inline void AddAddition(Domain::HOperator anOperator, Operator::HPredicate a, PDDL::HPredicate aPredicate) {
			theOperators[anOperator].AddAddition(a, aPredicate);
		}
		inline void AddDeletion(Domain::HOperator anOperator, Operator::HPredicate a, PDDL::HPredicate aPredicate) {
			theOperators[anOperator].AddDeletion(a, aPredicate);
		}
		inline void AddOperatorPredicate(Domain::HOperator anOperator, Operator::HPredicate a, PDDL::HPredicate aPredicate) {
			theOperators[anOperator].AddPredicate(a, aPredicate);
		}
		inline PDDL::Number GetNumericCost(Domain::HOperator anOperator) {
			return theCosts[anOperator];
		}
		inline void AddNumericCost(Domain::HOperator anOperator, PDDL::Number aCost) {
			if (anOperator == theCosts.size())
				theCosts.push_back(aCost);
			else // (anOperator != theCosts.size()) but we mean (0 <= anOperator < theCosts.size())
				theCosts[anOperator] = aCost;
		}
		inline Problem* GettheProblem(Domain::HProblem aProblem) {
			return theProblems[aProblem];
		}
		inline Domain::HProblem AddProblem(Problem& aProblem) {
			Domain::HProblem p = (Domain::HProblem) theProblems.size();
			theProblems.push_back(&aProblem);
			return p;
		}
		// Remember a requirement for this Domain
		inline void RegisterRequireKey(PDDL::RequireKey aRequireKey) {
			std::pair<PDDL::REQUIREMENTS, bool> ri = PDDL::GettheRequirementInfo(aRequireKey);
			Domain::theRequirements[ri.first] = ri.second;
		}
		inline std::pair<Predicate::PredicatesStore::iterator, bool> Find(PDDL::Identifier& token) {
			Predicate::PredicatesStore::iterator it = thePredicatesStore.find(token);
			return std::pair<Predicate::PredicatesStore::iterator, bool>(it, thePredicatesStore.end() != it);
		}
};

#endif	// _SPHG_PDDL_DOMAIN_H
// ================================================================================================ End of file "Domain.h"