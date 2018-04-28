// ================================================================================================ Beginning of file "main_BCP.cpp"
// Copyright (c) 2012-2018, Eric Jacopin, ejacopin@ymail.com
/////////////////////////////////////////////////////////////////////////////////////////////////// File Content
//
//	This file contains the code for the following target of the BCP planning component:
//		- Console Application where planning data input is from PDDL files
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////////// Inclusion of files
//
#ifndef _SPHG_PLANNER_CONDITIONAL_COMPILING_H
	#include "ConditionalCompiling.h"	// Visibility for BCP_TARGET, BUILD, DLL, TEST and C_SHARP_INTERFACE
#endif
//
/////////////////////////////////////////////////////////////////////////////////////////////////// Console Application
//
//                           C O N S O L E     A P P L I C A T I O N
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Files included for the Console Application target
#ifndef _DOMAIN_FILE_H
	#include "DomainFile.h"
#endif

#ifndef _SPHG_PLANNER_PLAN_H
	#include "Plan.h"
#endif

#ifndef _SPHG_PDDL_PROBLEM_FILE_H
	#include "ProblemFile.h"
#endif

#ifndef _SPHG_TOOLS_RUNTIMES_H
	#include "Runtimes.h"
#endif

#ifndef _SPHG_PLANNER_SEARCH_SSFC_H
	#include "StateSpaceForwardChaining.h"
#endif


// ************************************************************************************************
//
//	                             R E T U R N E D     V A L U E S
//
// ------------------------------------------------------------------------------------------------
//
#define HELP							   -1;		// Somebody asked for information about the application

#define SEARCH_IS_NOT_NEEDED				0;		// The :goal in the PDDL Problem file is declared included in the :init
#define	SEARCH_STOPS_FOUND_A_SOLUTION		1;		// Plan is at least 1 action long
#define SEARCH_ENDS_WITH_NO_SOLUTION		2;		// Empty plan

#define	ERROR_READING_DOMAIN_FILE			11;		// PDDL Domain file must be fixed
#define	ERROR_READING_PROBLEM_FILE			12;		// PDDL Problem file must be fixed
#define ERROR_UNKNOWN_OPTION				13;		// See next comments on Command-Line Options for available options
#define ERROR_DASH_PREFIX_MISSING			14;		// The '-' prefix is missing before an option
#define ERROR_WRONG_NUMBER_OF_ARGUMENTS		15;		// Correct number of arguments is 1, 2, 3 or 4 (we don't count argv[0] cf. line 79 or line 221))


