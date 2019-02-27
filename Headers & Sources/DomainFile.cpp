// ================================================================================================ Beginning of file "DomainFile.cpp"
// Copyright (c) 2012-2019, Eric Jacopin, ejacopin@ymail.com
/////////////////////////////////////////////////////////////////////////////////////////////////// File Content
//
//  DomainFile/3 --- Calls Read/3
//		  Read/3 --- (READ THE WARNING BELOW) Constructs various structures for this domain while reading a PDDL Domain file
//		SizeOf/0 --- Computes the memory size used by this domain
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////// Inclusion of files
//
#if defined(_MSC_VER)			// _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#include <unordered_map>	// Use C++11 STL hashed structures
#else
	#include <map>				// Use a classic associative STL container when the compiler is not Microsoft's
#endif							// _MSC_VER

// STD and STL files
#include <set>					// Unique occurrences of PDDL::Identifiers and PDDL::HPredicates

#include <sys/stat.h>			// Visibility for stat and _S_IREAD
#include <fstream>

#ifndef _SPHG_PLANNER_CONDITIONAL_COMPILING_H
	#include "ConditionalCompiling.h"		// Visibility for MEMORY_ALLOCATION, STL, HPHA, CLASSIC
#endif

#ifndef _SPHG_PDDL_DOMAIN_FILE_H
	#include "DomainFile.h"		// Class visibility
#endif

#ifndef _SPHG_PDDL_FILES_ROUTINES
	#include "FilesRoutines.h"	// Visibility for GetArity/2, GotoNextLeftPar/2, GotoNextRightPar/2, GetNextToken/3 and SkipWhiteSpacesAndLineFeeds/2
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////// DomainFile::DomainFile/3
//
//
DomainFile::DomainFile(const PDDL::Identifier& aDomainFileName, PDDL::StoreHouse& aPDDLLocalStore, std::ostream& error_stream) {
	Read(aDomainFileName, aPDDLLocalStore, error_stream);
}

