// ================================================================================================ Beginning of file "StateSpaceForwardChaining.h"
// Copyright (c) 2012-2019, Eric Jacopin, ejacopin@ymail.com
// ------------------------------------------------------------------------------------------------
#ifndef _SPHG_PLANNER_SEARCH_SSFC_H
#define _SPHG_PLANNER_SEARCH_SSFC_H

#if defined(_MSC_VER)	// _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#pragma once		// When compiling, this file will be included (opened) only once by the Microsoft C++ compiler 
#endif					// _MSC_VER

// ------------------------------------------------------------------------------------------------ Inclusion of files
// STD and STL files
#include <deque>		// STL Store house for the plans waiting to be expanded
#include <functional>	// Less binary_function between two Plans with respect to their costs
#include <iostream>		// Writing a plan out on a stream
#include <queue>		// STL Store house for the plans with their costs, waiting to be expanded
#include <vector>		// STL Store house for various structures (Identifiers, Predicates, etc)

// Project files
#ifndef _EXTERNAL_TYPES_H
	#include "ExternalTypes.h"
#endif

#ifndef _INTERNAL_TYPES_H
	#include "InternalTypes.h"			// Visibility for u32
#endif

#ifndef _SPHG_PDDL_DOMAIN_H
	#include "Domain.h"
#endif

#ifndef _SPHG_PLANNER_PLAN_H
	#include "Plan.h"
#endif

#ifndef _SPHG_PLANNER_PREDICATE_H
	#include "Predicate.h"
#endif

#ifndef _SPHG_SEARCH_FACADE_H
	#include "Search.h"					// Visibility for Search
#endif

#ifndef _SPHG_PLANNER_STATES_H
	#include "States.h"
#endif

// Memory heat maps purposes
//#include <fstream>
//#include "Runtimes.h"

// ------------------------------------------------------------------------------------------------ Class definition
class StateSpaceForwardChaining : Search {
	// ----- Types
	public:
		typedef		std::map<Predicate, PDDL::HPredicate>	StatePredicates;		// Allowing a fast answer to: What's the integer pointer to this Predicate?
		typedef		INTERNAL_TYPES::u32						IterationNumber;		// Number of search iterations
		typedef		INTERNAL_TYPES::u32						size_type;				// Total memory used by one search

	private:
		typedef		std::deque<Plan>						SearchFrontier;			// Plans waiting to be processed by BreadthExpand/0

		typedef		std::pair<PDDL::Number, Plan>			PlanWithCost;			// A Plan and its floating point cost
		struct Greater_PlanWithCost : public std::binary_function<PlanWithCost, PlanWithCost, bool> {	// Has the left plan a greater cost than the right plan?
			bool operator() (const PlanWithCost& left, const PlanWithCost& right) const {
				return (left.first > right.first);
			}
		};
		typedef		std::priority_queue<PlanWithCost, std::vector<PlanWithCost>, Greater_PlanWithCost>		SearchFrontierWithCosts;	// Plans and their costs waiting to be processed by CostExpand/0; its top() plan has the lowest cost of all plans


	// ----- Properties
	private:
		Domain*										theDomain;						// An access to the PDDL Domain constants, predicates and actions for the search
		Domain::HProblem							theProblem;						// The PDDL Problem (accessed through theDomain) owning this StateSpaceForwardChaining search
		PDDL::HPredicate							theSizeOftheSharedPredicates;	// Computed before search in order to re-initialize future searches

		States::iterator							theInitialState;				// An access to the Initial State in theStates
		States::iterator							theFinalState;					// An access to the Final State in theStates

		Plan										thePlanFound;					// A solution plan found by this StateSpaceForwardChaining
		PDDL::Number								theCostOfthePlanFound;			// A solution plan found by this StateSpaceForwardChaining

		StateSpaceForwardChaining::size_type		s_open_Max;						// [0, 2^32 - 1 = 4294967295] Maximum number of bytes used to store theOpen, over all search iterations
		SearchFrontier								theOpen;						// The StateSpaceForwardChaining Frontier (i.e. the queue of nodes yet to be expanded by this StateSpaceForwardChaining)
		SearchFrontierWithCosts						theOpenWithCosts;				// The StateSpaceForwardChaining Frontier (i.e. the priority queue of nodes and their costs, yet to be expanded by this StateSpaceForwardChaining)

																					// std::vectors
		std::vector<PDDL::Identifier>				theIdentifiers;					// The union of the constants of the domain and the constants of a problem
		std::vector<Predicate>						theSharedPredicates;			// In order to be shared among all states of the StateSpaceForwardChaining, thePredicates of theDomain must be globals to the StateSpaceForwardChaining
		Operator::OperatorSignatures				theSharedOperatorSignatures;	// In order to be shared among all plans of the search frontier, OperatorSignatures must be globals to the StateSpace ForwardChaining
		IndexedState								theStateIndexedByPredicates;	// Index the predicates of a state in an array; make the index the identifier of the predicates for a very quick access

