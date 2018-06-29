// ================================================================================================ Beginning of file "Predicate.cpp"
// Copyright (c) 2012-2018, Eric Jacopin, ejacopin@ymail.com
/////////////////////////////////////////////////////////////////////////////////////////////////// File Content
//
//					  Unify/2 --- Simple unification (unsigned integer/string based) of this predicate with aPredicate
//					  Match/2 --- Simple pattern matching (unsigned integer/string based) of this predicate with aPredicate
//		UpdatetheParameters/1 --- set theParameters of this Predicate to values from the parameters of an Operator (after Unify/2 has succeeded)
//					 SizeOf/0 --- Memory size for this predicate
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////// Inclusion of files
//
#ifndef _SPHG_PLANNER_PREDICATE_H
	#include "Predicate.h"						// Class visibility
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////// Unify/2
//
// When calling this function, it is assumed that:
//	- the identifier of this Predicate AND of aPredicate are the same (verified in StateSpaceForwardChaining::Expand/2)
//	- ALL the parameters of this Predicate points to theParameters of anOperator
//	- ALL the parameters of aPredicate are valued
//
// So here we only check whether we can unify the parameters
//
bool Predicate::Unify(Predicate& aPredicate, PDDL::Parameters P) {

#if (STL == MEMORY_ALLOCATION)
	for (Predicate::HArity a = 0; a < theArity; ++a)
	{
		PDDL::SORT theSORT_OfParameter_a = P[Predicate::theParameters[a]].first;
		if (PDDL::STRING_VALUE == theSORT_OfParameter_a)
		{
			// Check whether both values are the same
			if (P[Predicate::theParameters[a]].second != aPredicate.GettheParameter(a))
				return false;
		}
		else if (PDDL::VARIABLE == theSORT_OfParameter_a)
		{
			// Mutate this parameter from a VARIABLE into a STRING_VALUE
			P[Predicate::theParameters[a]] = PDDL::Parameter(PDDL::STRING_VALUE, aPredicate.GettheParameter(a));
		}
	}

	// STRING_VALUEs were the same AND VARIABLEs were successfully mutated
	return true;
#elif (DL_MALLOC == MEMORY_ALLOCATION) || (HPHA == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION)
	for (HArity a = 0; a < theArity; ++a)
	{
		Operator::HArity Parameter_a = Predicate::theParameters[a];
		PDDL::SORT theSORT_OfParameter_a = P[Parameter_a].first;
		if (PDDL::STRING_VALUE == theSORT_OfParameter_a)
		{
			// Check whether both values are the same
			if (P[Parameter_a].second != aPredicate.GettheParameter(a))
				return false;
		}
		else // Currently, there are only 2 SORTs so reaching this point is because (VARIABLE == theSORT_OfParameter_a)
		{
			// Mutate this parameter from a VARIABLE into a STRING_VALUE and set its value
			P[Parameter_a].first = PDDL::STRING_VALUE;
			P[Parameter_a].second = aPredicate.GettheParameter(a);
		}
	}

	// STRING_VALUEs were the same AND VARIABLEs were successfully mutated
	return true;
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////// Match/2
bool Predicate::Match(Predicate& aPredicate, PDDL::Parameters P) {

#if (STL == MEMORY_ALLOCATION)
	for (Predicate::HArity a = 0; a < theArity; ++a)
	{
		// Check whether both values are the same
		if (P[Predicate::theParameters[a]].second != aPredicate.GettheParameter(a))
			return false;
	}

	// STRING_VALUEs were the same AND VARIABLEs were successfully mutated
	return true;
#elif (DL_MALLOC == MEMORY_ALLOCATION) || (HPHA == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION)
	for (HArity a = 0; a < theArity; ++a)
	{
		// Check whether both values are the same
		if (P[Predicate::theParameters[a]].second != aPredicate.GettheParameter(a))
			return false;
	}

	// STRING_VALUEs were the same AND VARIABLEs were successfully mutated
	return true;
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
#error MEMORY_ALLOCATION
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////// UpdatetheParameters/1
//
Predicate& Predicate::UpdatetheParameters(PDDL::Parameters OperatorParameters) {
		// Make parameters pointing to positions in theIdentifiers when unification has succeeded
		for (Predicate::HArity i = 0; i < theArity; ++i)
			theParameters[i] = OperatorParameters[theParameters[i]].second;

		return (*this);
}

/////////////////////////////////////////////////////////////////////////////////////////////////// SizeOf/0
//
size_t Predicate::SizeOf() const {
	size_t s_p = sizeof(theParameters);
	for (Predicate::HArity p = 0; p < theArity; ++p)
		s_p += sizeof(theParameters[p]);

	return (s_p + sizeof(theIdentifier) + sizeof(theArity));
}

// ================================================================================================ End of file "Predicate.cpp"