/////////////////////////////////////////////////////////////////////////////////////////////////// DomainFile::Read/3
//
//	W A R N I N G  ===  W A R N I N G  ===  W A R N I N G  ===  W A R N I N G  ===  W A R N I N G
//
//
//						DomainFile/3 DOES LITTLE PDDL ERROR RECOVERY
//
//			MAKE YOUR DOMAINFILE PDDL COMPLIANT BEFORE GIVING IT TO THIS CONSTRUCTOR
//
//
//	W A R N I N G  ===  W A R N I N G  ===  W A R N I N G  ===  W A R N I N G  ===  W A R N I N G
//
// ------------------------------------------------------------------------------------------------
//
// The reading of a PDDL Domain file follows the current PDDL Standard:
//	1. Open the file.
//	2. Get the domain name
//	3. Reading the :requirements section
//	4. Reading the :predicate section	(ALL OTHER PDDL sections ARE IGNORED: :constraints and :types)
//	5. Reading the :functions section	(:action-costs is implemented so the only function allowed is total-cost)
//	6. Reading the :action section
//  7. Closing the file.
//
void DomainFile::Read(const PDDL::Identifier& aDomainFileName, PDDL::StoreHouse& aPDDLLocalStore, std::ostream& error_stream) {

	reading_of_theDomainFile_failed = true;	// Set to false at the end of this constructor when the reading of aDomainFileName terminates with no error

	// Remember the name of the PDDL Domain file associated with this PDDL Domain Store
	theDomainFileName = aDomainFileName;

	// Open the PDDL Domain file
	std::ifstream aDomainFile(aDomainFileName);
	if (!aDomainFile)
	{
		// Tell the user about the non-existent problem file
		error_stream << "PDDL Problem file >> " << aDomainFileName << " << cannot be opened!" << std::endl;
		// Don't go any further; 
		return;
	}

	// Place the file into a char buffer:
	//	1. Get the file length
	struct stat buf;
	stat(aDomainFileName.c_str(), &buf);
	long size = buf.st_size;
	//	2. Allocate a buffer with the file length
#if ((STL == MEMORY_ALLOCATION) || (CLASSIC == MEMORY_ALLOCATION) || (DL_MALLOC == MEMORY_ALLOCATION))
	char* buffer = (char*) SPHG_ALLOC(sizeof(char) * size);
#elif (HPHA == MEMORY_ALLOCATION)
	char* buffer = (char*) SPHG_ALLOC(sizeof(char) * size, sizeof(char));
#else
	#pragma message (__FILE__ "(" STRING(__LINE__) "): Unknown value <" STRING(MEMORY_ALLOCATION) "> for MEMORY_ALLOCATION (cf. ConditionalCompiling.h)")
	#error MEMORY_ALLOCATION
#endif
	//	3. Place all the characters of the file into the buffer
	aDomainFile.read(buffer, size);
	//	4. Close the file
	aDomainFile.close();

	// Now, parse the buffer
	PDDL_Token token = END_OF_FILE;			// TRUE <=> (position >= size)
	bool And = false;						// TRUE <=> We are reading a conjunction of preconditions or effects; FALSE <=> There is ONLY ONE precondition to read
	short parenthesis_level = 0;			// At most 127 parentheses levels 
	INTERNAL_TYPES::u16 line_number = 1;	// At most 65535 lines in the buffer
	INTERNAL_TYPES::u16 position = 0;		// At most 65536 characters in the buffer (whereas size is a long)
	PDDL::Identifier token_string = "";		// The successive tokens a PDDL file according to the PDDL syntax ("and", ":action", "not", "(", ")", "?", ...) read from aFileName

	std::set<PDDL::Identifier> theConstantsIdentifiers;

	std::map<PDDL::Identifier, PDDL::HIdentifier> theIdentifiersLocalStore;								// Local dictionary in order to reuse existing identifiers in theIdentifiers of this domain
	std::map<PDDL::Identifier, std::pair<PDDL::HIdentifier, PDDL::HIdentifier> > theConstantsLocalStore1;		// Local dictionary in order to declare constants appearing in predicates as extra parameters of operators
	PDDL::HIdentifier thePositionOftheConstant = 255;
	std::map<PDDL::HIdentifier, std::map<Operator::HArity, PDDL::Identifier> > theConstantsLocalStore2;	// Local dictionary in order to declare extra parameters of operators
	std::map<PDDL::HPredicate, std::vector<std::pair<Operator::HArity, PDDL::Identifier> > > theConstantParametersLocalStore;	// Local dictionary of all extra parameters of a predicate
	std::map<PDDL::HIdentifier, Domain::HOperator> theOperatorsLocalStore;								// Operator's Identifier to Operator's position in the Operator Domain Store

	// ========================================================== Get the domain Name
	if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
	{
		// Tell the user about the missing left parenthesis
		error_stream << theDomainFileName << "(" << line_number << "): Left parenthesis expected."
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}

	token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
	if ("define" != token_string)
	{
		// Tell the user about the PDDL keyword "define" is missing
		error_stream << theDomainFileName << "(" << line_number << "): PDDL keyword define is expected."
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}

	if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
	{
		// Tell the user about the missing left parenthesis
		error_stream << theDomainFileName << "(" << line_number << "): Left parenthesis expected."
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}

	token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
	if ("domain" != token_string)
	{
		// Tell the user about the PDDL keyword "domain" is missing
		error_stream << theDomainFileName << "(" << line_number << "): PDDL keyword domain is expected."
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}
	token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
	theName = token_string;		// the Domain name

	if (RIGHT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
	{
		// Tell the user about the missing right parenthesis
		error_stream << theDomainFileName << "(" << line_number << "): Right parenthesis expected."
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}

	// =================================================== Reading the :requirements section
	// The :typing requirements is accepted but PDDL types are ignored and consequently,
	// NO checking of any kind (not even spelling) is performed on types.
	if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
	{
		// Tell the user about the missing left parenthesis
		error_stream << theDomainFileName << "(" << line_number << "): Left parenthesis expected."
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}

	token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
	if (":requirements" == token_string)
	{
		while (RIGHT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
		{
			if (PDDL::UnknownRequireKey(token_string))
			{
				// Tell the user that token_string contains an invalid PDDL requirement
				error_stream << theDomainFileName << "(" << line_number << "): <" << token_string
					<< "> is not a valid PDDL requirement." << std::endl;
				// Don't go any further: user must fix the PDDL domain file first
				return;
			}
			else if (! PDDL::GettheRequirementInfo(token_string).second)
			{
				// Tell the user that token_string contains an un-implemented PDDL requirement
				error_stream << theDomainFileName << "(" << line_number
					<< "): Features associated with PDDL requirement " << token_string
					<< " are not implemented." << std::endl;
				// Don't go any further: user must fix the PDDL domain file first
				return;
			}

			// Remember a requirement was read in the :requirements section of this PDDL Domain file
			Domain::RegisterRequireKey(token_string);
		}

		if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
		{
			// Tell the user about the missing left parenthesis
			error_stream << theDomainFileName << "(" << line_number << "): Left parenthesis expected."
				<< std::endl;
			// Don't go any further: user must fix the PDDL domain file first
			return;
		}
		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
	}

	// =================================================== Reading the :constants section
	if (":constants" == token_string)
	{
		// Register each declared constant for future checking
		do
		{
			token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
			if (IDENTIFIER == token)
				theConstantsIdentifiers.insert(token_string);
		} while (RIGHT_PAR != token);


		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
		if (LEFT_PAR != token)
		{
			// Tell the user about the missing left parenthesis
			error_stream << theDomainFileName << "(" << line_number << "): Left parenthesis expected."
				<< std::endl;
			// Don't go any further: user must fix the PDDL domain file first
			return;
		}
		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
	}
	
	// =================================================== Reading the :predicate section
	// ------------------------------------- Detection of the :predicates definitions
	// Ignore the following PDDL sections until the :predicates section is found:
	//		- :types			(types for :action or :predicates parameters)
	if (":types" == token_string)
	{
		// Warn the user that PDDL types are badly handled...
		error_stream << theDomainFileName << "(" << line_number 
			<< "): Warning: Types are accepted but neither used nor checked."
			<< std::endl;

		do
		{
			token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
			if (END_OF_FILE == token)
			{
				// Tell the user it's as if the end of file had been read
				error_stream << theDomainFileName << "(" << line_number << "): Unexpected end of file."
					<< std::endl;
				// Don't go any further: user must fix the PDDL domain file first
				return;
			}
		}
		while (RIGHT_PAR != token);

		if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
		{
			// Tell the user about the missing left parenthesis
			error_stream << theDomainFileName << "(" << line_number << "): Left parenthesis expected."
				<< std::endl;
			// Don't go any further: user must fix the PDDL domain file first
			return;
		}
		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
	}

	if (":predicates" != token_string)
	{
		// Tell the user the PDDL keyword ":predicates" is missing
		error_stream << theDomainFileName << "(" << line_number << "): :predicates keyword is missing."
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}

	token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
	if (LEFT_PAR != token)
	{
		// Tell the user about the missing left parenthesis
		error_stream << theDomainFileName << "(" << line_number << "): Left parenthesis expected."
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}

	// Read the identifiers and arities of the PDDL domain predicates
	while (RIGHT_PAR != token)
	{
		if (LEFT_PAR != token)
		{
			// Tell the user about the missing left parenthesis
			error_stream << theDomainFileName << "(" << line_number << "): Left parenthesis expected."
				<< std::endl;
			// Don't go any further: user must fix the PDDL domain file first
			return;
		}
		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
		PDDL::Identifier predicate_identifier = token_string;
		thePredicatesStore[predicate_identifier] = Predicate::PredicateSignature(AddLocalIdentifier(predicate_identifier, theIdentifiersLocalStore), GetArity(token, buffer, size, position, line_number, token_string, parenthesis_level));
		if (RIGHT_PAR != token)
		{
			// Tell the user about the missing right parenthesis
			error_stream << theDomainFileName << "(" << line_number << "): Right parenthesis expected."
				<< std::endl;
			// Don't go any further: user must fix the PDDL domain file first
			return;
		}
		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
	}

	if (Domain::theRequirements[action_costs])
	{
		// From the PDDL 3.1 document:
		// (increase (total-cost) <numeric-term>) where <numeric-term> is either:
		//		- (IMPLEMENTED) a positive numeric constant
		//		- (NOT IMPLEMENTED) (function-symbol <term>*)
		PDDL::Identifier increase("increase");
		thePredicatesStore[increase] = Predicate::PredicateSignature(AddLocalIdentifier(increase, theIdentifiersLocalStore), 2);
	}

	if (MAXIMUM_NUMBER_OF_PREDICATES < thePredicatesStore.size())
	{
		// Warn the user about the great number of predicates in this PDDL domain file
		error_stream << "Warning: The :predicate section of PDDL Domain file " << theDomainFileName 
					 << " declares " << thePredicatesStore.size() << " predicates signatures."
					 << std::endl
					 << "         Only " << MAXIMUM_NUMBER_OF_PREDICATES << " predicates can be further declared (in :action, :init or :goal)."
					 << std::endl;
		// Maybe there should be a return here...
	}


	// =================================================== Reading the :functions section
	// When :action-costs was declared in the :requirements section
	// From domain.pddl of the elevators example of the IPC 2011:
	//		(:functions (total-cost) - number
    //					(travel-slow ?f1 - count ?f2 - count) - number
    //					(travel-fast ?f1 - count ?f2 - count) - number 
	//		)
	if (Domain::theRequirements[action_costs])
	{
		if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
		{
			// Tell the user about the missing left parenthesis
			error_stream << theDomainFileName << "(" << line_number << "): Left parenthesis expected."
				<< std::endl;
			// Don't go any further: user must fix the PDDL domain file first
			return;
		}

		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
		if (":functions" != token_string)
		{
			// Tell the user about the missing ":functions" PDDL keyword
			error_stream << theDomainFileName << "(" << line_number << "): PDDL keyword :functions is expected."
				<< std::endl;
			// Don't go any further: user must fix the PDDL domain file first
			return;
		}

		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
		while (RIGHT_PAR != token)
		{
			if (LEFT_PAR != token)
			{
				// Tell the user about the missing left parenthesis
				error_stream << theDomainFileName << "(" << line_number << "): Left parenthesis expected."
					<< std::endl;
				// Don't go any further: user must fix the PDDL domain file first
				return;
			}

			token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
			PDDL::Identifier function_identifier = token_string;
			theFunctionsStore[function_identifier] = Predicate::PredicateSignature(AddLocalIdentifier(function_identifier, theIdentifiersLocalStore), GetArity(token, buffer, size, position, line_number, token_string, parenthesis_level));

			if (DASH == GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
			{
				token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
				if ("number" != token_string)
				{
					// IF a type is specified THEN it must be number
					error_stream << theDomainFileName << "(" << line_number 
						<< "): PDDL keyword number is expected (number is the only allowed :functions type)."
						<< std::endl;
					// Don't go any further: user must fix the PDDL domain file first
					return;
				}
			}

			token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
		}

		// Check whether total-cost was declared with arity 0 in the :functions section
		Predicate::PredicatesStore::iterator it = theFunctionsStore.find(PDDL::Identifier("total-cost"));
		if (theFunctionsStore.end() == it)
		{
			// Tell the user "total-cost" is missing in the :functions PDDL section of this domain file
			error_stream << "total-cost is missing in the :function section of PDDL domain file " << theDomainFileName
				<< std::endl;
			// Don't go any further: user must fix the PDDL domain file first
			return;
		}
		else if (0 != it->second.second)
		{
			// Tell the user the arity of "total-cost" is 0
			error_stream << "In PDDL domain file " << theDomainFileName
				<< ", total-cost is declared in :functions section with arity "
				<< (INTERNAL_TYPES::u16) (it->second.second)
				<< " whereas total-cost must be declared with arity 0."	<< std::endl;
			// Don't go any further: user must fix the PDDL domain file first
			return;
		}

	}

	// ================================================== Reading the :action section
	if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
	{
		// Tell the user about the missing left parenthesis
		error_stream << theDomainFileName << "(" << line_number << "): Left parenthesis expected."
			<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}
	// while (RIGHT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
	while (0 < parenthesis_level)
	{

		// ------------------------------------------------- Reading one PDDL :action
		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
		if (":action" != token_string)
		{
			// Tell the user the missing ":action" PDDL keyword
			error_stream << theDomainFileName << "(" << line_number
				<< "): PDDL keyword :action is expected." << std::endl;
			// Don't go any further: user must fix the PDDL domain file first
			return;
		}
		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);

		PDDL::HIdentifier Operator_ID = AddLocalIdentifier(token_string, theIdentifiersLocalStore);

		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);

		// ------------------------------------------- Reading the :action parameters
		if (":parameters" != token_string)
		{
			// Tell the user about the missing ":parameters" PDDL keyword
			error_stream << theDomainFileName << "(" << line_number
				<< "): PDDL keyword :parameters is expected." << std::endl;
			// Don't go any further: user must fix the PDDL domain file first
			return;
		}
		std::vector<PDDL::HIdentifier> theActionParametersLocalStore;
		std::map<PDDL::Identifier,						// the Identifier of this parameter in theIdentifiersLocalStore
					std::pair<	Operator::HArity,		// the position of this parameter in the enumeration of parameters for the currently read :action
								PDDL::HIdentifier> >	// the position (in theIdentifiersLocalStore) of the identifier of this parameter
				theActionParametersIdentifiers;			// A local store to ease the checking of predicate parameters (preconditions, additions and deletions)

		while (RIGHT_PAR != token)
		{
			// Now, create a variable for each parameter we read
			if (QUERY == token)
			{
				token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
				// Place the parameters in theVariables of theDomain
				PDDL::HIdentifier p = AddLocalIdentifier(token_string, theIdentifiersLocalStore);
				theActionParametersLocalStore.push_back(p);
				// Remember this parameter so as to later check whether the parameters of preconditions, additions and deletions are valid :action parameters
				// But remember this parameter in theActionParametersIdentifiers: it's a local variable
				theActionParametersIdentifiers[token_string] = std::pair<Operator::HArity, PDDL::HIdentifier>((Operator::HArity) theActionParametersIdentifiers.size(), p);
			}
			token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
		}

		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);

		// Create a local store to later detect whether a predicate has multiple occurrences in the :action as, for instance,
		// it is the case for deleted preconditions; for such predicates, only one predicate shall be stored in
		// theDomain Predicate store and, consequently, only one numeric HPredicate value shall be stored in the :action
		std::map<Predicate, PDDL::HPredicate> theActionPredicatesLocalStore;

		Operator::HArity extra_parameter_position = (Operator::HArity) theActionParametersLocalStore.size();

		std::vector<std::pair<Operator::HArity, PDDL::Identifier> > theTemporaryConstantParametersLocalStore;	// constant parameter position and identifier for this :action

		// ---------------------------------------- Reading the :action preconditions
		if (":precondition" != token_string)
		{
			// Tell the user about the missing ":precondition" PDDL keyword
			error_stream << theDomainFileName << "(" << line_number
				<< "): PDDL keyword :precondition is expected." << std::endl;
			// Don't go any further: user must fix the PDDL domain file first
			return;
		}

		short precondition_parenthesis_level = parenthesis_level;
		if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
		{
			// Tell the user about the missing left parenthesis
			error_stream << theDomainFileName << "(" << line_number << "): Left parenthesis expected."
				<< std::endl;
			// Don't go any further: user must fix the PDDL domain file first
			return;
		}
		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
		And = ("and" == token_string);

		std::set<PDDL::HPredicate> theLocalPreconditions;
		std::set<PDDL::HPredicate> theLocalNegativePreconditions;
		Operator::HPredicate pre = 0;
	
		while (RIGHT_PAR != token)
		{
			if (And)
			{
				if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
				{
					// Tell the user about the missing left parenthesis
					error_stream << theDomainFileName << "(" << line_number << "): Left parenthesis expected."
						<< std::endl;
					// Don't go any further: user must fix the PDDL domain file first
					return;
				}
				token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
			}

			bool PreconditionIsNegative = ("not" == token_string);
			if (PreconditionIsNegative)
			{
				if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
				{
					// Tell the user about the missing left parenthesis
					error_stream << theDomainFileName << "(" << line_number << "): Left parenthesis expected."
						<< std::endl;
					// Don't go any further: user must fix the PDDL domain file first
					return;
				}
				token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);

				if (! Domain::theRequirements[negative_preconditions])
				{
					// Tell the user about the invalid negative precondition
					error_stream << theDomainFileName << "(" << line_number
							<< "): Negative precondition " << token_string 
							<< " encountered in :action " << theIdentifiers[Operator_ID]
							<< " without :negative-preconditions in the :requirements section."
							<< std::endl;
					// Don't go any further: user must fix the PDDL domain file first
					return;
				}
			}

			// This token_string MUST match an Identifier in thePredicatesStore, declared in the :predicates section of the PDDL domain
			Predicate::PredicatesStore::iterator i = thePredicatesStore.find(token_string);

			if (thePredicatesStore.end() == i)
			{
				// Tell the user about the invalid predicate identifier
				error_stream << theDomainFileName << "(" << line_number << "): Precondition predicate identifier "
						<< token_string  << " in :action " << theIdentifiers[Operator_ID] << " is unknown."
						<< std::endl;
				// Don't go any further: user must fix the PDDL domain file first
				return;
			}
			
			// (i != thePredicatesStore.end())
			// Remember the token_string identifier of the predicate for a quick access
			PDDL::Identifier thePredicateIdentifier = token_string;
			// Create a temporary predicate as a template for the precondition we are reading from the PDDL domain file
			Predicate aTemporaryPredicate(i->second.first, i->second.second);
			// Get the arity for parameter checking
			Predicate::HArity a_current = aTemporaryPredicate.GettheArity();

			while (RIGHT_PAR != token)
			{
				// Now, create a variable for each parameter we read
				while (QUERY != token)
				{
					token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
					if (QUERY == token)
					{
						// A new precondition parameter is to be read: check the arity of the declared predicate
						if (0 == a_current)
						{
							// Tell the user about the number of parameters in this precondition
							error_stream << theDomainFileName << "(" << line_number
									<< "): Too many parameters in precondition " << thePredicateIdentifier
									<< "/" << (INTERNAL_TYPES::u16) (aTemporaryPredicate.GettheArity())
									<< " of :action " << theIdentifiers[Operator_ID] << "." 
									<< std::endl;
							// Don't go any further: user must fix the PDDL domain file first
							return;
						}	

						token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
						// Check this token_string is a valid parameter of the :action
						std::map<PDDL::Identifier, std::pair<Operator::HArity, PDDL::HIdentifier> >::iterator i = theActionParametersIdentifiers.find(token_string);
						if (theActionParametersIdentifiers.end() == i)
						{
							// Tell the user about the wrong number of parameters for this precondition
							error_stream << theDomainFileName << "(" << line_number
									<< "): Invalid parameter " << token_string << " in precondition " << GettheIdentifier(thePredicates[(i->second).second].GettheIdentifier())
									<< " of :action " << theIdentifiers[Operator_ID] << "."
									<< std::endl;
							// Don't go any further: user must fix the PDDL domain file first
							return;
						}

						// Add this parameter to the parameters of aTemporaryPredicate (temporary because we're still reading the PDDL Domain file -- i.e. checking the content of the file
						aTemporaryPredicate.AddParameter(aTemporaryPredicate.GettheArity() - a_current, (i->second).second);
						
						// Remember a valid precondition parameter has just been read
						--a_current;
					}
					else if (IDENTIFIER == token)
					{
						// A constant parameter is to be read: check the arity of the declared predicate
						if (0 == a_current)
						{
							// Tell the user about the invalid number of parameters for this precondition
							error_stream << theDomainFileName << "(" << line_number
									<< "): Too many parameters in precondition " << thePredicateIdentifier
									<< "/" << (INTERNAL_TYPES::u16) (aTemporaryPredicate.GettheArity())
									<< " of :action " << theIdentifiers[Operator_ID] << "."
									<< std::endl;
							// Don't go any further: user must fix the PDDL domain file first
							return;
						}

						token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
						if (theConstantsIdentifiers.end() == theConstantsIdentifiers.find(token_string))
						{
							// Tell the user the constant just read should have been declared in the :constants section of this PDDL Domain file
							error_stream << theDomainFileName << "(" << line_number
									<< "): Undeclared constant " << token_string << " appearing in precondition " << thePredicateIdentifier
									<< "/" << (INTERNAL_TYPES::u16) (aTemporaryPredicate.GettheArity())
									<< " of :action " << theIdentifiers[Operator_ID] << "."
									<< std::endl;
						}

						theActionParametersIdentifiers[token_string] = std::pair<Operator::HArity, PDDL::HIdentifier>((Operator::HArity) theActionParametersIdentifiers.size(), thePositionOftheConstant);
						aTemporaryPredicate.AddParameter(aTemporaryPredicate.GettheArity() - a_current, thePositionOftheConstant);

						// Remember the operator and predicate where this constant appeared and its (predicate) parameter position and its extra parameter position for this operator
						theTemporaryConstantParametersLocalStore.push_back(std::pair<Predicate::HArity, PDDL::Identifier>((Operator::HArity) (aTemporaryPredicate.GettheArity() - a_current), token_string));
						theConstantsLocalStore1[token_string] = std::pair<PDDL::HIdentifier, PDDL::HIdentifier>(0, thePositionOftheConstant--);		// Position in theIdentifiers shall be given later
						theConstantsLocalStore2[Operator_ID][extra_parameter_position++] = token_string;

						// Remember a valid precondition parameter has just been read
						--a_current;
					}
					
					if (RIGHT_PAR == token)
					{
						// Check whether the number of parameters given to this precondition predicate is correct
						if (a_current > 0)
						{
							// Tell the user about the wrong number of parameters for this precondition
							error_stream << theDomainFileName << "(" << line_number
									<< "): Not enough parameters in precondition " << thePredicateIdentifier
									<< "/" << (INTERNAL_TYPES::u16) (aTemporaryPredicate.GettheArity())
									<< " of :action " << theIdentifiers[Operator_ID] << "."
									<< std::endl;
							// Don't go any further: user must fix the PDDL domain file first
							return;
						}

						// We are now certain this predicate is a valid precondition, so it is time to remember it:
						// IF this is the first occurrence of this predicate in this :action THEN
						//		Remember this predicate in thePredicates
						//		Copy the parameters of aTemporaryPredicate to the newly added predicate in thePredicates
						//		Remember this predicate in theActionPredicatesLocalStore
						//		Remember the position of this predicate as a precondition of this :action
						// ELSE
						//		Get its position in thePredicatesStore
						//		Remember the position of this predicate as a precondition of this :action
						PDDL::HPredicate p;
						std::map<Predicate, PDDL::HPredicate>::iterator i = theActionPredicatesLocalStore.find(aTemporaryPredicate);
						if (theActionPredicatesLocalStore.end() == i)
						{
							// This predicate is a valid precondition, check whether it can be added to thePredicates store of this PDDL domain
							if (MAXIMUM_NUMBER_OF_PREDICATES < thePredicates.size())
							{
								// Tell the user the domain file declares more predicates than he allowed with MAXIMUM_NUMBER_OF_PREDICATES
								error_stream << theDomainFileName << "(" << line_number
									<< "): Too many predicates (" << thePredicates.size()
										<< ") declared in PDDL domain file " << theDomainFileName << "."
										<< std::endl
										<< "	First extra predicate is precondition " << thePredicateIdentifier
										<< "/" << (INTERNAL_TYPES::u16) (aTemporaryPredicate.GettheArity())
										<< " of :action " << theIdentifiers[Operator_ID] << "." 
										<< std::endl;
								// Don't go any further: user must fix the PDDL problem file first
								return;
							}

							// Remember this predicate in thePredicates
							p = AddPredicate(AddLocalIdentifier(thePredicateIdentifier, theIdentifiersLocalStore), aTemporaryPredicate.GettheArity());
							// Copy the parameters of aTemporaryPredicate in thePredicates[p] and remember the supplementary use of the parameters
							for (Predicate::HArity a = 0; a < aTemporaryPredicate.GettheArity(); ++a)
							{
								PDDL::HIdentifier par = aTemporaryPredicate.GettheParameter(a);
								if (par < thePositionOftheConstant)
									thePredicates[p].AddParameter(a, theActionParametersIdentifiers[theIdentifiers[par]].first);
							}
							// Remember this predicate in theActionPredicatesLocalStore
							theActionPredicatesLocalStore[aTemporaryPredicate] = p;
						}
						else
						{
							// Get its position in thePredicates
							p = (i->second);
						}

						// Remember the position of this predicate as a precondition of this :action
						// An element of a set appears only once in a set; thus:
						//		1. two exact occurrences of the same precondition (e.g. (a ?x) and (a ?x)) shall be stored only once (and not reported)
						//		2. two occurrences of the same predicate (e.g. (a ?x) and (a ?y)) shall be stored differently (and not reported)
						// IF you wish to report on case 1, check before insert(p) whether p alreay is a member of
						//	theLocalNegativePreconditions or theLocalPreconditions
						if (PreconditionIsNegative)
							theLocalNegativePreconditions.insert(p);
						else
							theLocalPreconditions.insert(p);

						for (std::vector<std::pair<Operator::HArity, PDDL::Identifier> >::size_type i = 0; i < theTemporaryConstantParametersLocalStore.size(); ++i)
							theConstantParametersLocalStore[p].push_back(theTemporaryConstantParametersLocalStore[i]);
						theTemporaryConstantParametersLocalStore.clear();

						// Goto reading the next precondition (if any)
						break;
					}
				}
			}

			if (precondition_parenthesis_level == parenthesis_level)
				break;

			// Goto to read next precondition OR ELSE find a ')' and then exit
			token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
			if (PreconditionIsNegative)
			{
				if (RIGHT_PAR != token)
				{
					// Tell the user about the missing right parenthesis
					error_stream << "Missing closing ')' for :action " << theIdentifiers[Operator_ID]
							  << " from PDDL Domain file " << theDomainFileName << "."
							  << std::endl;
					// Don't go any further: user must fix the PDDL domain file first
					return;
				}
				token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
			}

			if (RIGHT_PAR != token)
			{
				token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
				And = ("and" == token_string);
			}
	
		} // ------------------------------- End of reading the :action preconditions

		std::set<PDDL::HPredicate>::size_type theNumberOfPositivePreconditions = theLocalPreconditions.size();			// Could be of type Operator::HPredicate
		std::set<PDDL::HPredicate>::size_type theNumberOfNegativePreconditions = theLocalNegativePreconditions.size();	// Could be of type Operator::HPredicate
		PDDL::Number aNumericCost = 0.0f;

		// ---------------------------------------------- Reading the :action effects
		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
		if (":effect" != token_string)
		{
			// Tell the user about the missing ":effect" PDDL keyword
			error_stream << theDomainFileName << "(" << line_number << "): PDDL keyword :effect is expected."
				<< std::endl;
			// Don't go any further: user must fix the PDDL domain file first
			return;
		}
		short effect_parenthesis_level = parenthesis_level;
		if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
		{
			// Tell the user about the missing left parenthesis
			error_stream << theDomainFileName << "(" << line_number << "): Left parenthesis expected."
				<< std::endl;
			// Don't go any further: user must fix the PDDL domain file first
			return;
		}
		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
		And = ("and" == token_string);

		Operator::HPredicate add = 0;
		Operator::HPredicate del = 0;

		std::vector<PDDL::HPredicate> theLocalAddedPreconditions;
		std::vector<PDDL::HPredicate> theLocalAdditions;
		std::vector<PDDL::HPredicate> theLocalDeletedPreconditions;
		std::vector<PDDL::HPredicate> theLocalDeletions;

		while (RIGHT_PAR != token)
		{
			if (And)
			{
				if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
				{
					// Tell the user about the missing left parenthesis
					error_stream << theDomainFileName << "(" << line_number << "): Left parenthesis expected."
						<< std::endl;
					// Don't go any further: user must fix the PDDL domain file first
					return;
				}
				token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
			}

			// Remember whether we are to read a negated effect and if so, get the next token_string (which MUST be an identifier)
			bool EffectIsDeleted = ("not" == token_string);
			if (EffectIsDeleted)
			{
				if (LEFT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
				{
					// Tell the user about the missing left parenthesis
					error_stream << theDomainFileName << "(" << line_number << "): Left parenthesis expected."
						<< std::endl;
					// Don't go any further: user must fix the PDDL domain file first
					return;
				}
				token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
			}

			// This token_string MUST match an Identifier in thePredicatesStore, declared in the :predicates section of the PDDL domain
			Predicate::PredicatesStore::iterator i = thePredicatesStore.find(token_string);
			if (thePredicatesStore.end() == i)
			{
				// Tell the user about the invalid predicate identifier
				error_stream << theDomainFileName << "(" << line_number << "): Effect predicate identifier "
						<< token_string << " in :action " << theIdentifiers[Operator_ID] << " is unknown."
						<< std::endl;
				// Don't go any further: user must fix the PDDL domain file first
				return;
			}
			
			// (i != thePredicatesStore.end())
			// Remember the token_string identifier of the predicate for a quick access
			PDDL::Identifier thePredicateIdentifier = token_string;
			// Create a temporary predicate as a template for the effect we are reading from the PDDL domain file
			Predicate aTemporaryPredicate(i->second.first, i->second.second);
			// Get the arity for parameter checking
			Predicate::HArity a_current = aTemporaryPredicate.GettheArity();

			while (RIGHT_PAR != token)
			{
				// Now, create a variable for each parameter we read
				while (QUERY != token)
				{
					token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
					if (QUERY == token)
					{
						// A new effect parameter is to be read: check the arity of the declared predicate
						if (0 == a_current)
						{
							// Tell the user about the invalid number of parameter for this predicate
							error_stream << theDomainFileName << "(" << line_number
									<< "): Too many parameters in effect " << thePredicateIdentifier
									<< "/" << (INTERNAL_TYPES::u16) (aTemporaryPredicate.GettheArity())
									<< " of :action " << theIdentifiers[Operator_ID] << "."
									<< std::endl;
							// Don't go any further: user must fix the PDDL domain file first
							return;
						}

						token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
						// Check this token_string is a valid parameter of the :action
						std::map<PDDL::Identifier, std::pair<Operator::HArity, PDDL::HIdentifier> >::iterator i = theActionParametersIdentifiers.find(token_string);
						if (theActionParametersIdentifiers.end() == i)
						{
							// Tell the user about a parameter which appears in a predicate but is not declared in the :parameters of this :action
							error_stream << theDomainFileName << "(" << line_number << "): Invalid parameter "
									  << token_string << " in effect " << GettheIdentifier(thePredicates[(i->second).second].GettheIdentifier())
									  << " of :action " << theIdentifiers[Operator_ID] << "."
									  << std::endl;
							// Don't go any further: user must fix the PDDL domain file first
							return;
						}

						// Add this parameter to the parameters of aTemporaryPredicate (temporary because we're still reading the PDDL Domain file -- i.e. checking the content of the file
						aTemporaryPredicate.AddParameter(aTemporaryPredicate.GettheArity() - a_current, (i->second).second);
						
						// Remember a valid effect parameter has just been read
						--a_current;
					}
					else if (LEFT_PAR == token) // Then check whether it's a :function for :predicate "increase"
					{

						// A new effect parameter is to be read: check the arity of the declared predicate
						if (0 == a_current)
						{
							// Tell the user about the invalid number of parameter for this predicate
							error_stream << theDomainFileName << "(" << line_number << "): Too many parameters in effect "
									  << thePredicateIdentifier << "/" << (INTERNAL_TYPES::u16) (aTemporaryPredicate.GettheArity())
									  << " of :action " << theIdentifiers[Operator_ID] << "."
									  << std::endl;
							// Don't go any further: user must fix the PDDL domain file first
							return;
						}
						if (("increase" == thePredicateIdentifier) && (2 == a_current))
						{
							token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
							if ("total-cost" != token_string)
							{
								// Tell the user about the invalid number of parameter for this predicate
								error_stream << theDomainFileName << "(" << line_number
										  << "): Function identifier <total-cost> expected." 
										  << std::endl;
								// Don't go any further: user must fix the PDDL domain file first
								return;
							}
							if (RIGHT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
							{
								// Tell the user about the invalid number of parameter for this predicate
								error_stream << theDomainFileName << "(" << line_number
										  << "): Left parenthesis expected (total-cost is a 0-ary function)." 
										  << std::endl;
								// Don't go any further: user must fix the PDDL domain file first
								return;
							}
							if (NUMBER != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
							{
								// Tell the user about the invalid number of parameter for this predicate
								error_stream << theDomainFileName << "(" << line_number
										  << "): The second parameter of predicate increase/2 must be a positive number." 
										  << std::endl;
								// Don't go any further: user must fix the PDDL domain file first
								return;
							}
							ValidPDDLNumber(token_string, aNumericCost);

							token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
							if (RIGHT_PAR != token)
							{
								// Tell the user about the invalid number of parameter for this predicate
								error_stream << theDomainFileName << "(" << line_number
										  << "): Right parenthesis expected after numeric cost." 
										  << std::endl;
								// Don't go any further: user must fix the PDDL domain file first
								return;
							}
							a_current = 0;
							break;
						}
						else
						{
							// Tell the user about the invalid number of parameter for this predicate
							error_stream << theDomainFileName << "(" << line_number
									  << "): Unexpected function term as parameter of predicate "
									  << thePredicateIdentifier
									  << std::endl;
							// Don't go any further: user must fix the PDDL domain file first
							return;
						}
					}
					else if (IDENTIFIER == token)
					{
						// A constant parameter is to be read: check the arity of this predicate
						if (0 == a_current)
						{
							// Tell the user about the invalid number of parameter for this predicate
							error_stream << theDomainFileName << "(" << line_number << "): Too many parameters in effect "
									<< thePredicateIdentifier << "/" << (INTERNAL_TYPES::u16) (aTemporaryPredicate.GettheArity())
									<< " of :action " << theIdentifiers[Operator_ID] << "."
									<< std::endl;
							// Don't go any further: user must fix the PDDL domain file first
							return;
						}

						if (theConstantsIdentifiers.end() == theConstantsIdentifiers.find(token_string))
						{
							// Tell the user the constant just read should have been declared in the :constants section of this PDDL Domain file
							error_stream << theDomainFileName << "(" << line_number
								<< "): Undeclared constant " << token_string << " appearing in effect " << thePredicateIdentifier
								<< "/" << (INTERNAL_TYPES::u16) (aTemporaryPredicate.GettheArity())
								<< " of :action " << theIdentifiers[Operator_ID]
								<< "." << std::endl;
							// Don't go any further: user must fix the PDDL domain file first
							return;
						}

						theActionParametersIdentifiers[token_string] = std::pair<Operator::HArity, PDDL::HIdentifier>((Operator::HArity) theActionParametersIdentifiers.size(), thePositionOftheConstant);
						aTemporaryPredicate.AddParameter(aTemporaryPredicate.GettheArity() - a_current, thePositionOftheConstant);

						// Remember the operator and predicate where this constant appeared and its (predicate) parameter position and its extra parameter position for this operator
						theTemporaryConstantParametersLocalStore.push_back(std::pair<Predicate::HArity, PDDL::Identifier>(aTemporaryPredicate.GettheArity() - a_current, token_string));
						theConstantsLocalStore1[token_string] = std::pair<PDDL::HIdentifier, PDDL::HIdentifier>(0, thePositionOftheConstant--);		// Position in theIdentifiers shall be given later
						theConstantsLocalStore2[Operator_ID][extra_parameter_position++] = token_string;

						// Remember a valid precondition parameter has just been read
						--a_current;
					}
					else if (RIGHT_PAR == token)
					{
						// Check all the parameters were given to this effect predicate
						if (a_current > 0)
						{
							// Tell the user about the wrong number of parameters for this effect
							error_stream << theDomainFileName << "(" << line_number << "): Not enough parameters in effect "
									<< thePredicateIdentifier << "/" << INTERNAL_TYPES::u16(aTemporaryPredicate.GettheArity())
									<< " of :action " << theIdentifiers[Operator_ID] << "."
									<< std::endl;
							// Don't go any further: user must fix the PDDL domain file first
							return;
						}

						// We are now certain this predicate is a valid effect, so it is time to remember it:
						// IF this is the first occurrence of this predicate in this :action THEN
						//		Remember this predicate in thePredicates
						//		Copy the parameters of aTemporaryPredicate to the newly added predicate in thePredicates
						//		Remember this predicate in theActionPredicatesLocalStore
						//		Remember the position of this predicate as an effect of this :action
						// ELSE
						//		Get its position in thePredicatesStore
						//		Remember the position of this predicate as an effect of this :action

						PDDL::HPredicate p;
						std::map<Predicate, PDDL::HPredicate>::iterator i = theActionPredicatesLocalStore.find(aTemporaryPredicate);
						if (theActionPredicatesLocalStore.end() == i)
						{
							// This predicate is a valid precondition, check whether it can be added to thePredicates store of this PDDL domain
							if (MAXIMUM_NUMBER_OF_PREDICATES < thePredicates.size())
							{
								// Tell the user the domain file declares more predicates than he allowed with MAXIMUM_NUMBER_OF_PREDICATES
								error_stream << theDomainFileName << "(" << line_number
										<< "): Too many predicates (" << thePredicates.size()
										<< ") declared in PDDL domain file " << theDomainFileName << "."
										<< std::endl
										<< "	First extra predicate is effect " << thePredicateIdentifier
										<< "/" << (INTERNAL_TYPES::u16) (aTemporaryPredicate.GettheArity())
										<< " of :action " << theIdentifiers[Operator_ID] << "." 
										<< std::endl;
								// Don't go any further: user must fix the PDDL problem file first
								return;
							}

							// Remember this predicate in thePredicates
							p = AddPredicate(AddLocalIdentifier(thePredicateIdentifier, theIdentifiersLocalStore), aTemporaryPredicate.GettheArity());
							// Copy the parameters of aTemporaryPredicate in thePredicates[p] and remember the supplementary use of the parameters
							for (Predicate::HArity a = 0; a < aTemporaryPredicate.GettheArity(); ++a)
							{
								PDDL::HIdentifier par = aTemporaryPredicate.GettheParameter(a);
								// Add a pointer to the :action parameter only if this parameter is not a constant of this domain
								if (par < thePositionOftheConstant)
									thePredicates[p].AddParameter(a, theActionParametersIdentifiers[theIdentifiers[par]].first);
							}
							// Remember this predicate in theActionPredicatesLocalStore
							theActionPredicatesLocalStore[aTemporaryPredicate] = p;
						}
						else
						{
							// Get its position in thePredicates
							p = (i->second);
						}

						// Remember the position of this predicate as an effect of this :action
						// An element of a set appears only once in a set; thus:
						//		1. two exact occurrences of the same effect (e.g. (a ?x) and (a ?x)) shall be stored only once (and not reported)
						//		2. two occurrences of the same effect (e.g. (a ?x) and (a ?y)) shall be stored differently (and not reported)
						// IF you wish to report on case 1, check before push_back(p) whether p alreay is a member of
						//	theLocalDeletions, theLocalDeletedPreconditions, theLocalAdditions or theLocalAddedPreconditions
						if (EffectIsDeleted)
						{
							// Check whether this deleted predicate also is a precondition
							std::set<PDDL::HPredicate>::iterator it = theLocalPreconditions.find(p);
							if (theLocalPreconditions.end() == it)
								theLocalDeletions.push_back(p);
							else // (it != theLocalPreconditions.end())
							{
								// No need to remember this predicate any longer
								theLocalPreconditions.erase(p);
								theLocalDeletedPreconditions.push_back(p);
							}
						}
						else
						{
							// Check whether this added predicate also is a negative precondition
							std::set<PDDL::HPredicate>::iterator it = theLocalNegativePreconditions.find(p);
							if (theLocalNegativePreconditions.end() == it)
								theLocalAdditions.push_back(p);
							else // (it != theLocalNegativePreconditions.end())
							{
								// No need to remember this predicate any longer
								theLocalNegativePreconditions.erase(p);
								theLocalAddedPreconditions.push_back(p);
							}

						}

						for (std::vector<std::pair<Operator::HArity, PDDL::Identifier> >::size_type i = 0; i < theTemporaryConstantParametersLocalStore.size(); ++i)
							theConstantParametersLocalStore[p].push_back(theTemporaryConstantParametersLocalStore[i]);
						theTemporaryConstantParametersLocalStore.clear();

						// Goto reading the next effect (if any)
						break;
					}
				}
			}

			if (effect_parenthesis_level == parenthesis_level)
				break;

			// Goto to read next effect OR ELSE find a ')' closing the and of the :effect and then exit
			token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
			if (EffectIsDeleted)
			{
				if (RIGHT_PAR != token)
				{
					// Tell the user about the missing right parenthesis
					error_stream << theDomainFileName << "(" << line_number << "): Missing closing ')' for :action "
							<< theIdentifiers[Operator_ID]
							<< std::endl;
					// Don't go any further: user must fix the PDDL domain file first
					return;
				}
				token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
			}

			if (RIGHT_PAR != token)
			{
				token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);
				And = ("and" == token_string);
			}
		} // --------------------------------------------- End of reading the effects

		// Initiate either the next action or else the end of this PDDL Domain file
		if (RIGHT_PAR != GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level))
		{
			// Tell the user about the missing right parenthesis
			error_stream << theDomainFileName << "(" << line_number << "): Missing closing ')' for :action "
					<< theIdentifiers[Operator_ID] << "."
					<< std::endl;
			// Don't go any further: user must fix the PDDL domain file first
			return;
		}

		// Initiate either the next action (i.e. an opening "(") or else the end of this PDDL Domain file (i.e. a closing ")")
		token = GetNextToken(buffer, size, position, line_number, token_string, parenthesis_level);


		// ------------------------------------------------ Creating operators in the PDDL Domain store house
		// We are now certain what we just read is a valid PDDL :action, so it's time to construct the corresponding Operator for this domain:
		//  1. Reserve memory for this operator and get a (n integer) pointer to that memory
		//  2. Declare the cost of this operator
		//	3. Declare the positions of this operator
		//  4. Declare the parameters of this operator
		//  5. Declare the predicates of this operator

		// 1. Create a new operator in the PDDL Domain store house
		Domain::HOperator anOperator = Domain::AddOperator(Operator(Operator_ID,
													(Operator::HArity) theActionParametersLocalStore.size(),
													(Operator::HPredicate) theLocalDeletedPreconditions.size(),
													(Operator::HPredicate) theNumberOfPositivePreconditions,
													(Operator::HPredicate) theLocalDeletions.size(),
													(Operator::HPredicate) (theLocalAdditions.size() + theLocalAddedPreconditions.size()),
													(Operator::HPredicate) theNumberOfNegativePreconditions,
													(Operator::HPredicate) theLocalAddedPreconditions.size()));
		theOperatorsLocalStore[Operator_ID] = anOperator;

		// 2. Remember the cost of this operator
		AddNumericCost(anOperator, aNumericCost);	// aNumericCost was initiliazed to 0.0f and maybe got a greater value if :action-costs is required

		// 3. Setting the positions	(predicate number and parameter number, of the paramters of this operator)
		std::vector<std::vector<Operator::Position>> theActionParametersPositionsLocalStore(theActionParametersLocalStore.size());
		Operator::HPredicate precond = 0;
		//	- Positions from the pure preconditions
		for (std::set<PDDL::HPredicate>::iterator it = theLocalPreconditions.begin(); it != theLocalPreconditions.end(); ++it)
		{
			for(Predicate::HArity a = 0; a < thePredicates[(*it)].GettheArity(); ++a)
				theActionParametersPositionsLocalStore[thePredicates[(*it)].GettheParameter(a)].push_back(Operator::Position(precond,a));

			++precond;
		}
		//	- Positions from the deleted preconditions
		for (std::vector<PDDL::HPredicate>::size_type del_pre = 0; del_pre < theLocalDeletedPreconditions.size(); ++del_pre)
		{
			for(Predicate::HArity a = 0; a < thePredicates[theLocalDeletedPreconditions[del_pre]].GettheArity(); ++a)
				theActionParametersPositionsLocalStore[thePredicates[theLocalDeletedPreconditions[del_pre]].GettheParameter(a)].push_back(Operator::Position(precond,a));

			++precond;
		}
		// See whether there are operator parameters not linked to any (either pure or else deleted) precondition parameters
		for (std::vector<std::vector<Operator::Position>>::size_type i = 0; i < theActionParametersPositionsLocalStore.size(); ++i)
		{
			if (0 == theActionParametersPositionsLocalStore[i].size())
			{
				// Warn the user about this "free" operator parameter
				error_stream << theDomainFileName << "(:action " << theIdentifiers[Operator_ID]
						<< "): Warning! No precondition parameter matches operator parameter <" << theIdentifiers[theActionParametersLocalStore[i]] << ">."
						<< std::endl;
			}
		}
		AddOperatorPositions(anOperator, theActionParametersPositionsLocalStore);	// Store the computed positions into anOperator

		// 4. Setting the parameters
		for (std::vector<PDDL::HIdentifier>::size_type i = 0; i < theActionParametersLocalStore.size(); ++i)
			AddOperatorParameter(anOperator, (Operator::HArity) i, PDDL::Parameter(PDDL::VARIABLE, theActionParametersLocalStore[i]));

		// 5. Setting the predicates, necessarily in the following order:
		//		(1) pure preconditions
		//		(2) deleted preconditions (i.e. (p) is a precondition and (not (p)) is an effect)
		//		(3) pure additions
		//		(4) added negative preconditions (i.e. (not p) is a precondition and (p) is an effect)
		//		(5) negative preconditions (i.e. (not p) is a precondition and does not appear anywhere else
		precond = 0;	// Increased for the setting of Positions; reset for the next task of setting preconditions
		//	5.1. Setting the preconditions
		//		- Pure preconditions:
		for (std::set<PDDL::HPredicate>::iterator it = theLocalPreconditions.begin(); it != theLocalPreconditions.end(); ++it)
			AddPrecondition(anOperator, precond++, (*it));
		//		- Deleted preconditions:
		for (std::vector<PDDL::HPredicate>::size_type del_pre = 0; del_pre < theLocalDeletedPreconditions.size(); ++del_pre)
			AddPrecondition(anOperator, (Operator::HPredicate) (precond + del_pre), theLocalDeletedPreconditions[del_pre]);
		//	5.2. Setting the pure deletions
		for (std::vector<PDDL::HPredicate>::size_type del = 0; del < theLocalDeletions.size(); ++del)
			AddDeletion(anOperator, (Operator::HPredicate) (theLocalPreconditions.size() + theLocalDeletedPreconditions.size() + del), theLocalDeletions[del]);
		//	5.3. Setting the additions
		//		- Pure additions
		for (std::vector<PDDL::HPredicate>::size_type add = 0; add < theLocalAdditions.size(); ++add)
			AddAddition(anOperator, (Operator::HPredicate) (theLocalPreconditions.size() + theLocalDeletedPreconditions.size() + theLocalDeletions.size() + add), theLocalAdditions[add]);
		//		- Negative preconditions (not p) which are now added predicates (p) is an effect
		for (std::vector<PDDL::HPredicate>::size_type add_neg_pre = 0; add_neg_pre < theLocalAddedPreconditions.size(); ++add_neg_pre)
			AddAddition(anOperator, (Operator::HPredicate) (theLocalPreconditions.size() + theLocalDeletedPreconditions.size() + theLocalDeletions.size() + theLocalAdditions.size() + add_neg_pre), theLocalAddedPreconditions[add_neg_pre]);
		std::vector<PDDL::HPredicate>::size_type neg_pre = theLocalPreconditions.size() + theLocalDeletedPreconditions.size() + theLocalDeletions.size() + theLocalAdditions.size() + theLocalAddedPreconditions.size();
		//	5.4. Setting the negative preconditions
		for (std::set<PDDL::HPredicate>::iterator it = theLocalNegativePreconditions.begin(); it != theLocalNegativePreconditions.end(); ++it)
			AddAddition(anOperator, (Operator::HPredicate) neg_pre++, (*it));

	} // ================================================ End of reading the :actions


	// ======== Validating the closing of the "define" of this domain: current == ')'
	// The reading of the PDDL domain file now is over, so say bye bye to your best friend

	// It's now time to fill up the constant parameters in the predicates
	// thePositionOfTheFirstConstant = theIdentifiers.size();		// Could be "Domain::SetthePositionOfTheFirstConstant();" as well
	for (std::map<PDDL::Identifier, std::pair<PDDL::HIdentifier, PDDL::HIdentifier> >::iterator it = theConstantsLocalStore1.begin(); it != theConstantsLocalStore1.end(); ++it)
		(it->second).first = Domain::AddIdentifier(it->first);

	for (std::map<PDDL::HPredicate, std::vector<std::pair<Operator::HArity, PDDL::Identifier> > >::iterator it = theConstantParametersLocalStore.begin(); it != theConstantParametersLocalStore.end(); ++it)
		for (std::vector<std::pair<Operator::HArity, PDDL::Identifier> >::size_type a = 0; a < (it->second).size(); ++a)
			thePredicates[(it->first)].AddParameter((it->second)[a].first, it->first);

	for (std::map<PDDL::HIdentifier, std::map<Predicate::HArity, PDDL::Identifier> >::iterator it = theConstantsLocalStore2.begin(); it != theConstantsLocalStore2.end(); ++it)
		for (std::map<Predicate::HArity, PDDL::Identifier>::iterator it_op = (it->second).begin(); it_op != (it->second).end(); ++it_op)
		{
			theOperators[theOperatorsLocalStore[(it->first)]].Resize((Operator::HArity) (it->second).size());
			theOperators[theOperatorsLocalStore[(it->first)]].SettheParameter((it_op->first), PDDL::Parameter(PDDL::STRING_VALUE, theConstantsLocalStore1[(it_op->second)].first));
		}

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

	if (0 < parenthesis_level)
	{
		// ("EOF" == token_string)
		error_stream << theDomainFileName << "(" << line_number << "): End of PDDL Domain file " << theDomainFileName << " expected."
				<< std::endl;
		// Don't go any further: user must fix the PDDL domain file first
		return;
	}

	// Tell the user about the last :action
	std::cout << "PDDL Domain file " << aDomainFileName << " is now closed." << std::endl
			  << "   Last :action read was " << GettheIdentifier(theOperators[theOperators.size() - 1].GettheIdentifier()) 
			  << " out of " << theOperators.size() << " :action (s)."
			  << std::endl;

	// Register this neat PDDL Domain for future use
	aPDDLLocalStore[theName] = this;

	// The fall of the curtain
	reading_of_theDomainFile_failed = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////// DomainFile::SizeOf/0
//
size_t DomainFile::SizeOf() const {
	// ------------------------------------ Values correspond to Microsoft's Visual C++ for Windows

	// Measure the size of theDomain store for Identifiers
	size_t sI = sizeof(theIdentifiers);		// 20 initial bytes
	for (PDDL::HIdentifier i = 0; i < theIdentifiers.size(); ++i)
		sI += sizeof(theIdentifiers[i]);

	// Measure the size of theDomain store for Predicates
	size_t sP = sizeof(thePredicates);		// 20 initial bytes
	for (PDDL::HPredicate p = 0; p < thePredicates.size(); ++p)
		sP += thePredicates[p].SizeOf();

	// Measure the size of theDomain store for Operators
	size_t sO = sizeof(theOperators);		// 20 initial bytes
	for (Domain::HOperator o = 0; o < theOperators.size(); ++o)
		sO += theOperators[o].SizeOf();

	size_t sO1 = sizeof(theName);

	// Report the measures to the standard console
	std::cout << std::endl
			  << "Memory usage for PDDL Domain file " << theDomainFileName << ":" << std::endl
			  << "   + " << sO1 << " bytes used for the domain name." << std::endl
			  << "   + " << theIdentifiers.size() << " identifiers for " <<  sI << " bytes." << std::endl 
			  << "   + " << thePredicates.size() << " predicates for " << sP << " bytes." << std::endl 
			  << "   + " << theOperators.size() << " operators for " << sO << " bytes." << std::endl
			  << "   = " << (sO1 + sI + sP + sO) << " bytes." << std::endl;

	size_t sO2 = sizeof(theDomainFileName);
	size_t sO3 = sizeof(reading_of_theDomainFile_failed);

	// Measure the size of theDomain store for Constants
	size_t sC = sizeof(theConstants);
	for (std::map<PDDL::Identifier, PDDL::HIdentifier>::const_iterator i = theConstants.begin(); i != theConstants.end(); ++i)
		sC += sizeof((*i));

	// Measure the size of theDomain store for Problems
	size_t sPb = sizeof(theProblems);
	for (std::vector<Problem*>::size_type p = 0; p < theProblems.size(); ++p)
		sPb += sizeof(theProblems[p]);

	size_t sPs = sizeof(thePredicatesStore);
	for(Predicate::PredicatesStore::const_iterator i = thePredicatesStore.begin(); i != thePredicatesStore.end(); ++i)
		sPs += sizeof(thePredicatesStore);

	size_t sR = 0;
	for (PDDL::HRequirement r = 0; r < PDDL::NUMBER_OF_REQUIREMENTS; ++r)
		sR += sizeof(theRequirements[r]);

	std::cout << "Extra memory used to store this PDDL Domain:" << std::endl
			  << "   + " << "1 domain file name for " << sO2 << " bytes." << std::endl
			  << "   + " << "1 boolean for " << sO3 << " bytes." << std::endl
			  << "   + " << theConstants.size() << " constants for " << sC << " bytes." << std::endl
			  << "   + " << theProblems.size() << " problems for " << sPb << " bytes." << std::endl
			  << "   + " << thePredicatesStore.size() << " predicates for " << sPs << " bytes." << std::endl
			  << "   + " << INTERNAL_TYPES::u16(PDDL::NUMBER_OF_REQUIREMENTS) << " requirements for " << sR << " bytes." << std::endl
			  << "   = " << (sO2 + sO3 + sC + sPb + sPs + sR) << " bytes." << std::endl
			  << "Total memory size is " << (sI + sP + sO + sC + sPb + sO1 + sO2 + sO3 + sPs + sR) << " bytes." << std::endl;

	return (sI + sP + sO + sC + sPb + sO1 + sO2 + sO3 + sPs + sR);
}

// ================================================================================================ End of file "DomainFile.cpp"