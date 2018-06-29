// ================================================================================================ Beginning of file "StateSpaceForwardChaining.cpp"
// Copyright (c) 2012-2018, Eric Jacopin, ejacopin@ymail.com
/////////////////////////////////////////////////////////////////////////////////////////////////// File Content
//
//	  StateSpaceForwardChaining/2 --- Constructs various search structures from the domain and the problem
//	 ~StateSpaceForwardChaining/0 --- Frees the space allocated with theStateIndexedByPredicates
//						  Reset/0 --- Must be called between two successive searches for the same problem
//						  Apply/4 --- Make a new state when all the preconditions have been unified and all parameters are valued
//		   UnifyPreconditions/4-5 --- Unify each of the positive preconditions of this operator with predicates from the current situation
//				  BreadthExpand/0 --- Apply the domain operators to the state of the front Plan of the search frontier and push back the new plans
//				   BreadthFirst/1 --- Expands the shallowest plan in the state space first
//					 CostExpand/0 --- Apply the domain operators to the state of the lowest cost Plan of the search frontier and push the new plans with updated costs
//					  BestFirst/0 --- Expands the least cost plan in the state space first
//							Put/4 --- Writing (a plan + its resulting state) to an ostream
//						 SizeOf/0 --- Computes the memory used by this search
//			ConvertthePlanFound/0 --- Consert (the plan found by the search) to the user's desired type
//			ConvertthePlanFound/1 --- Consert (the plan found by the search) to the some STL structure
//	ConvertthePlanFoundToString/0 --- Putting the actions and parameters identifiers into an std::string
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////// Inclusion of files
//
// STL and STD files
#include <iostream>						// Visibility for std::cout, std::endl
#include <map>							// Accessing the table of string constants for this search
#include <math.h>						// Visibility for floor/1 and log10/1

// Project files
#ifndef _SPHG_PLANNER_CONDITIONAL_COMPILING_H
	#include "ConditionalCompiling.h"		// Visibility for MEMORY_ALLOCATION, STL, HPHA, CLASSIC
#endif

#ifndef _SPHG_PLANNER_OPERATOR_H
	#include "Operator.h"
#endif

#ifndef _SPHG_PLANNER_PREDICATE_H
	#include "Predicate.h"
#endif

#ifndef _SPHG_PLANNER_SEARCH_SSFC_H
	#include "StateSpaceForwardChaining.h"	// Class visibility
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////// StateSpaceForwardChaining::StateSpaceForwardChaining/2
//
StateSpaceForwardChaining::StateSpaceForwardChaining(Domain& aDomain, Domain::HProblem aProblem) : theCostOfthePlanFound(0.0f) {
	// Set the pointers which hold necessary data for this search
	theDomain = &aDomain;
	theProblem = aProblem;

	// Build the table of string constants for this search
	for (std::map<PDDL::HIdentifier, PDDL::Identifier>::const_iterator i = ((theDomain->GettheProblem(theProblem))->GettheConstants()).begin(); i != ((theDomain->GettheProblem(theProblem))->GettheConstants()).end(); ++i)
		theIdentifiers.push_back(i->second);

	// Get the table of shared predicates for this search
	//	1. First, get the the shared predicates from theDomain
	theSharedPredicates = theDomain->GetthePredicates();

	//	2. Second, get the shared predicates from theProblem
	std::vector<Predicate> Temp = (theDomain->GettheProblem(theProblem))->GetthePredicates();
	for (PDDL::HPredicate p = 0; p < Temp.size(); ++p)
		theSharedPredicates.push_back(Temp[p]);

	//	3. Remember the current number of shared predicates in order to ease future searches
	theSizeOftheSharedPredicates = (PDDL::HPredicate) theSharedPredicates.size();

	// Set the initial and final state of the problem for this search
	theInitialState = theStates.insert(std::pair<State, Plan::length_type>(Convert((theDomain->GettheProblem(aProblem))->GetthePositiveInitialState()), 0)).first;
	theFinalState =  theStates.insert(std::pair<State, Plan::length_type>(Convert((theDomain->GettheProblem(aProblem))->GetthePositiveGoalState()), 0)).first;

	// Build theStatePredicates from theInitialState and theFinalState
	for (PDDL::HPredicate i = 0; i < (theInitialState->first).size(); ++i)
		theStatesPredicates[theSharedPredicates[((theInitialState->first)[i])]] = ((theInitialState->first)[i]);
	for (PDDL::HPredicate i = 0; i < (theFinalState->first).size(); ++i)
		theStatesPredicates[theSharedPredicates[((theFinalState->first)[i])]] = ((theFinalState->first)[i]);

	// Finally, register this search for aProblem
	(theDomain->GettheProblem(aProblem))->Register(this);

	// One more thing... Initialize map from the predicate identifiers to the predicates in the current state
	Predicate::PredicatesStore::size_type s = theDomain->GetthePredicatesStoreSize();
	for (Predicate::PredicatesStore::size_type i = 0; i < s; ++i)
		theStateIndexedByPredicates.push_back(new std::vector<PDDL::HPredicate>);
}

/////////////////////////////////////////////////////////////////////////////////////////////////// StateSpaceForwardChaining::~StateSpaceForwardChaining/0
//
StateSpaceForwardChaining::~StateSpaceForwardChaining() {
	Predicate::PredicatesStore::size_type s = theDomain->GetthePredicatesStoreSize();
	for (Predicate::PredicatesStore::size_type i = 0; i < s; ++i)
		delete theStateIndexedByPredicates[i];
}


/////////////////////////////////////////////////////////////////////////////////////////////////// StateSpaceForwardChaining::Reset/0
//
void StateSpaceForwardChaining::Reset() {
	// Reset the table of shared predicates for this search
	theSharedPredicates.resize(theSizeOftheSharedPredicates);

	// Reset the initial and final state of the problem for this search:
	//	1. Create a temporary store for both the initial and final states of the problem
	States Temp;
	theInitialState = (Temp.insert((*theInitialState))).first;
	theFinalState = (Temp.insert((*theFinalState))).first;
	//	2. Make theStates fresh with theInitialState and theFinalState of the problem
	theStates.clear();
	theInitialState = (theStates.insert((*theInitialState))).first;
	theFinalState = (theStates.insert((*theFinalState))).first;

	// Build theStatePredicates from theInitialState and theFinalState
	theStatesPredicates.clear();
	for (PDDL::HPredicate i = 0; i < (theInitialState->first).size(); ++i)
		theStatesPredicates[theSharedPredicates[((theInitialState->first)[i])]] = ((theInitialState->first)[i]);
	for (PDDL::HPredicate i = 0; i < (theFinalState->first).size(); ++i)
		theStatesPredicates[theSharedPredicates[((theFinalState->first)[i])]] = ((theFinalState->first)[i]);
}

/////////////////////////////////////////////////////////////////////////////////////////////////// StateSpaceForwardChaining::Apply/4
//
#if (STL == MEMORY_ALLOCATION)
State StateSpaceForwardChaining::Apply(const State& aCurrentState, const Operator* Op, Operator::Predicates& D, PDDL::Parameters P) {

		TemporaryState newCurrentState = Convert(aCurrentState);	// the state resulting from deleting the deletions and adding the additions

		Operator::HPredicate thePositionOfDeletedPreconditions = Op->GetthePositionOfDeletedPreconditions();
		Operator::HPredicate thePositionOfDeletions = Op->GetthePositionOfDeletions();
		Operator::HPredicate thePositionOfAdditions = Op->GetthePositionOfAdditions();
		Operator::HPredicate thePositionOfNegativePreconditions = Op->GetthePositionOfNegativePreconditions();

		// Substract the deleted preconditions from aCurrentState (results in newCurrentState)
		for (Operator::HPredicate del_pre = thePositionOfDeletedPreconditions; del_pre < thePositionOfDeletions; ++del_pre)
		{
			// In this range, all the predicates in D are preconditions which are deleted; consequently, 
			// erase/1 shall return 0; we disgard this returned value because we are in the good range.
			newCurrentState.erase(D[del_pre]);		// erase/1 returns the number of elements that have been removed from the set
		}

		for (Operator::HPredicate del = thePositionOfDeletions; del < thePositionOfAdditions; ++del)
		{
			// In this range, all the predicates only belong to the delete list.
			// Get the position of this deleted predicate in theStatesPredicates and delete it in a newCurrentState
			// (after the predicate parameters have been updated with the instantiated parameters of this Operator)
			StateSpaceForwardChaining::StatePredicates::const_iterator it = StateSpaceForwardChaining::Find( StateSpaceForwardChaining::MakeCopyOfthePredicate(Op->GetthePredicate(del)).UpdatetheParameters(P));
			if (StateSpaceForwardChaining::Found(it))
			{
				newCurrentState.erase(it->second);
			}
		}

		// Union the the set of predicates of newCurrentState with the added predicates of this Operator
		for (Operator::HPredicate add = thePositionOfAdditions; add < thePositionOfNegativePreconditions; ++add)
		{
			// In this range, all the predicates only belong to the add list.
			// Get the position of anAddedPredicate in theStatesPredicates and insert it in a newCurrentState
			// (after the predicate parameters have been updated with the instantiated parameters of this Operator)
			newCurrentState.insert(StateSpaceForwardChaining::InsertAsSharedPredicate( StateSpaceForwardChaining::MakeCopyOfthePredicate(Op->GetthePredicate(add)).UpdatetheParameters(P)));
		}

		return Convert(newCurrentState);
	}
