// ================================================================================================ Beginning of file "Problem.cpp"
// Copyright (c) 2012-2019, Eric Jacopin, ejacopin@ymail.com
/////////////////////////////////////////////////////////////////////////////////////////////////// File Content
//
//			Problem/2 --- Constructs various structures from the domain 
//		AddConstant/1 --- Remember specific constants for this problem
//			 SizeOf/1 --- Reporting on the memory usage for this problem
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////// Inclusion of files
//
#ifndef _SPHG_PDDL_DOMAIN_H
	#include "Domain.h"
#endif

#ifndef _SPHG_PDDL_PROBLEM_H
	#include "Problem.h"	// Class visibility
#endif

#include <iostream>			// Visibility for std::cout, std::endl

/////////////////////////////////////////////////////////////////////////////////////////////////// Problem/2
//
Problem::Problem(const PDDL::Identifier& aName, Domain& aDomain) : theName(aName), theDomain(&aDomain) {

	for (PDDL::HRequirement i = 0; i < PDDL::NUMBER_OF_REQUIREMENTS; ++i)
		Problem::theRequirements[i] = false;

	theCPosition = (PDDL::HIdentifier) ((theDomain->GettheConstants()).size() - 1);
	for (std::map<PDDL::Identifier, PDDL::HIdentifier>::const_iterator i = (theDomain->GettheConstants()).begin(); i != (theDomain->GettheConstants()).end(); ++i)
		theConstants[i->second] = i->first;

	thePPosition = (PDDL::HPredicate) (theDomain->GetthePredicates()).size();
}

/////////////////////////////////////////////////////////////////////////////////////////////////// AddConstant/1
//
PDDL::HIdentifier Problem::AddConstant(const PDDL::Identifier& anIdentifier) {
	std::map<PDDL::Identifier, HIdentifier>::iterator i = (theDomain->GettheConstants()).find(anIdentifier);
	if ((theDomain->GettheConstants()).end() == i)
	{
		theConstants[++theCPosition] = anIdentifier;
		return theCPosition;
	}
	else
		return (i->second);
}

/////////////////////////////////////////////////////////////////////////////////////////////////// SizeOf/1
//
size_t Problem::SizeOf(const PDDL::Identifier& aProblemFilename) {
	// ------------------------------------ Values correspond to Microsoft's Visual C++ for Windows

	// Measure the size of small objects of this problem
	size_t sSO = sizeof(theName) + sizeof(theDomain)
								 + sizeof(thePositiveInitialState) + sizeof(theNegativeInitialState)
								 + sizeof(thePositiveGoalState) + sizeof(theNegativeGoalState)
								 + sizeof(theCPosition) + sizeof(thePPosition);

	// Mesure the size of the searches
	size_t sS = sizeof(theSearches);
	for (std::vector<Search*>::size_type s = 0; s < theSearches.size(); ++s)
		sS += theSearches[s]->SizeOf();

	// Measure the size of this problem store for Objects
	size_t sO = sizeof(theConstants);		// 20 initial bytes
	for (std::map<HIdentifier, PDDL::Identifier>::iterator i = theConstants.begin(); i != theConstants.end(); ++i)
		sO += sizeof((*i));

	// Measure the size of this problem store for Predicates
	size_t sP = sizeof(thePredicates);		// 20 initial bytes
	for (PDDL::HPredicate p = 0; p < thePredicates.size(); ++p)
		sP += thePredicates[p].SizeOf();

	// Report the measures to the standard console
	if ("" == aProblemFilename)
		std::cout << std::endl << "Memory usage for PDDL Problem " << theName << ":" << std::endl;
	else
		std::cout << std::endl << "Memory usage for PDDL Problem file " << aProblemFilename << ":" << std::endl;

	std::cout << "   - " << sSO << " bytes are used to store small objects." << std::endl;
	std::cout << "   - " << theConstants.size() << " objects for " <<  sO << " bytes." << std::endl;
	std::cout << "   - " << thePredicates.size() << " predicates for " << sP << " bytes." << std::endl;
	std::cout << "   - " << theSearches.size() << " searches for " << sS << " bytes." << std::endl;
	std::cout << "Total memory size is " << (sSO + sO + sP + sS) << " bytes." << std::endl;

	return (sO + sP);
}

// ================================================================================================ End of file "Problem.cpp"