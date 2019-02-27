// ================================================================================================ Beginning of file ProblemFile.h
// Copyright (c) 2012-2019, Eric Jacopin, ejacopin@ymail.com
// ------------------------------------------------------------------------------------------------
#ifndef _SPHG_PDDL_PROBLEM_FILE_H
#define _SPHG_PDDL_PROBLEM_FILE_H

#if defined(_MSC_VER)	// _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#pragma once		// When compiling, this file will be included (opened) only once by the Microsoft C++ compiler 
#endif					// _MSC_VER

// ------------------------------------------------------------------------------------------------ Inclusion of files
// STD and STL files
#include <iostream>		// Error messages on std::cerr and end of reading on std::cout

// Project files
#ifndef _SPHG_PDDL_DOMAIN_H
	#include "Domain.h"		// The problem file must respect the definitions (predicates, ...) of its domain
#endif

#ifndef _SPHG_PDDL_PROBLEM_H
	#include "Problem.h"
#endif

// ------------------------------------------------------------------------------------------------ Class definition
class ProblemFile : public Problem {
	// ----- Properties
	private:
		PDDL::Identifier theProblemFileName;	// the name of the Problem File
		PDDL::Identifier theDomainName;			// the name of the PDDL Domain this PDDL Problem file refers to
		bool reading_of_theProblemFile_failed;	// Initially set to true by the constructor; set to false only when the reading of this PDDL Problem file correctly (i.e. no error) is over
		bool goal_included_in_init;				// Initially set to true by the constructor; set to false only when the :goal if this PDDL Problem is included in the :init


	// ----- Constructors
	public:
		ProblemFile() : theProblemFileName(""), theDomainName(""), reading_of_theProblemFile_failed(true), goal_included_in_init(true) {}

		ProblemFile(const PDDL::Identifier& aProblemFileName,			/* Name of the PDDL Problem file name */
						  PDDL::StoreHouse& aPDDLLocalStore,			/* RAM Store of the PDDL Domain when parsing is over */
						  bool reading_of_theDomainFile_failed = false,	/* A PDDL problem always refers to a Domain; was the Domain successfully built in memory ? */
						  std::ostream& error_stream = std::cerr		/* Where PDDL syntax errors are reported */);	// Building a PDDL Problem Store from a file


	// ----- Accessors
	public:
		PDDL::Identifier GettheProblemFileName() const { return theProblemFileName; }	// What is the name of the file containing this PDLL Problem?
		bool ReadingFailed() const { return reading_of_theProblemFile_failed; }			// Did the reading of this ProblemFile terminate correctly?
		bool SearchUnneeded() const { return goal_included_in_init; }					// Is this ProblemFile declaring a trivial problem? (i.e. the initial state of this problem includes the final state)


	// ----- Operation
	public:
		inline bool Required(PDDL::REQUIREMENTS r) const {						// Does this ProblemFile or its Domain require r? (used when a :requirements-based feature is encountered; was its :requirements r previously declared?)
			// The use of a logical OR in the return statement entails it is only mandatory to declare
			// a requirement in the domain. Is this acceptable/reasonable/...?
			return (Problem::theRequirements[r] || theDomain->GetRequirement(r));
		}

};

#endif	// _SPHG_PDDL_PROBLEM_FILE_H
// ================================================================================================ End of file ProblemFile.h