																					// std::maps
		StateSpaceForwardChaining::StatePredicates	theStatesPredicates;			// Predicates, instantiated during search, pointing to positions in theSharedPredicates
		States										theStates;						// Used to check whether a state computed from the application of an operator has already been computed


		//SPHG::Runtimes_hrc R;
		//std::string s;
		//std::ofstream of;

	// ----- Constructor
	public:
		StateSpaceForwardChaining(Domain& aDomain, Domain::HProblem aProblem);


	// ----- Destructor
	public:
		~StateSpaceForwardChaining();


	// ----- Accessors
	public:
		inline Predicate& GetthePredicate(PDDL::HPredicate aPredicate) { return theSharedPredicates[aPredicate]; }
		inline Predicate MakeCopyOfthePredicate(PDDL::HPredicate aPredicate) const { return theSharedPredicates[aPredicate]; }
		inline PDDL::HPredicate AddPredicate(const Predicate& aPredicate) {
			PDDL::HPredicate p = (PDDL::HPredicate) theSharedPredicates.size();
			theSharedPredicates.push_back(aPredicate);
			return p;
		}
		inline std::pair<States::iterator, bool> Insert(State& aState, Plan::length_type aPlanLength) {
			// BreadthExpand/0 does not call Insert/2 because aPlanLength always increases during state-space breadth-first search
			States::iterator it = theStates.find(aState);
			if (theStates.end() == it)
			{
				return theStates.insert(std::pair<State, Plan::length_type>(aState, aPlanLength));
			}
			else // (theStates.end() != it)
			{
				// ((it->second) <= aPlanLength) always true when breadth-first searching
				if ((it->second) > aPlanLength)
				{
					it->second = aPlanLength;
					return std::pair<States::iterator, bool>(it, true);
				}
				else // ((it->second) <= aPlanLength)
					return std::pair<States::iterator, bool>(it, false);
			}
		}
		inline StateSpaceForwardChaining::StatePredicates::const_iterator Find(const Predicate& aPredicate) const {
			return (theStatesPredicates.find(aPredicate));
		}
		inline bool Found(StateSpaceForwardChaining::StatePredicates::const_iterator it) const {
			return (theStatesPredicates.end() != it);
		}
		inline PDDL::HPredicate InsertAsSharedPredicate(const Predicate& aPredicate) {
			StateSpaceForwardChaining::StatePredicates::const_iterator it = theStatesPredicates.find(aPredicate);
			if (theStatesPredicates.end() == it)
			{
				PDDL::HPredicate p = StateSpaceForwardChaining::AddPredicate(aPredicate);
				theStatesPredicates[aPredicate] = p;
				return p;
			}
			else // (theStatesPredicates.end() != it)
				return (it->second);
		}
		inline Operator::HOperatorSignature AddOperatorSignature(Operator::OperatorSignature& anOperatorSignature) {
			Operator::HOperatorSignature p = theSharedOperatorSignatures.size();
			theSharedOperatorSignatures.push_back(anOperatorSignature);
			return p;
		}
		inline const Plan* GetthePlanFound() const { return &thePlanFound; }


	// ----- Operations
	private:
#if (STL == MEMORY_ALLOCATION)
		State Apply(const State& aCurrentState, const Operator* Op, Operator::Predicates& D, PDDL::Parameters P);
		bool UnifyPreconditions(const Operator* Op, Operator::Predicates& D, PDDL::Parameters P, IndexedState& aMappedCurrentState);
#elif (DL_MALLOC == MEMORY_ALLOCATION) || (HPHA == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION)
		State Apply(const State& aCurrentState, const Operator* Op, PDDL::HPredicate* D, PDDL::Parameters P);
		bool UnifyPreconditions(const Operator* Op, PDDL::HPredicate* D, PDDL::Parameters P, IndexedState& aMappedCurrentState);
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif

		bool CostExpand();		// Expanding the cheapest node from the search frontier and adding its costy children to it

		bool BreadthExpand();	// Expanding a node and adding its children to the search frontier in a breadth first manner

	public:
		size_t SizeOf();

		void Reset();			// Re-initialize data before a new search for the same problem

		bool BreadthFirst(bool memory_consumption_is_measured = false /* default is: memory usage is NOT measured */);	// Breadth first search of the state space

		bool BestFirst();		// Cheapest-action-cost-first search of the state space

		std::ostream& Put(std::ostream& o, const Plan* aPlan, const std::string& tab, const IterationNumber aCurrentIteration = 0) const;


	// ----- Interface Operations
	public:
		EXTERNAL_TYPES::op_str* ConvertthePlanFound() const;
		void ConvertthePlanFound(EXTERNAL_TYPES::plan& aPlan /* shall contain the result of the conversion */) const;

		// The returned std::string contains the plans as follows: "action_id_1(param_id_11,...,param_id_1n);action_id_2(param_id_21,...,param_id_2n); ... ; action_id_n(param_id_n1,...,param_id_nn)."
		std::string ConvertthePlanFoundToString() const;

};

#endif	// _SPHG_PLANNER_SEARCH_SSFC_H
// ================================================================================================ End of file "StateSpaceForwardChaining.h"
