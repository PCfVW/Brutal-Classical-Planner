// ================================================================================================ Beginning of file "ExternalTypes.h"
// Copyright (c) 2012-2019, Eric Jacopin, ejacopin@ymail.com
// ------------------------------------------------------------------------------------------------
#ifndef _EXTERNAL_TYPES_H
#define _EXTERNAL_TYPES_H

#if defined(_MSC_VER)	// _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#pragma once		// When compiling, this file will be included (opened) only once by the Microsoft C++ compiler 
#endif					// _MSC_VER

#include <memory.h>		// Visibility for memset/3
#include <string>		// For the :action identifier and its :parameters
#include <vector>		// Totally ordered set of :actions

namespace EXTERNAL_TYPES {
	//
	//
	///////////////////////////////////////////////////////////////////////////////////////////////
	//
	//							<int, char*>    L I N K E D    L I S T
	//
	// --------------------------------------------------------------------------------------------
	//

	struct op_str {	// Designed to mimic the op_str struct from FF
		#define	MAX_ARGS_OP		15					// At most 15 arguments

		// ----- Properties
		public:
			int			arity;						// Number of meaningful arguments <= MAX_ARGS_OP
			char*		arg_name[MAX_ARGS_OP];		// Up to MAX_ARGS_OP arguments per :action
			op_str*		op_next;					// Next op_str


		// ----- Constructors
		public:
			op_str() : arity(0), op_next(0) {		// 0-arity means search is over and consequently, that no error occurred
				memset(arg_name, 0, MAX_ARGS_OP * sizeof(char));
			}


		// ----- Destructor
		// No destructor; use clear/0 instead


		// ----- Operations
		public:
			void clear() {
				EXTERNAL_TYPES::op_str* p = this;

				if ((/* Help */ -1 == p->arity) || (0 < p->arity /* Error codes from 1 to 5 */))
				{
					// An error occurred or user asked for help, free the strings allocated to describe the error or the help information
					for (unsigned char a = 1; a < (unsigned char) (*(p->arg_name[0])); ++a)
						free(p->arg_name[a]);

					delete p;	// was allocated with new
				}
				else // (/* Search is over */ 0 == p->arity)
				{
					while (0 != p) // op_str/1 sets op_next to 0, so it can be used to check whether the end of the linked list has been reached
					{
						// Point to the next op_str, free the arguments and delete the current op_str
						EXTERNAL_TYPES::op_str* q = p;
						// Now, point one step further so as to keep the link when deleting q
						p = p->op_next;
						for (unsigned char a = 0; (a < MAX_ARGS_OP && a < q->arity) ; ++a)
							free(q->arg_name[a]);	// each argument was allocated with malloc/1

						delete q;					// each op_str was allocated with new
					}
				}
			}

	};

	//
	//
	///////////////////////////////////////////////////////////////////////////////////////////////
	//
	//							  S T L    I M P L E M E N T A T I O N
	//
	// --------------------------------------------------------------------------------------------
	//

	// Designed to provide a simple alternative to op_str
	typedef std::vector<std::string>										parameters;
	typedef std::pair<std::string /* :action identifier */, parameters>		op;
	typedef std::vector<op>													plan;
	
	//
	//
	///////////////////////////////////////////////////////////////////////////////////////////////
}
#endif					// _EXTERNAL_TYPES_H
// ================================================================================================ End of file "ExternalTypes.h"