// ================================================================================================ Beginning of file "Plan.h"
// Copyright (c) 2012-2018, Eric Jacopin, ejacopin@ymail.com
// ------------------------------------------------------------------------------------------------
#ifndef _SPHG_PLANNER_PLAN_H
#define _SPHG_PLANNER_PLAN_H

#if defined(_MSC_VER)	// _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#pragma once		// When compiling, this file will be included (opened) only once by the Microsoft C++ compiler 
#endif					// _MSC_VER

// ------------------------------------------------------------------------------------------------ Inclusion of files
// STD and STL files
#include <vector>		// STL Store House for the operator signatures (name + instantiated parameters) composing the plan

// Project files
#ifndef _SPHG_PLANNER_STATES_H
	#include "States.h"
#endif

#ifndef _SPHG_PLANNER_OPERATOR_H
	#include "Operator.h"
#endif

// ------------------------------------------------------------------------------------------------ Class definition
class Plan {
	// ----- Types
	public:
		typedef		INTERNAL_TYPES::u8		length_type;			// At most 256 operators in this plan


	// ----- Properties
	private:
		States::iterator					theCurrentState;		// A pointer to theCurrentState in the Store House of current states generated during the StateSpaceForwardChaining
		Operator::HOperatorSignatures		theOperatorSignatures;	// A totally ordered set of operator signatures (name + instantiated parameters), memorized as the operators are applied to theCurrentState


	// ----- Constructors
	public:
		Plan() {}
		Plan(States::iterator theInitialState) : theCurrentState(theInitialState) {}
		Plan(States::iterator aCurrentState, Operator::HOperatorSignatures& someOperatorSignatures) :
			theCurrentState(aCurrentState),
			theOperatorSignatures(someOperatorSignatures) {}


	// ----- Accessors
	public:
		inline States::iterator GettheCurrentState() const { return theCurrentState; }
		inline Plan::length_type Length() const { return (Plan::length_type) theOperatorSignatures.size(); }	// Cardinal of the totally ordered set of operator signatures
		inline const Operator::HOperatorSignatures* GettheOperatorSignatures() const { return &theOperatorSignatures; }


	// ----- Operations
	public:
		inline size_t SizeOf() const {
			size_t s_os = sizeof(theOperatorSignatures);
			for (Plan::length_type i = 0; i < theOperatorSignatures.size(); ++i)
				s_os += sizeof(theOperatorSignatures[i]);

			return (sizeof(theCurrentState) + s_os);
		}
};

#endif	// _SPHG_PLANNER_PLAN_H
// ================================================================================================ End of file "Plan.h"