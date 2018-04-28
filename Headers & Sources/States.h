// ================================================================================================ Beginning of file "States.h"
// Copyright (c) 2012-2018, Eric Jacopin, ejacopin@ymail.com
// ------------------------------------------------------------------------------------------------
#ifndef _SPHG_PLANNER_STATES_H
#define _SPHG_PLANNER_STATES_H

// ------------------------------------------------------------------------------------------------ Inclusion of files
// STD and STL files
#include <functional>	// Less_State binary_function between two State(s)
#include <map>			// Bijection between a State and the length of the plan which leads to it from theInitialState
#include <valarray>		// Storehouse for a State
#include <vector>		// Predicates in a State "sorted" (or "indexed") by an unsigned integer pointing to their identifier

// Project files
#ifndef _INTERNAL_TYPES_H
	#include "InternalTypes.h"			// Visibility for u8
#endif

#ifndef _SPHG_PDDL_H
	#include "PDDL.h"
#endif

#ifndef _SPHG_TOOLS_USET_H
	#include "uset.h"	// Quick access to a predicate in a (Temporary)State
#endif

// ------------------------------------------------------------------------------------------------ Declarations
//
typedef		std::valarray<PDDL::HPredicate>							State;				// Integer pointers to predicates // By definition, a State is assumed to be sorted; i.e. the integer pointers are sorted

#if defined(NUMBER_OF_SLOTS_IN_A_USET)	/* defined in "ConditionalCompiling.h" */
	typedef	SPHG::uset<PDDL::HPredicate, NUMBER_OF_SLOTS_IN_A_USET>	TemporaryState;		// Quickly accessing predicates in a state
#else	// There is no previous #define directive for NUMBER_OF_SLOTS_IN_A_USET
	#pragma message (__FILE__ "(" STRING(__LINE__) "): <" STRING(NUMBER_OF_SLOTS_IN_A_USET) "> is undefined (see file "States.h").")
	#error NUMBER_OF_SLOTS_IN_A_USET
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////// struct Less_State
// States can be stored to check whether they have already been reached; the States Store house
// (see definition below) sorts states by their contents (integers which act as pointers to
// predicates)

struct less_State : public std::binary_function<State, State, bool>
{
	bool operator() (const State& left, const State& right) const {
		// both left and right are sorted in increasing order
		//
		// if the two states have the same number of predicates then
		//		compare the predicates until one (in the left state) is either less or greater than the other (in the right state)
		// else
		//		compare the number of predicates in the two states
		if (left.size() == right.size())
		{
			for (TemporaryState::size_type i = 0; i < left.size(); ++i)
				if (left[i] < right[i])
					return true;
				else if (left[i] > right[i])
					return false;

			return false;
		}
		else
			return (left.size() < right.size());
	}
};

typedef		std::map<State, INTERNAL_TYPES::u8, less_State>		States;			// A State and the length of the plan which leads to it from theInitialState
typedef		std::vector<std::vector<PDDL::HPredicate>* >		IndexedState;	// Predicates in a State "sorted" (or "indexed") by an unsigned integer pointing to their identifier

/////////////////////////////////////////////////////////////////////////////////////////////////// Included/2
// In a forward search of the state space:
// WHEN ALL the predicates of theCurrentState are predicates of theFinalState THEN
//		the plan which produced theCurrentState is a solution to the planning problem
//
inline bool Included(const State& theCurrentState, const State& theFinalState) {
	if (theFinalState.size() > theCurrentState.size())
		return false;
	else // (theFinalState.size() <= theCurrentState.size())
	{
		TemporaryState::size_type a = 0;
		TemporaryState::size_type sz = (TemporaryState::size_type) theCurrentState.size();
		for (TemporaryState::size_type b = 0; b < theFinalState.size(); ++b)
		{
			for (; a < sz; ++a)
			{
				if (theFinalState[b] < theCurrentState[a])
					return false;
				else if (theFinalState[b] == theCurrentState[a])
				{
					if (a < (sz - 1))
					  ++a;
					break;
				}
				// Why there is no else case:
				//	When (theFinalState[b] > theCurrentState[a]), we need to check the remaining
				//	positions of theCurrentState which shall be taken care of by the next iteration
			}

			if (a == sz)
				// When this happens, all remaining positions of theCurrentState have been checked
				// but there still are unchecked positions in theFinalState
				return false;
		}

		return true;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////// Convert/1
// As, for efficiency matters, there are two memory structures for a state (cf. State and
// TemporaryState above), here are two routines to convert one into the other:
//
// ------------------------------------------------------------------------------------------------
// Converting an std::valarray<HPredicate> into an SPHG::uset<HPredicate, NUMBER_OF_SLOTS_IN_A_USET>
inline TemporaryState Convert(const State& aState) {
	// Note that we DO NOT check whether aTemporaryState is big enough to contain aState
	TemporaryState aTemporaryState;
	for (TemporaryState::size_type i = 0; i < aState.size(); ++i)
		aTemporaryState.insert(aState[i]);
	return aTemporaryState;
}

// ------------------------------------------------------------------------------------------------
// Converting an SPHG::uset<HPredicate, NUMBER_OF_SLOTS_IN_A_USET> into an std::valarray<HPredicate>
inline State Convert(const TemporaryState& aTemporaryState) {
	// An std::valarray can't be resized, so it's important to get it right now:
	State aState(aTemporaryState.size());

	TemporaryState::size_type last = aTemporaryState.last();
	 for (TemporaryState::size_type size = 0, i = aTemporaryState.first(); i <= last; ++i)
	 {
		 if (aTemporaryState.find(i))
			 aState[size++] = i;
	 }

	 return aState;
}

#endif		// _SPHG_PLANNER_STATES_H
// ================================================================================================ End of file "States.h"