#elif (DL_MALLOC == MEMORY_ALLOCATION) ||(HPHA == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION)
	State StateSpaceForwardChaining::Apply(const State& aCurrentState, const Operator* Op, PDDL::HPredicate* D, PDDL::Parameters P) {

		TemporaryState newCurrentState = Convert(aCurrentState);	// the state resulting from deleting the deletions and adding the additions

		Operator::HPredicate thePositionOfDeletedPreconditions = Op->GetthePositionOfDeletedPreconditions();
		Operator::HPredicate thePositionOfDeletions = Op->GetthePositionOfDeletions();
		Operator::HPredicate thePositionOfAdditions = Op->GetthePositionOfAdditions();
		Operator::HPredicate thePositionOfNegativePreconditions = Op->GetthePositionOfNegativePreconditions();

		// Substract the deleted preconditions from aCurrentState (results in newCurrentState)
		for (Operator::HPredicate del_pre = thePositionOfDeletedPreconditions; del_pre < thePositionOfDeletions; ++del_pre)
		{
			// In this range, all the predicates in D are preconditions which are deleted; consequently, 
			// erase/1 shall return 0; we disgard this returned value because we are in the good range.
			newCurrentState.erase(D[del_pre]);		// erase/1 returns the number of elements that have been removed from the set
		}

		for (Operator::HPredicate del = thePositionOfDeletions; del < thePositionOfAdditions; ++del)
		{
			// In this range, all the predicates only belong to the delete list.
			// Get the position of this deleted predicate in theStatesPredicates and delete this position in a newCurrentState
			// (after the predicate parameters have been updated with the instantiated parameters of this Operator)
			StateSpaceForwardChaining::StatePredicates::const_iterator it = StateSpaceForwardChaining::Find( StateSpaceForwardChaining::MakeCopyOfthePredicate(Op->GetthePredicate(del)).UpdatetheParameters(P));
			if (StateSpaceForwardChaining::Found(it))
			{
				newCurrentState.erase(it->second);
			}
		}

		// Union the set of predicates of newCurrentState with the added predicates of this Operator
		for (Operator::HPredicate add = thePositionOfAdditions; add < thePositionOfNegativePreconditions; ++add)
		{
			// In this range, all the predicates only belong to the add list.
			// Get the position of anAddedPredicate in theStatesPredicates and insert this position in a newCurrentState
			// (after the predicate parameters have been updated with the instantiated parameters of this Operator)
			newCurrentState.insert( StateSpaceForwardChaining::InsertAsSharedPredicate( StateSpaceForwardChaining::MakeCopyOfthePredicate(Op->GetthePredicate(add)).UpdatetheParameters(P)));
		}

		return Convert(newCurrentState);
	}
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////// StateSpaceForwardChaining::UnifyPreconditions/4-5
//
#if (STL == MEMORY_ALLOCATION)
	bool StateSpaceForwardChaining::UnifyPreconditions(const Operator* Op, Operator::Predicates& D, PDDL::Parameters P, IndexedState& aStateSortedByPredicates) {

		// Check the positive preconditions
		for (std::vector<std::vector<Operator::Positions>>::size_type par = 0; par < Op->GetthePositionsSize(); ++par)
		{ 
			// Check values from these state predicate parameters are all be equal
			PDDL::HIdentifier ref = StateSpaceForwardChaining::GetthePredicate(D[Op->GetthePosition(par, 0).first]).GettheParameter(Op->GetthePosition(par, 0).second);
			for (std::vector<Operator::Positions>::size_type pos = 1; pos < Op->GetthePositionsSize(par); ++pos)
			{
				if (ref != StateSpaceForwardChaining::GetthePredicate(D[Op->GetthePosition(par, pos).first]).GettheParameter(Op->GetthePosition(par,pos).second))
					// Sorry folks but this couple of (state) predicate parameters are inconsistent
					return false;
			}

			// All the predicate parameters are consistent for this Operator parameter par: give it a final value
			P[par].first = PDDL::STRING_VALUE;	// Might be a useful information for debugging purposes
			P[par].second = ref;
		}

		//// Check the positive preconditions
		//for (Operator::HPredicate pos_pre = 0; pos_pre < thePositionOfDeletions; ++pos_pre)
		//{
		//	if (! (aStateSpaceForwardChaining->GetthePredicate(thePredicates[pos_pre])).Unify(aStateSpaceForwardChaining->GetthePredicate(D[pos_pre]), P))
		//	{
		//		 Sorry folks, but the positive precondition thePredicates[pos_pre] does not unify with D[pos_pre]!
		//		return false;
		//	}
		//}

		Operator::HPredicate theNumberOfPredicates = Op->GettheNumberOfPredicates();

		// Check the negative preconditions (requires :negative-preconditions in the PDDL Domain)
		for (Operator::HPredicate neg_pre = Op->GetthePositionOfAddedPreconditions(); neg_pre < theNumberOfPredicates; ++neg_pre)
		{
			// Look for this predicate in theCurrentState
			std::vector<PDDL::HPredicate>* ptr = aStateSortedByPredicates[(StateSpaceForwardChaining::GetthePredicate(Op->GetthePredicate(neg_pre))).GettheIdentifier()];

			// Check the situation predicates (with the same identifier than thePredicates[neg_pre]) do not unify with thePredicates[neg_pre]
			for(std::vector<PDDL::HPredicate>::size_type p = 0; p < ptr->size(); ++p)
			{
				if (StateSpaceForwardChaining::GetthePredicate(Op->GetthePredicate(neg_pre)).Unify(StateSpaceForwardChaining::GetthePredicate(ptr->operator [](p)), P))
					// Sorry folks, but the negative precondition thePredicates[neg_pre] does unify with a situation predicate!
					return false;
			}
		}

		// Report each positive precondition predicate successfully unified with a situation predicate in D
		// and that each negative precondition predicate successfully did not unify with a situation predicate in D
		return true;
	}
