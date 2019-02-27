// ================================================================================================ Beginning of file DomainFile.h
// Copyright (c) 2012-2019, Eric Jacopin, ejacopin@ymail.com
// ------------------------------------------------------------------------------------------------
#ifndef _SPHG_PDDL_DOMAIN_FILE_H
#define _SPHG_PDDL_DOMAIN_FILE_H

#if defined(_MSC_VER)	// _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#pragma once		// When compiling, this file will be included (opened) only once by the Microsoft C++ compiler 
#endif					// _MSC_VER

// ------------------------------------------------------------------------------------------------ Inclusion of files
// STD and STL files
#include <iostream>		// Error messages on std::cerr and end of reading on std::cout

// Project files
#ifndef _SPHG_PDDL_DOMAIN_H
	#include "Domain.h"
#endif

// ------------------------------------------------------------------------------------------------ Class definition
class DomainFile : public Domain {
	// ----- Properties
	private:
		PDDL::Identifier theDomainFileName;			// the name of the Domain File
		bool reading_of_theDomainFile_failed;		// Initially set to true by the constructor; set to false only when the reading of this PDDL Domain file correctly (i.e. no error occurred) is over


	// ----- Constructors
	public:
		DomainFile() {}
		DomainFile(const PDDL::Identifier& aDomainFileName,		/* Name of the PDDL file name */
						 PDDL::StoreHouse& aPDDLLocalStore,		/* RAM Store of the PDDL Domain filled with the data from the PDDL Domain file */
						 std::ostream& error_stream = std::cerr /* Where PDDL syntax errors are reported */);


	// ----- Accessors
	public:
		inline PDDL::Identifier GettheDomainFileName() const {		// What is the name of the file containing this PDDL Domain?
			return theDomainFileName;
		}
		inline bool ReadingFailed() const {		// Did the reading of this DomainFile terminate correctly?
			return reading_of_theDomainFile_failed;
		}

	// ----- Operations
	private:
		inline PDDL::HIdentifier AddLocalIdentifier(PDDL::Identifier& token, std::map<PDDL::Identifier, PDDL::HIdentifier>& theIdentifiersLocalStore) {
			std::map<PDDL::Identifier, PDDL::HIdentifier>::iterator it = theIdentifiersLocalStore.find(token);
			// Remember the token as domain identifier when it's the first time we read it, otherwise return its position in theIdentifiers
			if (theIdentifiersLocalStore.end() == it)
			{
				PDDL::HIdentifier p = Domain::AddIdentifier(token);
				theIdentifiersLocalStore[token] = p;
				return p;
			}
			else // (theIdentifiersLocalStore.end() != it) <==> token was previously read from the PDDL Domain file: reuse its position in theIdentifiersLocalStore
				return (it->second);
		}

	protected:
		void Read(const PDDL::Identifier& aDomainFileName, PDDL::StoreHouse& aPDDLLocalStore, std::ostream& error_stream);	// Parsing the PDDL Domain and building its data into the RAM Store

	public:
		size_t SizeOf() const;	// Writing the number of identifiers, variables/parameters, predicates and actions in this PDDL Domain file

};

#endif	// _SPHG_PDDL_DOMAIN_FILE_H
// ================================================================================================ End of file DomainFile.h