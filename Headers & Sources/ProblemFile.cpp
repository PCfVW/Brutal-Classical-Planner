// ================================================================================================ Beginning of file ProblemFile.cpp
// Copyright (c) 2012-2018, Eric Jacopin, ejacopin@ymail.com
/////////////////////////////////////////////////////////////////////////////////////////////////// File Content
//
//	ProblemFile/4 --- (READ THE WARNING BELOW) Constructs various structures for this problem while reading a PDDL Problem file
//
/////////////////////////////////////////////////////////////////////////////////////////////////// 
//
/////////////////////////////////////////////////////////////////////////////////////////////////// Inclusion of files
//
#if defined(_MSC_VER)			// _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#include <unordered_map>	// Use Microsoft's STL extensions which provides hashed structures
#else
	#include <map>				// Use a classic associative STL container when the compiler is not Microsoft's
#endif							// _MSC_VER

// STD and STL files
#include <sys/stat.h>			// Visibility for stat and _S_IREAD
#include <fstream>				// std::ifstream

// Project files
#ifndef _SPHG_PLANNER_CONDITIONAL_COMPILING_H
	#include "ConditionalCompiling.h"		// Visibility for MEMORY_ALLOCATION, STL, HPHA, CLASSIC
#endif

#ifndef _SPHG_PDDL_PROBLEM_FILE_H
	#include "ProblemFile.h"	// Class visibility
#endif

#ifndef _SPHG_PDDL_FILES_ROUTINES
	#include "FilesRoutines.h"	// Visibility for GetArity/2, GotoNextLeftPar/2, GotoNextRightPar/2, GetNextToken/3 and SkipWhiteSpacesAndLineFeeds/2
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////// ProblemFile/4
//
//	W A R N I N G  ===  W A R N I N G  ===  W A R N I N G  ===  W A R N I N G  ===  W A R N I N G
//
//
//						ProblemFile/4 DOES LITTLE PDDL ERROR RECOVERY
//
//			MAKE YOUR PROBLEMFILE PDDL COMPLIANT BEFORE GIVING IT TO THIS CONSTRUCTOR
//
//
//	W A R N I N G  ===  W A R N I N G  ===  W A R N I N G  ===  W A R N I N G  ===  W A R N I N G
//
// ------------------------------------------------------------------------------------------------
//
// The reading of a PDDL Problem file follows the current PDDL Standard:
//	1. Open the file, get its size, read it and place it into an array of char, eventually close the file
//	2. Get the problem name and the domain
//	3. Read the :requirements and the :objects sections (ALL OTHER PDDL sections ARE IGNORED: :constants, :constraints, :functions and :types)
//	4. Read the initial predicates (initial situation) of the problem
//	5. Read the goal predicates (final situation) of the problem
 //	6. Read the :metric section
//  7. Free the memory allocated for the array of char mapping the problem file
//
ProblemFile::ProblemFile(const PDDL::Identifier& aProblemFileName, PDDL::StoreHouse& aPDDLLocalStore, bool reading_of_theDomainFile_failed, std::ostream& error_stream) :

	goal_included_in_init(true),			// Set to false as soon as one predicate of the :goal is not an :init predicate (we check whether positive/negative predicates)
	reading_of_theProblemFile_failed(true)	// Set to false at the end of this constructor when the reading of aDomainFileName terminates with no error