#elif (DL_MALLOC == MEMORY_ALLOCATION) || (HPHA == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION)
	bool StateSpaceForwardChaining::UnifyPreconditions(const Operator* Op, PDDL::HPredicate* D, PDDL::Parameters P, IndexedState& aStateSortedByPredicates) {

		// Check the positive preconditions
		for (std::vector<std::vector<Operator::Positions>>::size_type par = 0; par < Op->GetthePositionsSize(); ++par)
		{
			// Check values from these state predicate parameters are allS equal
			PDDL::HIdentifier ref = StateSpaceForwardChaining::GetthePredicate(D[Op->GetthePosition(par, 0).first]).GettheParameter(Op->GetthePosition(par, 0).second);
			for (std::vector<Operator::Positions>::size_type pos = 1; pos < Op->GetthePositionsSize(par); ++pos)
			{
				if (ref != StateSpaceForwardChaining::GetthePredicate(D[Op->GetthePosition(par, pos).first]).GettheParameter(Op->GetthePosition(par, pos).second))
					// Sorry folks but this couple of (state) predicate parameters are inconsistent
					return false;
			}

			// All the predicate parameters are consistent for this Operator parameter par: give it a final value
			P[par].second = ref;
		}

		//// Check the positive preconditions
		//for (Operator::HPredicate pos_pre = 0; pos_pre < thePositionOfDeletions; ++pos_pre)
		//{
		//	if (! (aStateSpaceForwardChaining->GetthePredicate(thePredicates[pos_pre])).Unify(aStateSpaceForwardChaining->GetthePredicate(D[pos_pre]), P))
		//	{
		//		// Sorry folks, but the positive precondition thePredicates[pos_pre] does not unify with D[pos_pre]!
		//		return false;
		//	}
		//}

		Operator::HPredicate theNumberOfPredicates = Op->GettheNumberOfPredicates();

		// Check the negative preconditions (requires :negative-preconditions in the PDDL Domain)
		for (Operator::HPredicate neg_pre = Op->GetthePositionOfAddedPreconditions(); neg_pre < theNumberOfPredicates; ++neg_pre)
		{
			// Look for this predicate in theCurrentState
			std::vector<PDDL::HPredicate>* ptr = aStateSortedByPredicates[(StateSpaceForwardChaining::GetthePredicate(Op->GetthePredicate(neg_pre))).GettheIdentifier()];

			// Check the situation predicates (with the same identifier than thePredicates[neg_pre]) do not unify with thePredicates[neg_pre]
			for(std::vector<PDDL::HPredicate>::size_type p = 0; p < ptr->size(); ++p)
			{
				if (StateSpaceForwardChaining::GetthePredicate(Op->GetthePredicate(neg_pre)).Match(StateSpaceForwardChaining::GetthePredicate(ptr->operator [](p)), P))
					// Sorry folks, but the negative precondition thePredicates[neg_pre] does unify with a situation predicate!
					return false;
			}
		}

		// Report each precondition predicate successfully unified with a situation predicate in D
		// and that each negative precondition predicate successfully did not unify with a situation predicate in D
		return true;
	}
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////// StateSpaceForwardChaining::BreadthExpand/0
//
bool StateSpaceForwardChaining::BreadthExpand() {

	// Get a reference to the front Plan of theOpen
	const Plan& aPlan = theOpen.front();
	// Get a reference to the current state of aPlan
	const State& aCurrentState = (aPlan.GettheCurrentState())->first;

	// First, Map the predicates of theCurrentState from theIdentifier of these predicates
	// This begins with memory cleaning
	Predicate::PredicatesStore::size_type s = theDomain->GetthePredicatesStoreSize();
	for (Predicate::PredicatesStore::size_type i = 0; i < s; ++i)
		theStateIndexedByPredicates[i]->clear();
	// And continue with building the desired mapping
	for (TemporaryState::size_type i = 0; i < aCurrentState.size(); ++i)
		theStateIndexedByPredicates[theSharedPredicates[aCurrentState[i]].GettheIdentifier()]->push_back(aCurrentState[i]);

	// Compute children for each possible operator from theDomainOperators
	for (Domain::HOperator Op = 0; Op < theDomain->GettheNumberOfOperators(); ++Op)
	{
		// Get a pointer to the current operator
		const Operator* aCandidate = theDomain->GettheOperator(Op);
		Operator::HPredicate NumberOfPositivePreconditions = aCandidate->GettheNumberOfPositivePreconditions();
		Operator::HArity NumberOfParameters = aCandidate->GettheNumberOfParameters();

		// C indexes all the idenfitiers of a precondition predicate of operator Op together with all their occurences in theStateIndexedByPredicates
		typedef std::vector<PDDL::HPredicate>* info;
		// struct info { std::vector<HPredicate>* p; std::vector<HPredicate>::size_type s;};
#if (STL == MEMORY_ALLOCATION)
		std::vector<info> C(NumberOfPositivePreconditions);
#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
		info* C = (info*) SPHG_ALLOC(sizeof(info) * NumberOfPositivePreconditions);
#elif (HPHA == MEMORY_ALLOCATION)
		info* C = (info*) SPHG_ALLOC(sizeof(info) * NumberOfPositivePreconditions, sizeof(info));
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif

#if (SPHG_MEMORY_MAPPING)
	#if (STL == MEMORY_ALLOCATION)
			std::vector<info> C(NumberOfPositivePreconditions);
			of << "{C, vector, " << ", " << R.TimeStamp() << ", " << (sizeof(info) * NumberOfPositivePreconditions) + sizeof(C) << ", " << sizeof(info) << "}," << std::endl;
	#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (HPHA == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
			of << "{C, alloc, " << (unsigned long) C << ", " << R.TimeStamp() << ", " << (sizeof(info) * NumberOfPositivePreconditions) << ", " << sizeof(info) << "}," << std::endl;
	#else
		#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
		#error MEMORY_ALLOCATION
	#endif
#endif

		// Check whether aCandidate is applicable (i.e. is there a precondition which is not a member of theCurrentState?), one precondition predicate at a time
		bool aCandidate_is_applicable = true;	// Is aCandidate applicable to theCurrentState?
		for (Operator::HPredicate pre = 0; pre < NumberOfPositivePreconditions; ++pre)
		{
			// Look for the predicate identifier of precondition pre in theCurrentState
			std::vector<PDDL::HPredicate>* ptr = theStateIndexedByPredicates[theSharedPredicates[aCandidate->GetthePredicate(pre)].GettheIdentifier()];

			if (0 == ptr->size())
			{
				// This (pointer to) precondition predicate identifier has no match in theCurrentState: aCandidate cannot be applied to theCurrentState
				aCandidate_is_applicable = false;
				break;
			}

			// Remember that this predicate identifier of theCurrentState matches the precondition predicate of precondition pre
			C[pre] = ptr;
		}

		//	When aCandidate is not applicable, go to next Op in theDomainOperators
		if (aCandidate_is_applicable)
		{
			// D shall contain a set of predicates from the current situation with the following properties:
			//	- same number of predicate than the NumberOfPositivePreconditions of aCandidate we try to apply to the current situation
			//	- each predicate has an identifier which is the same than that of a precondition predicate of Op
			//	- all predicates are different
#if (STL == MEMORY_ALLOCATION)
			Operator::Predicates D(NumberOfPositivePreconditions);
#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
			PDDL::HPredicate* D = (PDDL::HPredicate*) SPHG_ALLOC(sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions);
#elif (HPHA == MEMORY_ALLOCATION)
			PDDL::HPredicate* D = (PDDL::HPredicate*) SPHG_ALLOC(sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions, sizeof(PDDL::HPredicate));
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif

#if (SPHG_MEMORY_MAPPING)
	#if (STL == MEMORY_ALLOCATION)
			of << "{D, vector, " << ", " << R.TimeStamp() << ", " << (sizeof(info) * NumberOfPositivePreconditions) + sizeof(D) << ", " << sizeof(info) << "}," << std::endl;
	#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (HPHA == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
			of << "{D, alloc, " << (unsigned long) D << ", " << R.TimeStamp() << ", " << sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions << ", " << sizeof(PDDL::HPredicate) << "}," << std::endl;
	#else
		#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
		#error MEMORY_ALLOCATION
	#endif
#endif

			// We need to extract predicates from the vectors in C.
			// To this achieve this extraction, we use an integer number whose digits are stored in P.
			// Each digit ranges from 0 to the size of a vector in C.
			// We start with the number (0)(0)...(0)(0), which means we point to the first positions of all the vectors in C.
			// Then we increase this number by 1 (we begin by increasing the left-most digit)
			// When a digits reaches the size of its corresponding vector in C, we reset it to 0 and we increase the next digit by 1.
			// This eventually stops when the last digit reaches the size of its corresponding vector in C; we then try to apply the next Operator.
#if (STL == MEMORY_ALLOCATION)
			State P((PDDL::HPredicate) 0 /* Initial Value for each position in P */, NumberOfPositivePreconditions /* Size of P */);
			Operator::Parameters theParametersOfaCandidate(aCandidate->GettheParameters());
#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
			PDDL::HPredicate* P = (PDDL::HPredicate*) SPHG_ALLOC(sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions);
			// Begin with pointing to the first positions of the vectors in C
			memset(P, 0, sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions);
			PDDL::Parameters theParametersOfaCandidate = (PDDL::Parameters) SPHG_ALLOC(NumberOfParameters * sizeof(PDDL::Parameter));
#elif (HPHA == MEMORY_ALLOCATION)
			PDDL::HPredicate* P = (PDDL::HPredicate*) SPHG_ALLOC(sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions, sizeof(PDDL::HPredicate));
			//of << "{P, alloc, " << (unsigned long) P << ", " << R.TimeStamp() << ", " << sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions << ", " << sizeof(PDDL::HPredicate) << "}," << std::endl;
			// Begin with pointing to the first positions of the vectors in C
			memset(P, 0, sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions);
			PDDL::Parameters theParametersOfaCandidate = (PDDL::Parameters) SPHG_ALLOC(NumberOfParameters * sizeof(PDDL::Parameter), sizeof(PDDL::Parameter));
			//of << "{theParametersOfaCandidate, alloc, " << (unsigned long) theParametersOfaCandidate << ", " << R.TimeStamp() << ", " << NumberOfParameters * sizeof(PDDL::Parameter) << ", " << sizeof(PDDL::Parameter) << "}," << std::endl;
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif

			Operator::HPredicate i;	// gets initialized to 0 at each cycle in the loop below
			do
			{
				// Build a bijection from the preconditions of aCandidate to predicates from theCurrentState, based on matching theIdentifiers
				// D represents this bijection:
				//	- an index i of D is a precondition number
				//	- D[i] is a predicate in theCurrenTState
				// For the unification process to succeed the following condition must hold:
				//		For all i, j indexes of D, i != j, D[i] != D[j]
				// That is, we're not going to try to unify to distinct preconditions to the same predicate in theCurrentState
				for (Operator::HPredicate p = 0; p < NumberOfPositivePreconditions; ++p)
				{
					// Get a predicate with identifier C[p] from the current situation
					PDDL::HPredicate d = C[p]->operator [](P[p]);

					// Check whether this predicate is already in D
					for (Operator::HPredicate j = 0; j < p; ++j)
						if (D[j] == d)
							// IF a predicate is chosen twice THEN go to next set of predicates from theCurrentState
/********* GOTO *********/	goto NEXT_STATE_PREDICATES;

					// Remember predicate d when it is different from all other predicates already in D
					D[p] = d;
				}

				{
				// All predicates in D are different; try to unify the preconditions of the Operator Op with the predicates in D

					// Unify the preconditions of Op with the predicates in D
#if (STL == MEMORY_ALLOCATION)
					if (UnifyPreconditions(aCandidate, D, theParametersOfaCandidate, theStateIndexedByPredicates))
#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION) || (HPHA == MEMORY_ALLOCATION))
					aCandidate->CopytheParametersInto(theParametersOfaCandidate);
					if (UnifyPreconditions(aCandidate, D, theParametersOfaCandidate, theStateIndexedByPredicates))
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif

					{
						// Apply theDomainOperators[Op] to theCurrentState and check whether the resulting state is new
						State aState = StateSpaceForwardChaining::Apply(aCurrentState, aCandidate, D, theParametersOfaCandidate);

						// Make a child plan when the resulting state is new
						if (theStates.end() == theStates.find(aState))
						{
							// Put this new state into theStates data store
							States::iterator new_state = theStates.insert(std::pair<State, Plan::length_type>(aState, aPlan.Length() + 1)).first;

							// Remember this child plan leading to this new state
							Operator::HOperatorSignatures anOS( *(aPlan.GettheOperatorSignatures()) );
							Operator::OperatorSignature tempOS = aCandidate->MaketheOperatorSignature(theParametersOfaCandidate);
							anOS.push_back(AddOperatorSignature(tempOS));

#if defined(_DEBUG)

							Plan theCurrentChild = Plan(new_state, anOS);
							std::cout << "The current state is {";
							for (TemporaryState::size_type i = 0; i < (aPlan.GettheCurrentState())->first.size(); ++i)
							{
								Predicate aPredicate = theSharedPredicates[((aPlan.GettheCurrentState()->first)[i])];
								std::cout << theIdentifiers[aPredicate.GettheIdentifier()] << "(";
								for (Predicate::HArity a = 0; a < aPredicate.GettheArity(); ++a)
								{
									std::cout << theIdentifiers[aPredicate.GettheParameter(a)];
									if (a < (aPredicate.GettheArity() - 1))
										 std::cout << ",";
								}

								if (i < ((aPlan.GettheCurrentState())->first.size() - 1))
									std::cout << "), ";
							}
							std::cout << ")}." << std::endl << "   Applying operator ";

							Operator::OperatorSignature currentOS = theSharedOperatorSignatures[(theCurrentChild.GettheOperatorSignatures())->back()];
							std::cout << theIdentifiers[currentOS[currentOS.size() - 1]] << "(";
							for (Operator::HArity a = 0; a < (currentOS.size() - 1); ++a)
								if (a == (currentOS.size() - 2))
									std::cout << theIdentifiers[currentOS[a]];
								else
									std::cout << theIdentifiers[currentOS[a]] << ",";

							// Write a ')' at the end of the list of parameters but also when the theOperatorSignature[i].second.size() is 0 (zero)
							std::cout << ")" << std::endl;

							std::cout << "   The resulting state is {";
							for (size_t i = 0; i < (theCurrentChild.GettheCurrentState())->first.size(); ++i)
							{
								Predicate aPredicate = theSharedPredicates[((theCurrentChild.GettheCurrentState()->first)[i])];
								std::cout << theIdentifiers[aPredicate.GettheIdentifier()] << "(";
								for (Predicate::HArity a = 0; a < aPredicate.GettheArity(); ++a)
								{
									std::cout << theIdentifiers[aPredicate.GettheParameter(a)];
									if (a < (aPredicate.GettheArity() - 1))
										 std::cout << ",";
								}

								if (i < ((theCurrentChild.GettheCurrentState())->first.size() - 1))
									std::cout << "), ";
							}
							std::cout << ")}." << std::endl << std::endl;
#endif	// defined(_DEBUG)
							// Report immediately when this child plan is a solution
							if (Included(aState, theFinalState->first))
							{
								// Memorize this child plan as the current solution
								thePlanFound = Plan(new_state, anOS);

#if (STL == MEMORY_ALLOCATION)
								// Nothing to do here: the STL takes care of the memory management for its containers
#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
								SPHG_FREE(theParametersOfaCandidate);
								SPHG_FREE(D);
								SPHG_FREE(P);
								SPHG_FREE(C);
#elif (HPHA == MEMORY_ALLOCATION)
								//of << "{theParametersOfaCandidate, free1, " << (unsigned long) theParametersOfaCandidate << ", " << R.TimeStamp() << ", " << NumberOfParameters * sizeof(PDDL::Parameter) << ", " << sizeof(PDDL::Parameter) << "}," << std::endl;
								SPHG_FREE((PDDL::Parameters) theParametersOfaCandidate, sizeof(PDDL::Parameter) * NumberOfParameters, sizeof(PDDL::Parameter));
								//of << "{D, free1, " << (unsigned long) D << ", " << R.TimeStamp() << ", " << sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions << ", " << sizeof(PDDL::HPredicate) << "}," << std::endl;
								SPHG_FREE((PDDL::HPredicate*) D, sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions, sizeof(PDDL::HPredicate));
								//of << "{P, free1, " << (unsigned long) P << ", " << R.TimeStamp() << ", " << sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions << ", " << sizeof(PDDL::HPredicate) << "}," << std::endl;
								SPHG_FREE((PDDL::HPredicate*) P, sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions, sizeof(PDDL::HPredicate));
								//of << "{C, free1, " << (unsigned long) C << ", " << R.TimeStamp() << ", " << sizeof(info) * NumberOfPositivePreconditions << ", " << sizeof(info) << "}," << std::endl;
								SPHG_FREE((info*) C, sizeof(info) * NumberOfPositivePreconditions, sizeof(info));
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif

								// Report this child plan is a solution
								return true;
							}

							// This child plan IS NOT a solution; enqueue it so as to BreadthExpand/0 it later
							theOpen.push_back(Plan(new_state, anOS));
						}
					}
				}

/*** LABEL ***/	NEXT_STATE_PREDICATES:
				// Increase the number in P, possibly re-setting a digit to 0 when the increase reaches the size of a vector in C
				i = 0;	// Start with precondition number 0
				while ((i < NumberOfPositivePreconditions) && ((++P[i]) == C[i]->size()))
				{
					P[i] = 0;
					++i;
				}
			}
			// When the NumberOfPositivePreconditions has been reached, it means that all possible combinations of predicates
			// from theCurrentState have been generated and tested against the positive preconditions of Op; so it's time to break this loop
			while (i < NumberOfPositivePreconditions);

#if (STL == MEMORY_ALLOCATION)
			// Nothing to do here: the STL takes care of the memory management for its containers
#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
			SPHG_FREE(theParametersOfaCandidate);
			SPHG_FREE(D);
			SPHG_FREE(P);
#elif (HPHA == MEMORY_ALLOCATION)
			//of << "{theParametersOfaCandidate, free3, " << (unsigned long) theParametersOfaCandidate << ", " << R.TimeStamp() << ", " << NumberOfParameters * sizeof(PDDL::Parameter) << ", " << sizeof(PDDL::Parameter) << "}," << std::endl;
			SPHG_FREE((PDDL::Parameters) theParametersOfaCandidate, sizeof(PDDL::Parameter) * NumberOfParameters, sizeof(PDDL::Parameter));
			//of << "{D, free3, " << (unsigned long) D << ", " << R.TimeStamp() << ", " << sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions << ", " << sizeof(PDDL::HPredicate) << "}," << std::endl;
			SPHG_FREE((PDDL::HPredicate*) D, sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions, sizeof(PDDL::HPredicate));
			//of << "{P, free3, " << (unsigned long) P << ", " << R.TimeStamp() << ", " << sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions << ", " << sizeof(PDDL::HPredicate) << "}," << std::endl;
			SPHG_FREE((PDDL::HPredicate*) P, sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions, sizeof(PDDL::HPredicate));
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif
		}

#if (STL == MEMORY_ALLOCATION)
		// Nothing to do here: the STL takes care of the memory management for its containers
#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
		SPHG_FREE(C);
#elif (HPHA == MEMORY_ALLOCATION)
		//of << "{C, free4, " << (unsigned long) C << ", " << R.TimeStamp() << ", " << sizeof(info) * NumberOfPositivePreconditions << ", " << sizeof(info) << "}," << std::endl;
		SPHG_FREE((info*) C, sizeof(info) * NumberOfPositivePreconditions, sizeof(info));
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif
	}

	// Report that no solution has been found during this expansion; maybe next time...
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////// StateSpaceForwardChaining::BreadthFirst/1
//
bool StateSpaceForwardChaining::BreadthFirst(bool memory_consumption_is_measured /* default value is false */) {

	IterationNumber theCurrentIteration = 0;		// No iteration in the while loop below, yet (we won't iterate when theInitialState is included in theFinalState
	size_t s_open = 0;								// theOpen initially is empty
	s_open_Max = 0;									// Record the maximum memory size of the search

	//s = "memory heat map.txt";
	//of.open(s.c_str());
	//of.setf(std::ios_base::fixed, std::ios_base::floatfield);
	//of.precision(8);

	// Check whether theFinalState is included in theInitialState
	if (Included(theInitialState->first, theFinalState->first))
	{
		// Memorize this empty solution for external access
		thePlanFound = Plan(theInitialState);

		// Tell the user you did a really good job!
		return true;
	}
	else // TheFinalState IS NOT INCLUDED in the InitialState: this search starts now...
	{
		SearchFrontier::iterator theOpen_last;	// used to compute memory size of theOpen

		// Prepare for the ride on the Planning Frontier
		theOpen.push_back(Plan(theInitialState));

		// When the user wants to know more about the memory consumption, record the movements on theOpen
		if (memory_consumption_is_measured)
		{
			s_open = sizeof(theOpen) + sizeof(theOpen.front());
			s_open_Max = s_open;
			theOpen_last = theOpen.begin();
		}

		// ... and go for it!
		while (! theOpen.empty())
		{// StateSpaceForwardChaining as long as there are Plans to expand

			// Compute the children of the first plan appearing in theOpen
			if (BreadthExpand())
			{
				// BreadthExpand/0 memorized the solution child into thePlanFound

				// No need for heavy memory any longer
				theOpen.clear();

				// Tell the user you did a good job!
				return true;
			}

			// When the user wants to know more about the memory consumption, record the movements on theOpen
			if (memory_consumption_is_measured)
			{
				// Point to the first node added to theOpen by BreadthExpand/0
				++theOpen_last;
				// But if BreadthExpand/0 added nothing to theOpen then theOpen_last now points to (theOpen.end())...
				if (theOpen.end() == theOpen_last)
					// ... so get one node back in theOpen
					--theOpen_last;
				else // (theOpen.end() != theOpen_last)
					// Count the memory usage of the nodes added to theOpen by BreadthExpand/0
					for(SearchFrontier::iterator it = theOpen_last; it != theOpen.end(); ++it)
					{
						s_open += sizeof(*it);
						// Remember the memory usage of this node (it points to this node) was counted
						theOpen_last = it;
					}

				// Record, if necessary, the maximum memory consumption of theOpen
				if (s_open_Max < s_open) s_open_Max = s_open;

				std::cout << "Search step #" << (++theCurrentIteration) << " (" << INTERNAL_TYPES::u16(theOpen.front().Length()) << "): "
					<< theOpen.size() << " open plans for " << s_open << " bytes (Maximum total search size is " 
					<< StateSpaceForwardChaining::SizeOf() << " bytes)." << std::endl;

				// This substraction is in advance of the call to pop_front/0 just below
				s_open -= sizeof(theOpen.front());
			}

			// No child-plan is a solution: pop the parent-plan from the search frontier
			theOpen.pop_front();					
		}

		// Tell the user the search space became empty before a solution was found
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////// StateSpaceForwardChaining::CostExpand/0
//
bool StateSpaceForwardChaining::CostExpand() {

	// Get the lowest cost of theOpenWithCosts
	const PDDL::Number aCost = theOpenWithCosts.top().first;
	// Get a reference to the lowest cost Plan of theOpenWithCosts
	const Plan aPlan = theOpenWithCosts.top().second;
	// Get a reference to the current state of aPlan
	const State& aCurrentState = (aPlan.GettheCurrentState())->first;
	// As inserting children plans shall modify the top plan, pop aPlan (i.e. the current top plan) NOW from the search frontier
	theOpenWithCosts.pop();					

	// First, Map the predicates of theCurrentState from theIdentifier of these predicates
	// This begins with memory cleaning
	Predicate::PredicatesStore::size_type s = theDomain->GetthePredicatesStoreSize();
	for (Predicate::PredicatesStore::size_type i = 0; i < s; ++i)
		theStateIndexedByPredicates[i]->clear();
	// And continue with building the desired mapping
	for (TemporaryState::size_type i = 0; i < aCurrentState.size(); ++i)
		theStateIndexedByPredicates[theSharedPredicates[aCurrentState[i]].GettheIdentifier()]->push_back(aCurrentState[i]);

	// Compute children for each possible operator from theDomainOperators
	for (Domain::HOperator Op = 0; Op < theDomain->GettheNumberOfOperators(); ++Op)
	{
		// Get a pointer to the current operator
		const Operator* aCandidate = theDomain->GettheOperator(Op);
		Operator::HPredicate NumberOfPositivePreconditions = aCandidate->GettheNumberOfPositivePreconditions();
		Operator::HArity NumberOfParameters = aCandidate->GettheNumberOfParameters();

		// C indexes all the idenfitiers of a precondition predicate of operator Op together with all their occurences in theStateIndexedByPredicates
		typedef std::vector<PDDL::HPredicate>* info;
		// struct info { std::vector<HPredicate>* p; std::vector<HPredicate>::size_type s;};
#if (STL == MEMORY_ALLOCATION)
		std::vector<info> C(NumberOfPositivePreconditions);
#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
		info* C = (info*) SPHG_ALLOC(sizeof(info) * NumberOfPositivePreconditions);
#elif (HPHA == MEMORY_ALLOCATION)
		info* C = (info*) SPHG_ALLOC(sizeof(info) * NumberOfPositivePreconditions, sizeof(info));
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif

		// Check whether aCandidate is applicable (i.e. is there a precondition which is not a member of theCurrentState?), one precondition predicate at a time
		bool aCandidate_is_applicable = true;	// Is aCandidate applicable to theCurrentState?
		for (Operator::HPredicate pre = 0; pre < NumberOfPositivePreconditions; ++pre)
		{
			// Look for the predicate identifier of precondition pre in theCurrentState
			std::vector<PDDL::HPredicate>* ptr = theStateIndexedByPredicates[theSharedPredicates[aCandidate->GetthePredicate(pre)].GettheIdentifier()];

			if (0 == ptr->size())
			{
				// This (pointer to) precondition predicate identifier has no match in theCurrentState: aCandidate cannot be applied to theCurrentState
				aCandidate_is_applicable = false;
				break;
			}

			// Remember that this predicate identifier of theCurrentState matches the precondition predicate of precondition pre
			C[pre] = ptr;
		}

		//	When aCandidate is not applicable, go to next Op in theDomainOperators
		if (aCandidate_is_applicable)
		{
			// D shall contain a set of predicates from the current situation with the following properties:
			//	- same number of predicate than the NumberOfPositivePreconditions of aCandidate we try to apply to the current situation
			//	- each predicate has an identifier which is the same than that of a precondition predicate of Op
			//	- all predicates are different
#if (STL == MEMORY_ALLOCATION)
			State D(NumberOfPositivePreconditions);
#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
			PDDL::HPredicate* D = (PDDL::HPredicate*) SPHG_ALLOC(sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions);
#elif (HPHA == MEMORY_ALLOCATION)
			PDDL::HPredicate* D = (PDDL::HPredicate*) SPHG_ALLOC(sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions, sizeof(PDDL::HPredicate));
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif

			// We need to extract predicates from the vectors in C.
			// To this end, we use an integer number whose digits are stored in P.
			// Each digit ranges from 0 to the size of a vector in C.
			// We start with the number (0)(0)...(0)(0), which means we point to the first positions of all the vectors in C.
			// Then we increase this number by 1 (we begin increasing the left-most digit)
			// When a digits reaches the size of its corresponding vector in C, we reset it to 0 and we increase the next digit by 1.
			// Everything stops when the last digit reaches the size of its corresponding vector in C; we then try to apply the next Operator.
#if (STL == MEMORY_ALLOCATION)
			State P((PDDL::HPredicate) 0 /* Initial Value for each position in P */, NumberOfPositivePreconditions /* Size of P */);
#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
			PDDL::HPredicate* P = (PDDL::HPredicate*) SPHG_ALLOC(sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions);
			// Begin with pointing to the first positions of the vectors in C
			memset(P, 0, sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions);
#elif (HPHA == MEMORY_ALLOCATION)
			PDDL::HPredicate* P = (PDDL::HPredicate*) SPHG_ALLOC(sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions, sizeof(PDDL::HPredicate));
			// Begin with pointing to the first positions of the vectors in C
			memset(P, 0, sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions);
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif

			Operator::HPredicate i;	// gets initialized to 0 at each cycle in the loop below
			do
			{
				// Build a bijection from the preconditions of aCandidate to predicates from theCurrentState, based on matching theIdentifiers
				// D represents this bijection:
				//	- an index i of D is a precondition number
				//	- D[i] is a predicate in theCurrenTState
				// For the unification process to succeed the following condition must hold:
				//		For all i, j indexes of D, i != j, D[i] != D[j]
				// That is, we're not going to try to unify to distinct preconditions to the same predicate in theCurrentState
				for (Operator::HPredicate p = 0; p < NumberOfPositivePreconditions; ++p)
				{
					// Get a predicate with identifier C[p] from the current situation
					PDDL::HPredicate d = C[p]->operator [](P[p]);

					// Check whether this predicate is already in D
					for (Operator::HPredicate j = 0; j < p; ++j)
						if (D[j] == d)
							// IF a predicate is chosen twice THEN go to next set of predicates from theCurrentState
/********* GOTO *********/	goto NEXT_STATE_PREDICATES;

					// Remember predicate d when it is different from all other predicates already in D
					D[p] = d;
				}

				{
				// All predicates in D are different; try to unify the preconditions of the Operator Op with the predicates in D

					// Unify the preconditions of Op with the predicates in D
#if (STL == MEMORY_ALLOCATION)
					Operator::Parameters theParametersOfaCandidate(aCandidate->GettheParameters());
					if (UnifyPreconditions(aCandidate, D, theParametersOfaCandidate, theStateIndexedByPredicates))
#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
					PDDL::Parameters theParametersOfaCandidate = (PDDL::Parameters) SPHG_ALLOC(NumberOfParameters * sizeof(PDDL::Parameter));
					aCandidate->CopytheParametersInto(theParametersOfaCandidate);
					if (UnifyPreconditions(aCandidate, D, theParametersOfaCandidate, theStateIndexedByPredicates))
#elif (HPHA == MEMORY_ALLOCATION)
					PDDL::Parameters theParametersOfaCandidate = (PDDL::Parameters) SPHG_ALLOC(NumberOfParameters * sizeof(PDDL::Parameter), sizeof(PDDL::Parameter));
					aCandidate->CopytheParametersInto(theParametersOfaCandidate);
					if (UnifyPreconditions(aCandidate, D, theParametersOfaCandidate, theStateIndexedByPredicates))
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif

					{
						State aState = StateSpaceForwardChaining::Apply(aCurrentState, aCandidate, D, theParametersOfaCandidate);

						// Make a child plan when the resulting state is new
						if (theStates.end() == theStates.find(aState))
						{
							// Put this new state into theStates data store
							States::iterator new_state = theStates.insert(std::pair<State, Plan::length_type>(aState, aPlan.Length() + 1)).first;

							// Remember this child
							Operator::HOperatorSignatures anOS( *(aPlan.GettheOperatorSignatures()) );
							Operator::OperatorSignature tempsOS = aCandidate->MaketheOperatorSignature(theParametersOfaCandidate);
							anOS.push_back(AddOperatorSignature(tempsOS));

#if defined(_DEBUG)
							Plan theCurrentChild = Plan(new_state, anOS);

							std::cout << std::endl << "The current state is {";
							for (TemporaryState::size_type i = 0; i < (aPlan.GettheCurrentState())->first.size(); ++i)
							{
								Predicate aPredicate = theSharedPredicates[((aPlan.GettheCurrentState()->first)[i])];
								std::cout << theIdentifiers[aPredicate.GettheIdentifier()] << "(";
								for (Predicate::HArity a = 0; a < aPredicate.GettheArity(); ++a)
								{
									std::cout << theIdentifiers[aPredicate.GettheParameter(a)];
									if (a < (aPredicate.GettheArity() - 1))
										 std::cout << ",";
								}

								if (i < ((aPlan.GettheCurrentState())->first.size() - 1))
									std::cout << "), ";
							}
							std::cout << ")}." << std::endl 
								<< "The current cost is " << (aCost) << std::endl 
								<< "   Applying operator ";

							Operator::OperatorSignature currentOS = theSharedOperatorSignatures[(theCurrentChild.GettheOperatorSignatures())->back()];
							std::cout << theIdentifiers[currentOS[currentOS.size() - 1]] << "(";
							for (Operator::HArity a = 0; a < (currentOS.size() - 1); ++a)
								if (a == (currentOS.size() - 2))
									std::cout << theIdentifiers[currentOS[a]];
								else
									std::cout << theIdentifiers[currentOS[a]] << ",";

							// Write a ')' at the end of the list of parameters but also when the theOperatorSignature[i].second.size() is 0 (zero)
							std::cout << ")" << std::endl;

							std::cout << "The resulting state is {";
							for (size_t i = 0; i < (theCurrentChild.GettheCurrentState())->first.size(); ++i)
							{
								Predicate aPredicate = theSharedPredicates[((theCurrentChild.GettheCurrentState()->first)[i])];
								std::cout << theIdentifiers[aPredicate.GettheIdentifier()] << "(";
								for (Predicate::HArity a = 0; a < aPredicate.GettheArity(); ++a)
								{
									std::cout << theIdentifiers[aPredicate.GettheParameter(a)];
									if (a < (aPredicate.GettheArity() - 1))
										 std::cout << ",";
								}

								if (i < ((theCurrentChild.GettheCurrentState())->first.size() - 1))
									std::cout << "), ";
							}
							std::cout << ")}." << std::endl;
							std::cout << "The resulting cost is " << (1 + aCost + theDomain->GetNumericCost(Op)) << std::endl;
#endif	// defined(_DEBUG)
							// Report immediately when this child is a solution
							if (Included(aState, theFinalState->first))
							{
								thePlanFound = Plan(new_state, anOS);
								theCostOfthePlanFound = 1 + aCost + theDomain->GetNumericCost(Op);	// 1 operator has been added: Add 1 to the cost of this plan

#if (STL == MEMORY_ALLOCATION)
								// Nothing to do here: the STL takes care of the memory management for its containers
#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
								SPHG_FREE(theParametersOfaCandidate);
								SPHG_FREE(D);
								SPHG_FREE(P);
								SPHG_FREE(C);
#elif (HPHA == MEMORY_ALLOCATION)
								SPHG_FREE((PDDL::Parameters) theParametersOfaCandidate, sizeof(PDDL::Parameter) * NumberOfParameters, sizeof(PDDL::Parameter));
								SPHG_FREE((PDDL::HPredicate*) D, sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions, sizeof(PDDL::HPredicate));
								SPHG_FREE((PDDL::HPredicate*) P, sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions, sizeof(PDDL::HPredicate));
								SPHG_FREE((info*) C, sizeof(info) * NumberOfPositivePreconditions, sizeof(info));
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif

								// Report this child is a solution
								return true;
							}

							// Remember this child plan with its cost in the priority queue
							theOpenWithCosts.push(PlanWithCost(1 + aCost + theDomain->GetNumericCost(Op), Plan(new_state, anOS)));	// 1 operator has been added: Add 1 to the cost of this plan
						}
					}

#if (STL == MEMORY_ALLOCATION)
				// Nothing to do here: the STL takes care of the memory management for its containers
#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
				SPHG_FREE(theParametersOfaCandidate);
#elif (HPHA == MEMORY_ALLOCATION)
				SPHG_FREE((PDDL::Parameters) theParametersOfaCandidate, sizeof(PDDL::Parameter) * NumberOfParameters, sizeof(PDDL::Parameter));
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif
				}

/*** LABEL ***/	NEXT_STATE_PREDICATES:
				// Increase the number in P, possibly re-setting a digit to 0 when the increase reaches the size of a vector in C
				i = 0;	// Start with precondition number 0
				while ((i < NumberOfPositivePreconditions) && ((++P[i]) == C[i]->size()))
				{
					P[i] = 0;
					++i;
				}
			}
			// When the NumberOfPositivePreconditions has been reached, it means that all possible combinations of predicates
			// from theCurrentState have been generated and tested against the positive preconditions of Op; so it's time to break this loop
			while (i < NumberOfPositivePreconditions);

#if (STL == MEMORY_ALLOCATION)
			// Nothing to do here: the STL takes care of the memory management for its containers
#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
			SPHG_FREE(D);
			SPHG_FREE(P);
#elif (HPHA == MEMORY_ALLOCATION)
			SPHG_FREE((PDDL::HPredicate*) D, sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions, sizeof(PDDL::HPredicate));
			SPHG_FREE((PDDL::HPredicate*) P, sizeof(PDDL::HPredicate) * NumberOfPositivePreconditions, sizeof(PDDL::HPredicate));
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif
		}

#if (STL == MEMORY_ALLOCATION)
		// Nothing to do here: the STL takes care of the memory management for its containers
#elif ((DL_MALLOC == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION))
		SPHG_FREE(C);
#elif (HPHA == MEMORY_ALLOCATION)
		SPHG_FREE((info*) C, sizeof(info) * NumberOfPositivePreconditions, sizeof(info));
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif
	}

	// Report that no solution has been found during this expansion; maybe next time...
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////// StateSpaceForwardChaining::BestFirst/0
//
bool StateSpaceForwardChaining::BestFirst() {

	// Check whether theFinalState is included in theInitialState
	if (Included(theInitialState->first, theFinalState->first))
	{
		// Memorize this empty solution for external access
		thePlanFound = Plan(theInitialState);
		theCostOfthePlanFound = 0.0f;

		// Tell the user you did a really good job!
		return true;
	}
	else // TheFinalState IS NOT INCLUDED in the InitialState: this search is really starting now...
	{
		// Prepare for the ride on the Planning Frontier
		theOpenWithCosts.push(PlanWithCost(0.0f, Plan(theInitialState)));

		// ... and go for it!
		while (! theOpenWithCosts.empty())
		{// StateSpaceForwardChaining as long as there are Plans to expand

			// Compute the children of the lowest cost plan appearing in theOpenWithCosts
			if (CostExpand())
			{
				// CostExpand/0 memorized the solution child into thePlanFound and its cost into theCostOfthePlanFound

				// No need for heavy memory any longer
				while (! theOpenWithCosts.empty()) theOpenWithCosts.pop();

				// Tell the user you did a good job!
				return true;
			}
		}

		// Tell the user the search space became empty before a solution was found
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////// StateSpaceForwardChaining::Put/4
//
std::ostream& StateSpaceForwardChaining::Put(std::ostream& o, const Plan* aPlan, const std::string& tab, const IterationNumber aCurrentIteration) const {
	if (aCurrentIteration > 0)
	{
		o << "(Plan #" << aCurrentIteration << "):" << std::endl;
	}
	else
		o << "Solution plan found:" << std::endl;

	// Output the actions in aPlan
	if (0 == aPlan->Length())
	{
		o << tab << "Plan is empty." << std::endl;
	}
	else // (0 == aPlan->Length()) but we really mean (0 < aPlan->Length())
	{
		const Operator::HOperatorSignatures* theOperatorSignatures = aPlan->GettheOperatorSignatures();
		for (Operator::HOperatorSignatures::size_type i = 0; i < aPlan->Length(); ++i)
		{
			// Compute a pretty printing tab so that actions are aligned on the same column
			std::string spaces(tab);
			for(unsigned char ws = 0; ws < (unsigned char)(floor(log10((float) aPlan->Length())) - floor(log10((float) (i + 1)))); ++ws)
				spaces += " ";

			// 1 action per line:
			//		action_format	::=	action_identifier "(" parameter-list ")"
			//		parameter-list	::=	parameter_identifier [ "," parameter-list ]
			Operator::OperatorSignature OS = theSharedOperatorSignatures[(*theOperatorSignatures)[i]];
			o << spaces << (i + 1) << ": " << theIdentifiers[OS[OS.size() - 1]] << "(";
			for (Operator::HArity a = 0; a < (OS.size() - 1); ++a)
				if (a == (OS.size() - 2))
					o << theIdentifiers[OS[a]];
				else
					o << theIdentifiers[OS[a]] << ",";

			// Write a ')' at the end of the list of parameters but also when the theOperatorSignatures[i].second.size() is 0 (zero)
			o << ")" << std::endl;
		}
	}

	// Output the state after the application of the last action in aPlan
	o << "The resulting state is {";
	for (PDDL::HPredicate i = 0; i < (aPlan->GettheCurrentState())->first.size(); ++i)
	{
		Predicate aPredicate = theSharedPredicates[((aPlan->GettheCurrentState()->first)[i])];
		o << theIdentifiers[aPredicate.GettheIdentifier()] << "(";
		for (Predicate::HArity a = 0; a < aPredicate.GettheArity(); ++a)
		{
			o << theIdentifiers[aPredicate.GettheParameter(a)];
			if (a < (aPredicate.GettheArity() - 1))
				 o << ",";
		}

		if (i < ((aPlan->GettheCurrentState())->first.size() - 1))
			o << "), ";
	}
	o << ")}." << std::endl;

	return o;
}

/////////////////////////////////////////////////////////////////////////////////////////////////// StateSpaceForwardChaining::SizeOf/0
//
size_t StateSpaceForwardChaining::SizeOf() {
	// Compute the size of the memory for this StateSpaceForwardChaining search

	size_t s_id = sizeof(theIdentifiers);
	for (std::vector<PDDL::Identifier>::size_type id = 0; id < theIdentifiers.size(); ++id)
		s_id += sizeof(theIdentifiers[id]);

	size_t s_sp1 = sizeof(theSharedPredicates);
	for (PDDL::HPredicate sp = 0; sp < theSharedPredicates.size(); ++sp)
		s_sp1 += theSharedPredicates[sp].SizeOf();

	size_t s_sp2 = sizeof(theStatesPredicates);
	for (StatePredicates::iterator it = theStatesPredicates.begin(); it != theStatesPredicates.end(); ++it)
	{
		Predicate p = it->first;
		s_sp2 += p.SizeOf() + sizeof(it->second);
	}

	size_t s_sos = sizeof(theSharedOperatorSignatures);
	for (Operator::OperatorSignatures::size_type os1 = 0; os1 < theSharedOperatorSignatures.size(); ++os1)
	{
		s_sos += sizeof(theSharedOperatorSignatures[os1]);
		for (size_t os2 = 0; os2 < theSharedOperatorSignatures[os1].size(); ++os2)
		{
			s_sos += sizeof(theSharedOperatorSignatures[os1][os2]);
		}
	}

	size_t s_sip = sizeof(theStateIndexedByPredicates);
	for (IndexedState::size_type ip1 = 0; ip1 < theStateIndexedByPredicates.size(); ++ip1)
	{
		s_sip += sizeof(theStateIndexedByPredicates[ip1]);
		// We can't add the size of each indexed state because each search node expansion modify this size
	}

	size_t s_s = sizeof(theStates);
	for (States::iterator it = theStates.begin(); it != theStates.end(); ++it)
	{
		s_s += sizeof((*it));
		for (TemporaryState::size_type i = 0; i < (it->first).size(); ++i)
			s_s += sizeof(it->first[i]);
	}

	std::cout	<< std::endl
				<< "Memory consumption for this state space forward chaining search:" << std::endl
				<< "   - " << theIdentifiers.size() << " identifiers for " << s_id << " bytes." << std::endl
				<< "   - " << theSharedPredicates.size() << " predicates for " << s_sp1 << " bytes." << std::endl
				<< "   - " << theStates.size() << " states for " << s_s << " bytes." << std::endl
				<< "   - " << theSharedOperatorSignatures.size() << " operator signatures for " << s_sos << " bytes." << std::endl;

	return (+ sizeof(theDomain)
			+ sizeof(theProblem)
			+ sizeof(theSizeOftheSharedPredicates)
			+ sizeof(theInitialState)
			+ sizeof(theFinalState)
			+ thePlanFound.SizeOf()
			+ sizeof(theCostOfthePlanFound)
			+ s_open_Max	// == 0 when (memory_consumption_is_measured == false)
			+ s_id
			+ s_sp1
			+ s_sp2
			+ s_sos
			+ s_sip
			+ s_s
);
}

/////////////////////////////////////////////////////////////////////////////////////////////////// StateSpaceForwardChaining::ConvertthePlanFound/0
//
EXTERNAL_TYPES::op_str* StateSpaceForwardChaining::ConvertthePlanFound() const {

	// The linked list which shall contain thePlanFound
	EXTERNAL_TYPES::op_str* PlanFound = new EXTERNAL_TYPES::op_str;	// op_str default constructor sets arity to 0 (== the linked list contains a solution)

	// Begin with a fake element which contains the plan length, i.e. the number of elements in the linked list
	PlanFound->arg_name[0] = (char*) thePlanFound.Length();	// Length/0 return a size_t; hopefully, a plan length should be strictly less than 128

	EXTERNAL_TYPES::op_str* p = PlanFound;
	const Operator::HOperatorSignatures* theOperatorSignatures = thePlanFound.GettheOperatorSignatures();
	for (Plan::length_type i = 0; i < thePlanFound.Length(); ++i)
	{
		// Get some heap space for the next operator in the plan and point to this new store
		p->op_next = new EXTERNAL_TYPES::op_str;
		p = p->op_next;

		// Get the internal structure (the OperatorSignature) and convert it to the op_str* linked list
		Operator::OperatorSignature OS = theSharedOperatorSignatures[(*theOperatorSignatures)[i]];
		p->arity = (int) (1 + OS.size());	// theIdentifier of the Operator PLUS theParameters of the Operator

		// Put the Operator identifier in arg_name[0]
		// First, get some heap space for the string of characters PLUS 1 space for marking the end of the string
		PDDL::HIdentifier action_identifier = OS[OS.size() - 1];
		p->arg_name[0] = (char*) malloc(1 + theIdentifiers[action_identifier].size());
		for (unsigned char c = 0; c < theIdentifiers[action_identifier].size(); ++c)
			p->arg_name[0][c] = theIdentifiers[action_identifier].c_str()[c];
		// Mark the end of the string of characters
		p->arg_name[0][theIdentifiers[action_identifier].size()] = '\0';

		// Put the Operator parameters from arg_name[1] to arg_name[arity - 1]
		for (Operator::HArity a = 0; (a < MAX_ARGS_OP && a < p->arity); ++a)
		{
			// First, get space for the string of characters PLUS 1 space for marking the end of the string
			p->arg_name[a] = (char*) malloc(1 + theIdentifiers[OS[a]].size());
			for (unsigned char c = 0; c < theIdentifiers[OS[a]].size(); ++c)
				p->arg_name[a][c] = theIdentifiers[OS[a]].c_str()[c];
			// Mark the end of the string of characters
			p->arg_name[a][theIdentifiers[OS[a]].size()] = '\0';
		}
	}

	return PlanFound;
}

/////////////////////////////////////////////////////////////////////////////////////////////////// StateSpaceForwardChaining::ConvertthePlanFound/1
//
void StateSpaceForwardChaining::ConvertthePlanFound(EXTERNAL_TYPES::plan& aPlan) const {

	const Operator::HOperatorSignatures* theOperatorSignatures = thePlanFound.GettheOperatorSignatures();
	for (Plan::length_type i = 0; i < thePlanFound.Length(); ++i)
	{
		Operator::OperatorSignature OS = theSharedOperatorSignatures[(*theOperatorSignatures)[i]];

		EXTERNAL_TYPES::op op_i;
		op_i.first = theIdentifiers[OS[OS.size() - 1]];

		for (Operator::HArity a = 0; a < (OS.size() - 1); ++a)
			op_i.second.push_back(theIdentifiers[OS[a]]);

		aPlan.push_back(op_i);
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////// StateSpaceForwardChaining::ConvertthePlanFoundToString/0
//
std::string StateSpaceForwardChaining::ConvertthePlanFoundToString() const {
	std::string buffer = "";

	const Operator::HOperatorSignatures* theOperatorSignatures = thePlanFound.GettheOperatorSignatures();
	for (Plan::length_type i = 0; i < thePlanFound.Length(); ++i)
	{
		Operator::OperatorSignature OS = theSharedOperatorSignatures[(*theOperatorSignatures)[i]];

		buffer += theIdentifiers[OS[OS.size()]];
		buffer += "(";

		for (Operator::HArity a = 0; a < (OS.size() - 1); ++a)
		{
			buffer += theIdentifiers[OS[a]];
			if (a < (OS.size() - 2))
				buffer += ",";
			else // (a >= (OS.size() - 2))
			{
				if (i < (thePlanFound.Length() - 1))
					buffer += ");";
				else // (i == (thePlanFound.Length() - 1)
					buffer += ").";
			}
		}
	}

	return buffer;
}

// ================================================================================================ End of file "StateSpaceForwardChaining.cpp"