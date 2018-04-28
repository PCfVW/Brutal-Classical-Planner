// ================================================================================================ Beginning of file "Operator.cpp"
// Copyright (c) 2012-2018, Eric Jacopin, ejacopin@ymail.com
/////////////////////////////////////////////////////////////////////////////////////////////////// File Content
//
//					  SizeOf/0 --- Computing the memroy size of this operator
//
/////////////////////////////////////////////////////////////////////////////////////////////////// 
//
/////////////////////////////////////////////////////////////////////////////////////////////////// Inclusion of files
//
#ifndef _SPHG_PLANNER_OPERATOR_H
	#include "Operator.h"						// Class visibility
#endif

#ifndef _SPHG_PLANNER_PREDICATE_H
	#include "Predicate.h"
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////// Operator::SizeOf/0
//
size_t Operator::SizeOf() const {
	// Compute the size of the memory overhead for the std::vector which contains theParameters of the operator
	size_t s_par = sizeof(theNumberOfParameters) + sizeof(theParameters);
	// Compute the memory size for theParameters
	for (Operator::HArity p = 0; p < theNumberOfParameters; ++p)
		s_par += sizeof(theParameters[p]);

	// Compute the memory size for thePredicates
	size_t s_pre = sizeof(theNumberOfPredicates) + sizeof(thePredicates);
	for (Operator::HPredicate p = 0; p < theNumberOfPredicates; ++p)
		s_pre += sizeof(thePredicates[p]);

	// Compute the memory side for thePositions
	size_t s_pos = sizeof(thePositions);
	for(std::vector<std::vector<Position>>::size_type p1 = 0; p1 < thePositions.size(); ++p1)
	{
		s_pos += sizeof(thePositions[p1]);
		for(std::vector<Position>::size_type p2 = 0; p2 < thePositions[p1].size(); ++p2)
			s_pos += sizeof(thePositions[p1][p2]);
	}

	return (sizeof(theIdentifier)
				+ s_par + s_pre + s_pos
				+ sizeof(thePositionOfDeletedPreconditions)
				+ sizeof(thePositionOfAdditions) + sizeof(thePositionOfDeletions)
				+ sizeof(thePositionOfAddedPreconditions) + sizeof(thePositionOfNegativePreconditions));
}


// ================================================================================================ End of file "Operator.cpp"