{	// Let's begin our noble task!

	if (reading_of_theDomainFile_failed)
	{
		// Tell the user the reading of the domain file given by the user for this problem failed
		error_stream << "PDDL Problem " << theProblemFileName
			<< " cannot be read when the reading of its PDDL Domain file failed." << std::endl;
		// Don't go any further; 
		return;
	}

	// Open the PDDL Problem file
	std::ifstream aProblemFile(aProblemFileName);
	if (!aProblemFile)
	{
		// Tell the user about the non-existent problem file
		error_stream << "PDDL Problem file >> " << aProblemFileName << " << cannot be opened!" << std::endl;
		// Don't go any further; 
		return;
	}

	// Place the file into a char buffer:
	//	1. Get the file length
	struct stat buf;
	stat(aProblemFileName.c_str(), &buf);
	long size = buf.st_size;
	//	2. Allocate a buffer with the file length
#if ((STL == MEMORY_ALLOCATION)  || (CLASSIC == MEMORY_ALLOCATION) || (DL_MALLOC == MEMORY_ALLOCATION))
	char* buffer = (char*) SPHG_ALLOC(sizeof(char) * size);
#elif (HPHA == MEMORY_ALLOCATION)
	char* buffer = (char*) SPHG_ALLOC(sizeof(char) * size, sizeof(char));
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif
	//	3. Place all the characters of the file into the buffer
	aProblemFile.read(buffer, size);
	//	4. Close the file
	aProblemFile.close();

	// Now, parse the buffer
	PDDL_Token token;						// TRUE <=> (position >= size)
	short parenthesis_level = 0;			// At most 127 parentheses levels; it's important it's a signed short: there are too many left parenthesis when it becomes negative
	INTERNAL_TYPES::u16 line_number = 1;	// At most 65535 lines in the buffer
	INTERNAL_TYPES::u16 position = 0;		// At most 65536 characters in the buffer (whereas size is a long)
	PDDL::Identifier token_string = "";		// The successive tokens a PDDL file according to the PDDL syntax ("and", ":action", "not", "(", ")", "?", ...) read from aFileName

	// The PDDL Domain file exists, so remember the name of this file associated with this PDDL Problem Store
	theProblemFileName = aProblemFileName;
	bool total_cost_has_no_initial_value = true;
	PDDL::Number InitialNumericCost = 0.0f;

	std::map<PDDL::Identifier, PDDL::HIdentifier> theConstantsLocalStore;

	// ========================================================== Get the problem Name and check the domain it refers to
	if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
	{
		// Tell the user about the missing left parenthesis
		error_stream << theProblemFileName << "(" << line_number << "): Left parenthesis expected."
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}

	token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
	if ("define" != token_string)
	{
		// Tell the user about the missing "define" PDDL keyword
		error_stream << theProblemFileName << "(" << line_number << "): PDDL keyword define is expected."
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}

	if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
	{
		// Tell the user about the missing left parenthesis
		error_stream << theProblemFileName << "(" << line_number << "): Left parenthesis expected."
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}

	token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
	if ("problem" != token_string)
	{
		// Tell the user about the missing "define" PDDL keyword
		error_stream << theProblemFileName << "(" << line_number << "): PDDL keyword problem is expected."
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}
	if (IDENTIFIER != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
	{
		// Tell the user the problem identifier does not follow the PDDL format for identifiers
		error_stream << theProblemFileName << "(" << line_number
			<< "): Problem identifier " << token_string << " is invalid."
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}
	theName = token_string;		// the Domain name

	if (RIGHT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
	{
		// Tell the user about the missing right parenthesis
		error_stream << theProblemFileName << "(" << line_number << "): Right parenthesis expected."
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}
	if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
	{
		// Tell the user about the missing left parenthesis
		error_stream << theProblemFileName << "(" << line_number << "): Left parenthesis expected."
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}
	token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
	if (":domain" != token_string)
	{
		// Tell the user about the missing "domaine" PDDL keyword
		error_stream << theProblemFileName << "(" << line_number << "): PDDL keyword :domain is expected." << std::endl;
		// Don't go any further: user must fix the PDDL problem file first
		return;
	}
	token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
	PDDL::StoreHouse::iterator it = aPDDLLocalStore.find(token_string);
	if (aPDDLLocalStore.end() == it)
	{
		// Tell the user this problem refers to an unknown PDDL domain
		error_stream << theProblemFileName << "(" << line_number << "): PDDL Domain " << token_string << " is unknown." << std::endl;
		// Don't go any further: user must fix the PDDL problem file first
		return;
	}
	theDomain = (it->second);		// the valid domain (i.e. the PDDL Domain) this PDDL Problem refers to

	// Now that the PDDL Domain has been retrieved, it's time to initialize the problem structures
	theCPosition = (PDDL::HIdentifier) ((theDomain->GettheConstants()).size() - 1);
	for (std::map<PDDL::Identifier, PDDL::HIdentifier>::const_iterator i = (theDomain->GettheConstants()).begin(); i != (theDomain->GettheConstants()).end(); ++i)
		theConstants[i->second] = i->first;
	thePPosition = (PDDL::HPredicate) (theDomain->GetthePredicates()).size();

	// Close this part
	if (RIGHT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
	{
		// Tell the user about the missing right parenthesis
		error_stream << theProblemFileName << "(" << line_number << "): Right parenthesis expected."
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}

	// ========================================================== Get the requirements for this problem
	// The :typing requirements is accepted but PDDL types are ignored and consequently,
	// NO checking of any kind (not even spelling) is performed on types.
	if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
	{
		// Tell the user about the left parenthesis
		error_stream << theProblemFileName << "(" << line_number << "): Left parenthesis expected."
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}
	token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);

	if (":requirements" == token_string)
	{
		while (RIGHT_PAR != token)
		{
			token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
			if (PDDL::UnknownRequireKey(token_string))
			{
				// Tell the user about token-string containing an invalid PDDL requirement
				error_stream << theProblemFileName << "(" << line_number
					<< "): " << token_string << " is not a valid PDDL requirement." << std::endl;
				// Don't go any further: user must fix the PDDL problem file first
				return;
			}
			else if (! PDDL::GettheRequirementInfo(token_string).second)
			{
				// Tell the user that token-string refers to an un-implemented PDDL requirement
				error_stream << theProblemFileName << "(" << line_number
					<< "): Features associated with PDDL requirement " << token_string << " are not implemented." << std::endl;
				// Don't go any further: user must fix the PDDL problem file first
				return;
			}

			// Remember a requirement was read in the :requirements section of this PDDL Problem file
			Problem::RegisterRequireKey(token_string);

		}

		if (RIGHT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
		{
			// Tell the user about the missing right parenthesis
			error_stream << theProblemFileName << "(" << line_number << "): Right parenthesis expected."
				<< std::endl;
			// Don't go any further: user must fix the PDDL domain file first
			return;
		}
		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
	}


	// ========================================================== Get the objects for this problem
	if (":objects" == token_string)
	{
		// Read the objects 
		while (RIGHT_PAR != token)
		{
			token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
			if (IDENTIFIER == token)
			{
				std::map<PDDL::Identifier, PDDL::HIdentifier>::iterator it = theConstantsLocalStore.find(token_string);
				if (theConstantsLocalStore.end() != it)
				{
					// Tell the user about the re-declaration of a PDDL object
					error_stream << theProblemFileName << "(" << line_number << "): PDDL Problem object " 
						<< token_string << " is redeclared."
						<< std::endl;
					// Don't go any further: user must fix the PDDL problem file first
					return;
				}
				theConstantsLocalStore[token_string] = AddConstant(token_string);
			}
			else if (DASH == token)
			{
				// Types are ignored!!!
				token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
			}
		}
	}

	std::map<Predicate, std::pair<PDDL::HPredicate, bool> > theInitialStatePredicates;	// bool is true when the initial state predicate is negated

	// ========================================================== Get the initial predicates for this problem
	if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
	{
		// Tell the user about the missing left parenthesis
		error_stream << theProblemFileName << "(" << line_number << "): Left parenthesis expected."
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}
	token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
	if (":init" != token_string)
	{
		// Tell the user about the missing ":init" PDDL keyword
		error_stream << theProblemFileName << "(" << line_number
			<< "): PDDL Keyword :init is expected." << std::endl;
		// Don't go any further: user must fix the PDDL problem file first
		return;
	}
	token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);

	while (RIGHT_PAR != token)
	{
		// ================================================== Get the identifier of the initial state predicate
		if (LEFT_PAR != token)
		{
			// Tell the user about the missing left parenthesis
			error_stream << theProblemFileName << "(" << line_number << "): Left parenthesis expected."
				<< std::endl;
			// Don't go any further: user must fix the PDDL domain file first
			return;
		}
		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);

		if (EQUAL == token)
		{
			// The only current accepted assigment in the :init section of the PDDL problem declaration is (= (total-cost) 0)
			if (! Required(action_costs))
			{
				// Tell the user about the invalid negative :init statement
				error_stream << theProblemFileName << "(" << line_number << "): Invalid :init assignment <=> encountered without :action-costs in the :requirements section."
					<< std::endl;
				// Don't go any further: user must fix the PDDL problem file first
				return;
			}

			// Now, we're waiting for "(total-cost) numeric-value)"
			if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
			{
				// Tell the user about the missing left parenthesis
				error_stream << theProblemFileName << "(" << line_number << "): Left parenthesis expected."
					<< std::endl;
				// Don't go any further: user must fix the PDDL domain file first
				return;
			}
			token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
			if ("total-cost" != token_string)
			{
				// Tell the user about the missing token_string: total-cost
				error_stream << theProblemFileName << "(" << line_number << "): total-cost expected."
					<< std::endl;
				// Don't go any further: user must fix the PDDL domain file first
				return;
			}
			if (RIGHT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
			{
				// Tell the user about the missing right parenthesis
				error_stream << theProblemFileName << "(" << line_number << "): Right parenthesis expected."
					<< std::endl;
				// Don't go any further: user must fix the PDDL domain file first
				return;
			}
			if (NUMBER != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
			{
				// Tell the user about the invalid assignment value
				error_stream << theProblemFileName << "(" << line_number
					<< "): The second parameter of this assignment must be a positive number." 
					<< std::endl;
				// Don't go any further: user must fix the PDDL domain file first
				return;
			}
			ValidPDDLNumber(token_string, InitialNumericCost);

			if (RIGHT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
			{
				// Tell the user about the missing right parenthesis
				error_stream << theProblemFileName << "(" << line_number
					<< "): Right parenthesis expected after numeric cost." 
					<< std::endl;
				// Don't go any further: user must fix the PDDL domain file first
				return;
			}
			// Remember we just read a valid assignment for total-cost
			total_cost_has_no_initial_value = false;

			// Initiate the next round
			token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
		}
		else
		{
			bool negative_init_predicate = ("not" == token_string);
			if (negative_init_predicate)
			{
				if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
				{
					// Tell the user about the missing left parenthesis
					error_stream << theProblemFileName << "(" << line_number << "): Left parenthesis expected."
						<< std::endl;
					// Don't go any further: user must fix the PDDL domain file first
					return;
				}
				token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);

				if (! Required(negative_preconditions))
				{
					// Tell the user about the invalid negative :init predicate
					error_stream << theProblemFileName << "(" << line_number << "): Negative :init predicate " << token_string
						<< " encountered without :negative-preconditions in the :requirements section."
						<< std::endl;
					// Don't go any further: user must fix the PDDL problem file first
					return;
				}
			}
				
			// This token_string MUST match an Identifier declared in the :predicates section of the PDDL domain
			std::pair<Predicate::PredicatesStore::iterator, bool> r = theDomain->Find(token_string);
			if (!(r.second))
			{
				// Tell the user about the invalid predicate identifier
				error_stream << theProblemFileName << "(" << line_number << "): Invalid :init predicate "
					<< token_string << " in problem " << theName
					<< "." << std::endl;
				// Don't go any further: user must fix the PDDL problem file first
				return;
			}
			PDDL::Identifier thePredicateIdentifier = token_string;
			if (MAXIMUM_NUMBER_OF_PREDICATES < thePredicates.size())
			{
				// Tell the user the problem file declares more predicates than he allowed with MAXIMUM_NUMBER_OF_PREDICATES
				error_stream << theProblemFileName << "(" << line_number << "): Too many predicates(" << thePredicates.size()
					<< ") declared in the :init section."
					<< std::endl;
				// Don't go any further: user must fix the PDDL problem file first
				return;
			}

			// Remember this predicate in thePredicates
			PDDL::HPredicate p;
			if (negative_init_predicate)
				p = Problem::AddInitialNegativePredicate(Predicate((r.first)->second.first, (r.first)->second.second));
			else
				p = Problem::AddInitialPositivePredicate(Predicate((r.first)->second.first, (r.first)->second.second));

			Predicate::HArity a_current = (r.first)->second.second;

			token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);

			while (RIGHT_PAR != token)
			{
				// ================================================== Get the predicate (constant) parameters
				if (0 == a_current)
				{
					// Tell the user about the invalid predicate identifier
					error_stream << theProblemFileName << "(" << line_number << "): Too many parameters in :init predicate "
						<< thePredicateIdentifier << "/" << (INTERNAL_TYPES::u16) Predicate::HArity((r.first)->second.second) << "."
						<< std::endl;
					// Don't go any further: user must fix the PDDL problem file first
					return;
				}	

				// This token_string MUST match an Identifier declared in the :objects section of the PDDL problem
				if (IDENTIFIER != token)
				{
					// Tell the user the problem identifier does not follow the PDDL format for identifiers
					error_stream << theProblemFileName << "(" << line_number << "): :init predicate identifier "
						<< token_string << " is invalid."
						<< std::endl;
					// Don't go any further: user must fix the PDDL domain file first
					return;
				}

				std::map<PDDL::Identifier, PDDL::HIdentifier>::iterator it = theConstantsLocalStore.find(token_string);
				if (theConstantsLocalStore.end() == it)
				{
					// Tell the user that token-string contains an un-declared PDDL object
					error_stream << theProblemFileName << "(" << line_number << "): PDDL Problem object " 
						<< token_string << " is unknown."
						<< std::endl;
					// Don't go any further: user must fix the PDDL problem file first
					return;
				}

				// It should be time to check the type of the object, but types ARE NOT YET IMPLEMENTED

				Problem::AddPredicateParameter(p, ((r.first)->second.second) - (a_current--), (it->second));

				// Position yourself on the next parameter
				token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
			}

			if (0 != a_current)
			{
				// Tell the user about the invalid predicate 
				error_stream << theProblemFileName << "(" << line_number << "): Not enough parameters in :init predicate "
						<< thePredicateIdentifier
						<< "/" << (INTERNAL_TYPES::u16) Predicate::HArity((r.first)->second.second)
						<< "." << std::endl;
				// Don't go any further: user must fix the PDDL problem file first
				return;
			}

			// Remember the location of this predicate in thePredicates, in case we read it again in the :goal
			theInitialStatePredicates[thePredicates[p]] = std::pair<PDDL::HPredicate, bool>(p, negative_init_predicate);

			token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
		}
	}

	if (Required(action_costs) && total_cost_has_no_initial_value)
	{
		// Tell the user he forget to assign an initial value to "total-cost"
		error_stream << theProblemFileName << "(" << line_number
			<< "): :action-costs is required but total-cost is never assigned in the :init section." 
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}

	// ========================================================== Get the goal predicates for this problem
	if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
	{
		// Tell the user about the missing left parenthesis
		error_stream << theProblemFileName << "(" << line_number << "): Left parenthesis expected."
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}
	token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
	if (":goal" != token_string)
	{
		// Tell the user about the missing ":goal" PDDL keyword
		error_stream << theProblemFileName << "(" << line_number << "): PDDL Keyword :goal is expected."
			<< std::endl;
		// Don't go any further: user must fix the PDDL problem file first
		return;
	}
	if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
	{
		// Tell the user about the missing left parenthesis
		error_stream << theProblemFileName << "(" << line_number << "): Left parenthesis expected."
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}
	token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
	bool And = ("and" == token_string);

	while (RIGHT_PAR != token)
	{
		// ================================================== Get the identifier of the goal state predicate
		if (And)
		{
			if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
			{
				// Tell the user about the missing left parenthesis
				error_stream << theProblemFileName << "(" << line_number << "): Left parenthesis expected."
					<< std::endl;
				// Don't go any further: user must fix the PDDL domain file first
				return;
			}
			token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
		}

		bool negative_goal_predicate = ("not" == token_string);
		if (negative_goal_predicate)
		{
			if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
			{
				// Tell the user about the missing left parenthesis
				error_stream << theProblemFileName << "(" << line_number << "): Left parenthesis expected."
					<< std::endl;
				// Don't go any further: user must fix the PDDL domain file first
				return;
			}
			token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);

			if (! Required(negative_preconditions))
			{
				// Tell the user about the invalid negative :goal predicate
				error_stream << theProblemFileName << "(" << line_number
					<< "): Negative :goal predicate " << token_string
					<< " encountered without :negative-preconditions in the :requirements section."
					<< std::endl;
				// Don't go any further: user must fix the PDDL problem file first
				return;
			}
		}

		// This token_string MUST match an Identifier declared in the :predicates section of the PDDL domain
		std::pair<Predicate::PredicatesStore::iterator, bool> r = theDomain->Find(token_string);

		if (!(r.second))
		{
			// Tell the user about the invalid predicate identifier
			error_stream << theProblemFileName << "(" << line_number << "): Invalid :goal predicate "
				<< token_string << " in problem " << theName << "." 
				<< std::endl;
			// Don't go any further: user must fix the PDDL problem file first
			return;

		}
		PDDL::Identifier thePredicateIdentifier = token_string;
		if (MAXIMUM_NUMBER_OF_PREDICATES < thePredicates.size())
		{
			// Tell the user the problem file declares more predicates than he allowed with MAXIMUM_NUMBER_OF_PREDICATES
			error_stream << theProblemFileName << "(" << line_number << "): Too many predicates("
				<< thePredicates.size() << ") declared in the :goal section."
				<< std::endl;
			// Don't go any further: user must fix the PDDL problem file first
			return;
		}

		// Remember this predicate in thePredicates
		PDDL::HPredicate p;
		Predicate aTemporaryPredicate((r.first)->second.first, (r.first)->second.second);

		//if (negative_goal_predicate)
		//	p = Problem::AddGoalNegativePredicate(Predicate((r.first)->second.first, (r.first)->second.second));
		//else
		//	p = Problem::AddGoalPositivePredicate(Predicate((r.first)->second.first, (r.first)->second.second));

		Predicate::HArity a_current = (r.first)->second.second;
		while (RIGHT_PAR != token)
		{
			// ================================================== Get the predicate (constant) parameters
			token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
			if (IDENTIFIER == token)
			{
				if (0 == a_current)
				{
					// Tell the user about the invalid predicate identifier
					error_stream << theProblemFileName << "(" << line_number << "): Too many parameters in :goal predicate "
						<< thePredicateIdentifier << "/" << (INTERNAL_TYPES::u16) Predicate::HArity((r.first)->second.second) << "." 
						<< std::endl;
					// Don't go any further: user must fix the PDDL problem file first
					return;
				}
				// This token_string MUST match an Identifier declared in the :objects section of the PDDL problem
				std::map<PDDL::Identifier, PDDL::HIdentifier>::iterator it = theConstantsLocalStore.find(token_string);
				if (theConstantsLocalStore.end() == it)
				{
					// Tell the user that token-string contains an un-declared PDDL object
					error_stream << theProblemFileName << "(" << line_number << "): PDDL Problem object " 
						<< token_string << " is unknown."
						<< std::endl;
					// Don't go any further: user must fix the PDDL problem file first
					return;
				}
				// the type of the object should be checked here, but types ARE NOT YET IMPLEMENTED
				aTemporaryPredicate.AddParameter(((r.first)->second.second) - (a_current--), (it->second));
			}
		}

		if (0 != a_current)
		{
			// Tell the user about the wrong number of parameters in this :goal predicate
			error_stream << theProblemFileName << "(" << line_number << "): Not enough parameters in :goal predicate "
				<< thePredicateIdentifier << "/" << (INTERNAL_TYPES::u16) Predicate::HArity((r.first)->second.second) << "."
				<< std::endl;
			// Don't go any further: user must fix the PDDL problem file first
			return;
		}

		// Remember aTemporaryPredicate and check whether the :goal in included in the :init
		std::map<Predicate, std::pair<PDDL::HPredicate, bool> >::iterator it = theInitialStatePredicates.find(aTemporaryPredicate);
		if (theInitialStatePredicates.end() == it)
		{
			// Remember aTemporaryPredicate is a :goal predicate which is not an :init predicate
			goal_included_in_init = false;

			if (negative_goal_predicate)
				p = Problem::AddGoalNegativePredicate(aTemporaryPredicate);
			else // (! negative_goal_predicate)
				p = Problem::AddGoalPositivePredicate(aTemporaryPredicate);
		}
		else // (theInitialStatePredicates.end() != it)
		{
			// aTemporaryPredicate is both a :goal predicate and an :init predicate
			if (negative_goal_predicate)
			{
				p = Problem::AddGoalNegativePredicate(it->second.first);

				// Remember whether the search is necessary
				if (! it->second.second)
					// aTemporaryPredicate is a negative :goal predicate and a positive :init predicate
					goal_included_in_init = false;
			}
			else // (! negative_goal_predicate)
			{
				p = Problem::AddGoalPositivePredicate(it->second.first);

				// Remember whether the search is necessary
				if (it->second.second)
					// aTemporaryPredicate is a positive :goal predicate and a negative :init predicate
					goal_included_in_init = false;
			}
		}

		// Process the end of it
		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
		if (negative_goal_predicate)
		{
			if (RIGHT_PAR != token)
			{
				// Tell the user about the missing right parenthesis
				error_stream << theProblemFileName << "(" << line_number << "): Missing closing ')' for :goal predicate "
					<< thePredicateIdentifier
					<< std::endl;
				return;
			}
			token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
		}
		if ((thePredicates[p].GettheArity() == 0) && (! And))
			break;

		if (RIGHT_PAR != token)
		{
			token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
			And = ("and" == token_string);
		}
	}

	if (RIGHT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
	{
		// Tell the user about the missing right parenthesis
		error_stream << theProblemFileName << "(" << line_number << "): Missing closing ')' for the :goal section."
			<< std::endl;
		return;
	}

	// IF the next token_string is a left parenthesis THEN
	//		the next section is a :minimize section
	// ELSE the next token_string must be a right parenthesis closing the PDDL :problem
	if (LEFT_PAR == GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
	{
		// ========================================================== Get the optional :metric section for this problem
		// The :metric section is optional even when :action-costs is required (that is, you are allowed to ignore declared costs
		// But when the :metric section is included in the problem file, its only legal syntax is:
		// (:metric (minimize (total-cost)))
		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
		if ((":metric" == token_string) && (! Required(action_costs)))
		{
			// Tell the user about the missing ":metric" PDDL keyword
			error_stream << theProblemFileName << "(" << line_number << "): PDDL keyword :metric without :action-costs in the :requirements section." 
				<< std::endl;
			// Don't go any further: user must fix the PDDL problem file first
			return;
		}

		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
		if ("minimize" != token_string)
		{
			// Tell the user about the missing "minimize" PDDL keyword
			error_stream << theProblemFileName << "(" << line_number << "): PDDL keyword minimize is expected."
				<< std::endl;
			// Don't go any further: user must fix the PDDL problem file first
			return;
		}
		if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
		{
			// Tell the user about the missing left parenthesis
			error_stream << theProblemFileName << "(" << line_number << "): Left parenthesis is expected."
				<< std::endl;
			// Don't go any further: user must fix the PDDL problem file first
			return;
		}
		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
		if ("total-cost" != token_string)
		{
			// Tell the suer about the missing "total-cost" PDDL keyword
			error_stream << theProblemFileName << "(" << line_number << "): PDDL keyword total-cost is expected."
				<< std::endl;
			// Don't go any further: user must fix the PDDL problem file first
			return;
		}
		do
		{
			if (RIGHT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
			{
				// Tell the suer about the missing left parenthesis
				error_stream << theProblemFileName << "(" << line_number << "): Left parenthesis is expected."
					<< std::endl;
				// Don't go any further: user must fix the PDDL problem file first
				return;
			}
		} while (parenthesis_level != 0);

		// Remember that the search must minimize the total costs of the actions of the plan
		// minimize_total_cost = true;
	}

	// ========================================================== Get the end of this PDDL Problem file
	// The reading of the PDDL problem file now is over, so say bye bye to your best friend
#if ((STL == MEMORY_ALLOCATION)  || (CLASSIC == MEMORY_ALLOCATION))
	free(buffer);
#elif (DL_MALLOC == MEMORY_ALLOCATION)
	SPHG_FREE(buffer);
#elif (HPHA == MEMORY_ALLOCATION)
	SPHG_FREE(buffer, sizeof(char) * size, sizeof(char));
#else
#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
#error MEMORY_ALLOCATION
#endif

	// Tell the user the work is over
	std::cout << "PDDL Problem file " << aProblemFileName << " is now closed." << std::endl;

	// The fall of the curtain
	reading_of_theProblemFile_failed = false;
}

// ================================================================================================ End of file ProblemFile.cpp