// ************************************************************************************************
//
//							C O M M A N D - L I N E		O P T I O N S 
//
// ------------------------------------------------------------------------------------------------
//
//		-h		// prints some help about this application
//		-help	// prints some help about this application
//		-m		// prints some measures on the memory use
//		-r		// prints the reading of both the PDDL Domain and Problem runtimes and SEARCH runtime
//
int main(int argc, char* argv[]) {

	if ((3 <= argc) && (argc <= 5))
	{
		// When the number of arguments is between 3 (from 0 to 2) and 5 (from 0 to 4):
		//	- argv[0] contains the full path to the executable file (i.e. the ".exe") of this application
		//	- argv[1] contains the full path to the PDDL Domain file
		//	- argv[2] contains the full path to the PDDL Problem file
		//	- argv[3] may contain a command-line option
		//	- argv[4] may contain a command-line option

		// Check the options first
		bool options[2] = {false /* -m */, false /* -r */};		// Note the alphabetical order between the options the user can activate
		for (unsigned char i = 3; i < argc;)
		{
			if ('-' == argv[i][0])
				switch (argv[i][1])
				{	// Following cases are in alphabetical order
					case 'm':
						{
							options[0] = true;
							++i;
							break;
						}
					case 'r':
						{
							// This option only toggles the printing of runtimes (see calls to RT.Start() and RT.Stop() below);
							options[1] = true;
							++i;
							break;
						}
					default:
						{
							std::cerr << "Option -" << argv[i][1] << " is unknown." << std::endl;

							return ERROR_UNKNOWN_OPTION;
						}
				}
			else // ('-' != argv[i][0])
			{
				std::cerr << "Option " << argv[i] << " must be preceded by '-'." << std::endl;

				return ERROR_DASH_PREFIX_MISSING;
			}
		}

		// 3 runtimes shall be measured:
		//		RT[0] <=> How long was the reading the PDDL Domain file?
		//		RT[1] <=> How long was the reading the PDDL Problem file?
		//		RT[2] <=> How long was the searching for a solution plan?
#if defined(_MSC_VER)	// _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
		SPHG::Runtimes_T_hrc<3> RT;
#else					// When it's not MSVC++ and maybe not under windows (e.g. unix)
		SPHG::Runtimes_T_clock_t<3> RT;
#endif					// _MSC_VER
		if (options[1] /* -r */)
		{
			// Set up the output format of the time measures
			std::cout.setf(std::ios_base::fixed /* fixed-decimal notation */, std::ios_base::floatfield /* fixed || scientific (with exponent field) */);
			std::cout.precision(RT.Precision() + 1);
		}

		Domain::StoreHouse thePDDLLocalStore;	// The Planning store house: domains, problems, searches, ...

		// Runtime of the reading of the PDDL Domain file is always measured...
		RT.Start();
		DomainFile df(argv[1] /* The Domain filename is the first argument */, thePDDLLocalStore);
		RT.Stop();

		// ... but printed ONLY WHEN the corresponding option is ON
		if (options[1] /* -r */)
			std::cout << "Reading of the PDDL Domain file took " << RT.Runtime(0) << " second(s)." << std::endl;
 
		// Check whether the PDDL Domain file has been correctly read
		if (0 == thePDDLLocalStore.size() || df.ReadingFailed())
		{
			std::cerr << "PDDL Domain file " << argv[1] << " is incorrect." << std::endl;

			return ERROR_READING_DOMAIN_FILE;
		}

		if (options[0] /* -m */) df.SizeOf();

		// Runtime of the reading of the PDDL Problem file is always measured...
		RT.Start();
		ProblemFile pf(argv[2], thePDDLLocalStore, df.ReadingFailed());
		RT.Stop();

		// Check whether the PDDL Problem file has been correctly read
		if (pf.ReadingFailed())
		{
			std::cerr << "PDDL Problem file " << argv[2] << " is incorrect." << std::endl;

			return ERROR_READING_PROBLEM_FILE;
		}

		// Register the correct PDDL problem file
		Domain::HProblem pb = df.AddProblem(pf);

		// ... but printed ONLY WHEN the PDDL Problem file is correct and the corresponding option is ON
		if (options[1] /* -r */)
			std::cout << "Reading of the PDDL Problem file took " << RT.Runtime(1) << " second(s)." << std::endl;

		if (options[0] /* -m */) (df.GettheProblem(0))->SizeOf(argv[2]);

		if (pf.SearchUnneeded())
		{
			std::cout << "No search is needed: PDDL Problem file " << argv[2] << " declares a trivial problem." << std::endl;

			return SEARCH_IS_NOT_NEEDED;
		}
		else // (! pf.SearchUnneeded())
		{
			// Create the structure for the search
			StateSpaceForwardChaining s(df, pb);

			// Search runtime is ALWAYS measured...
			RT.Start();
			bool aSolutionWasFound = s.BreadthFirst(options[0]);
			// bool aSolutionWasFound = s.BestFirst();
			RT.Stop();

			// Search runtime is ALWAYS measured... but printed ONLY WHEN the corresponding option is ON
			if (options[1] /* -r */)
				std::cout << "Planning took " << RT.Runtime(2) << " second(s)." << std::endl;

			// Always print memory consumption during search (tells us the number of predicates)
			std::cout << "Total memory size is greater than " << s.SizeOf() << " bytes." << std::endl << std::endl;
		
			// Print search results
			if (aSolutionWasFound)
			{
				s.Put(std::cout /* Where to print */, s.GetthePlanFound(), "    " /* Tabulation to align :actions*/);

				return SEARCH_STOPS_FOUND_A_SOLUTION;
			}
			else // (! aSolutionWasFound) <=> search found no solution
			{
				std::cout << "No solution found." << std::endl;

				return SEARCH_ENDS_WITH_NO_SOLUTION;
			}
		}
	}
	else if (2 == argc)
	{
		// When this console application receives 2 arguments:
		//	- argv[0] contains the full path to the executable file (i.e. ".exe") of this application
		//  - argv[1] contains the help option
		
		PDDL::Identifier Option = argv[1];
		if (("-h" == Option) || ("-help" == Option))
		{
			std::cout << std::endl
				<< "The command-line for this application is:" << std::endl
				<< "  \"Planner  a_PDDL_Domain_FileName  a_PDDL_Problem_Filename  [-m][-r]\"" << std::endl
				<< "Option -m prints memory usage (domain, problem and search)." << std::endl
				<< "Option -r prints runtimes (reading of the domain and problem PDDL files, and search)." << std::endl
				<< "This Planner accepts the following PDDL :requirements (in alphabetical order):" << std::endl
				<< "   - :action-costs" << std::endl
				<< "   - :negative-preconditions" << std::endl
				<< "   - :strips" << std::endl
				<< "   - :types (although types are accepted in a PDDL file, they are ignored)" << std::endl
				<< std::endl;

			return HELP;
		}
		else // (("-h" != Option) && ("-help" != Option))
		{
			std::cerr << "Option " << Option << " is unknown." << std::endl;

			return ERROR_UNKNOWN_OPTION;
		}
	}
	else // ((argc < 2) || (5 < argc))
	{
		std::cerr << "Incorrect number of arguments: " << (argc - 1) /* Don't count argv[0] (cf. line 79 or line 221) */ << "." << std::endl;

		std::cerr << std::endl
			<< "The command-line for this application is:" << std::endl
			<< "  \"Planner  a_PDDL_Domain_FileName  a_PDDL_Problem_Filename  [-m][-r]\"" << std::endl
			<< "Option -m prints memory usage (domain, problem and search)." << std::endl
			<< "Option -r prints runtimes (reading of the domain and problem PDDL files, and search)." << std::endl
			<< "This Planner accepts the following PDDL :requirements (in alphabetical order):" << std::endl
			<< "   - :action-costs" << std::endl
			<< "   - :negative-preconditions" << std::endl
			<< "   - :strips" << std::endl
			<< "   - :types (although this application accepts types in a PDDL file, they are ignored)" << std::endl
			<< std::endl;

		return ERROR_WRONG_NUMBER_OF_ARGUMENTS;
	}
}
//
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
// =============================================================================================== End of file "main_BCP.cpp"