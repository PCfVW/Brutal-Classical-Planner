// ================================================================================================ Beginning of file "Runtimes.h"
// Copyright (c) 2010-2018, Eric Jacopin, ejacopin@ymail.com
// ================================================================================================ File Content
//
// This file contains the following sections and subsections:
//	- File History                  (Line 17)
//  - Inclusion of files            (Line 131)
//	- Namespace declaration         (Line 142)
//	- Comments                      (Line 147)
//	- Classes declarations          (Line 218)
//	- Runtimes_timeb                (Line 218)
//	- Runtimes_clock_t              (Line 295)
//	- Runtimes_T_clock_t            (Line 347)
//	- Runtimes_hrc                  (Line 400)
//	- Runtimes_T_hrc                (Line 491)
//
// ================================================================================================ File history
//
// [Author, Created, Last modification] = [Éric JACOPIN, 28/FEV/2010, 15/DEC/2017]
//	Compilers:	MSVC++ 2003, 2008, 2010, 2012, 2013 and 2017 (NOT TESTED WITH 2005); GCC 3.2
//
//      - (dev 11): C++11 ------------------------------------------------------------- 15/DEC/2017
//          ."0 error, 0 warning" with 2017
//      - (dev 10): Precision --------------------------------------------------------- 05/OCT/2012
//          .Precision/0 now returns an exponent value:
//				.returns 0 for second == 10^0 == 1
//				.returns -3 for milli-second == 10^(-3) == 0.001
//				.returns -6 for micro-second == 10^(-6) == 0.000001
//				.(new) Example 1:
//					// Set up the output format of the time measures
//					std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
//					if (-6 == r.Precision())
//						std::cout.precision(8);		// 8-digits for the fractional part
//					else if (-3 == r.Precision())
//						std::cout.precision(3);		// 3-digits for the fractional part
//				.(new) Example 2:
//					// Set up the output format of the time measures
//					std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
//					// (note the substraction as Precision/0 now returns a negative number)
//					std::cout.precision(1 - r.Precision());
//          .New file guard constant (_SPHG_TOOLS_RUNTIMES_H)
//      - (dev 9): Time stamps -------------------------------------------------------- 06/JUN/2012
//          .Comments corrected and updated
//          .(Fix) Runtimes_timeb.Precision/0 returns 1 (seconds)
//				(previously was 3 -- milli-seconds)
//          .Runtime/0 no longer checks whether its parameter is a valid measure number
//          .LastRuntime/0, Precision/0, Runtime/1 and TimeStamp/0 now return a const
//          .Added private typedef Measure for the std::pair of time stamps
//          .Added operation GetCounterFrequency/0 which returns the number of ticks of
//				the counter used by the class
//          .Added operation TimeStamp/0 which returns the current time without storing it
//      - (dev 8): VC++ 2003 & 2010 --------------------------------------------------- 13/MAR/2012
//          ."0 error, 0 warning" with 2010
//          .Changes in order to get a "0 error, 0 warning" with Visual Studio 2003:
//				.Comments updated
//				. '>>' ending templates changed to '> >' 
//				._timeb and _time_t respectively changed to __timeb64 and __time64_t
//				."0 error, 0 warning" with 2003 AND 2008
//      - (dev 7): Templates ---------------------------------------------------------- 02/NOV/2011
//          .Comments improved
//          .Classes renamed as follows:
//				Runtimes_timeb, Runtimes_clock_t, Runtimes_T_clock_t, Runtimes_hrc and
//				Runtimes_T_hrc
//          .Added template classes Runtimes_T_clock_t and Runtimes_T_hrc
//				These two classes provide a fix number (which is a template parameter)
//					of measures whereas the other classes uses std:vector to store the
//					measures
//				The template parameter is here declared as an unsigned char:
//					there can be at most 127 measures
//      - (dev 6): Micro-seconds  ----------------------------------------------------- 03/SEP/2011
//          .Comments improved
//          .Both Runtimes_b::Runtime and Runtimes_b::LastRuntime now return a double
//          .Added operation Precision/0 which informs about the precision of the measures:
//				.Precision/0 returns 3 when the precision is up to the milli-second
//				.Precision/0 returns 6 when the precision is up to the micro-second
//				.Example 1:
//					// Set up the output format of the time measures
//					std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
//					if (6 == r.Precision())
//						std::cout.precision(8);		// 8-digits for the fractional part
//					else if (3 == r.Precision())
//						std::cout.precision(3);		// 3-digits for the fractional part
//				.Example 2:
//					// Set up the output format of the time measures
//					std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
//					 // (1 + the Precision())-digits for the fractional part
//					std::cout.precision(r.Precision() + 1);
//          .Added namespace SPHG for all classes of this file
//          .Added class Runtimes_hrc which provides precision up to the micro-second
//				.The frequency of the high resolution counter is 3579545 ticks per seconds
//					as measured on an intel-based Windows machine.
//				.(1 / 3579545) is about 279 * 10^-7 which gives a precision up to the micro-second
//				.Added operation HighResolutionCounterIsSupported/0 to check whether the
//					hardware supports such a high resolution counter
//          .This file must be in the include directory of MSC++ and thus included with < and >
//          .Removed conditional compiling data in order to be able to produce a DLL
//      - (dev 5): DLL ---------------------------------------------------------------- 07/JUL/2011
//          .Added conditional compiling data so as only MSC++ compiles _timeb and _ftime64 
//          .Added conditional compiling data in order to be able to produce a DLL
//          .LastRuntime/0 is now inlined
//      - (dev 4): Improvements ------------------------------------------------------- 03/APR/2011
//          .Added Class Runtimes_clock which computes runtimes from calls to clock()
//          .Class Runtimes renamed Runtimes_b
//          ._ftime64_s changed to _ftime64 (warning 4996 is disabled through a #pragma)
//          .Comments updated according to the above changes
//      - (dev 3): Improvements ------------------------------------------------------- 02/FEB/2011
//          .Added LastRuntime/0 to return the runtime of the last measure
//          .Added default value 0 (zero) to Runtime/1's parameter
//          .Runtime/1 is now a float-based const operation
//          .Fixed 2 bugs in Runtime/2 when last-milli < first-milli
//          .Added Clear/0 to clear theMeasures
//          .Successive measures are now possible through the use of a vector
//          .Comments improved
//      - (dev 2): Improvements ------------------------------------------------------- 21/JUN/2010
//          .The parameter of Runtime/1 is now passed as a const reference
//          .Comments added
//      - (dev 1): Basic functionalities ---------------------------------------------- 19/MAR/2010
//          .Recording of departure and arrival times
//          .String-based computation of the time between departure and arrival times
// ------------------------------------------------------------------------------------------------
#ifndef _SPHG_TOOLS_RUNTIMES_H
#define _SPHG_TOOLS_RUNTIMES_H

#if defined(_MSC_VER)               // _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#pragma once                    // When compiling, this file will be included (opened) only once by the Microsoft C++ compiler 
	#pragma warning(push)           // Stores the current warning state for all warnings
	#pragma warning(disable : 4996) // 4996 <=> _CRT_SECURE_NO_WARNINGS is now OFF (NO WARNING for the use of _ftime64); see warning(pop) at the end of this file, line 588
#endif                              // _MSC_VER


/////////////////////////////////////////////////////////////////////////////////////////////////// Inclusion of files
//
#if defined(_MSC_VER)               // _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#include <sys/timeb.h>          // Visibility for __time64_t, __timeb64 and _ftime64/1 in class Runtimes_timeb
	#include <windows.h>            // Visibility for LARGE_INTEGER, LONGLONG, QueryPerformanceFrequency/1 and QueryPerformanceCounter/1
#endif                              // _MSC_VER

#include <time.h>                   // Visibility for time_t, clock_t, clock/0 and CLOCKS_PER_SEC
#include <vector>                   // The Store House for successive runtime measures in classes Runtimes_timeb, Runtimes_clock and Runtimes_hrc


/////////////////////////////////////////////////////////////////////////////////////////////////// Namespace declaration
//
namespace SPHG {        // Simple Planning, Happy Gaming!


/////////////////////////////////////////////////////////////////////////////////////////////////// Comments
//
// System routines such as those in <time.h> usually return the absolute elapsed time, not the
// relative (i.e. between two time stamps) elapsed time: the so-called run time.
//
// The purpose of this file is to provide several classes to measure run times; template classes are
// provided when the number of measures is known in advance; second (10^0), milli- (10^-3) and micro-
// seconds (10^-6) precisions are provided on windows; milli-seconds precision is provided on Linux.
//
//      Class           Template        OS          Measuring type   Precision  Included Header
// ---------------------------------------------------------------------------------------------
//  Runtimes_timeb        No        Windows          __timeb64         10^0      <sys/timeb.h>
//  Runtimes_clock_t      No        Unix + Windows   clock_t           10^-3     <time.h>
//  Runtimes_T_clock_t    Yes       Unix + Windows   clock_t           10^-3     <time.h>
//  Runtimes_hrc          No        Windows          LARGE_INTEGER     10^-6     <windows.h>
//  Runtimes_T_hrc        Yes       Windows          LARGE_INTEGER     10^-6     <windows.h>
//
// All classes of this files are enclosed into the SPHG namespace.
//
// All classes possess the following operations (alphabetical order):
//      - Clear/0 ---------------> void     // Forgets all the previous couples of Start and Stop
//      - GetCounterFrequency/0             // Returns the number of ticks per second for the related counter
//      - LastRuntime/0 ---------> double   // Returns the time difference between the very last couple of Start and Stop
//      - Precision/0 -----------> short    // Returns 1 for seconds, 3 for milli-seconds and 6 for micro-seconds
//      - Runtime/1	-------------> double   // Returns the time difference (cf. Precision/0) between a given couple of Start and Stop
//      - Start/0 ---------------> void     // Records the current value of a specific counter
//      - Stop/0 ----------------> void     // Records the current value of a specific counter and pairs it, as a measure, with that of the last call to Start/0
//      - TimeStamp/0                       // Returns the current value of a specific counter (cf. lines 150-159)
//
// A call to the operation Start/0 performs the first time measure. A call to the operation Stop/0
// performs the second time measure; Stop/0 does not check whether Start/0 has been previously
// called. It is the user's responsability to first call Start/0 BEFORE calling Stop/0.
//
// Successive calls to Start/0 and then Stop/0 perform successive couples of measures which are
// recorded.
//
// Successive couples of measures are numbered in the order they are made; 0 (i.e. zero) is the
// number of the first measure. The user can later compute a run time referring to a couple of 
// measures by calling Runtime/1 with the desired measure number as parameter.
//
// A call to Clear/0 clears all the couples of measures. After a call to Clear/0, the next couple
// of measures shall be numbered 0 (i.e. zero).
//
// LastRuntime/0 computes the run time from the last couple of measures.
//
// TimeStamp/0 reads the current time, returns it and does NOT record it as a measure; the return type
// varies according to the class:
//
//     Class                     Return type of TimeStamp/0
// --------------------------------------------------------
//  Runtimes_timeb                      __timeb64
//  Runtimes_clock_t                    double
//  Runtimes_T_clock_t                  double
//  Runtimes_hrc                        long double
//  Runtimes_T_hrc                      long double
//
// GetCounterFrequency/0 returns the number of ticks of the counter according to the class:
//
//     Class                 Return type of GetCounterFrequency/0
// --------------------------------------------------------------
//  Runtimes_timeb                      unsigned int
//  Runtimes_clock_t                    unsigned int
//  Runtimes_T_clock_t                  unsigned int
//  Runtimes_hrc                        LONGLONG
//  Runtimes_T_hrc                      LONGLONG
//
// Moreover, classes Runtimes_hrc and Runtimes_T_hrc provide operation HighResolutionCounterIsSupported/0
// in order to test whether the hardware (where compiling happens) supports a high resolution counter.
// ------------------------------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////////////////////////// Classes declarations
//
// ------------------------------------------------------------------------------------------------ Runtimes_timeb
//
#if defined(_MSC_VER)	// __time64_t, __timeb64 and _ftime64 are specific to MS VC++
class Runtimes_timeb {
	// ----- Types
	private:
		typedef	std::pair<__timeb64, __timeb64>	Measure;

	// ----- Properties
	private:
		std::vector<Measure> theMeasures;   // Pairs of first and last measures so as to compute run times
		__timeb64 theFirstMeasure;          // Set by Start/0
		__timeb64 theLastMeasure;           // Set by Stop/0

	// ----- Constructors
	public:
		Runtimes_timeb() {}

	// ----- Accessors
	public:
		inline const short Precision() const { return (0 /* 10^(0) = second */); }
		inline const unsigned int GetCounterFrequency() const { return 1; }

	// ----- Operations
	public:
		inline const __timeb64 TimeStamp () {
			_ftime64(&theFirstMeasure);

			return theFirstMeasure;
		}

		inline void Start() {
			_ftime64(&theFirstMeasure);
		}

		inline void Stop() {
			_ftime64(&theLastMeasure);
			theMeasures.push_back(Measure(theFirstMeasure, theLastMeasure));
		}

		inline void Clear() {
			theMeasures.clear();
		}
		
		inline const double LastRuntime() const {
			return Runtime(theMeasures.size() - 1);
		}
		
		const double Runtime(std::vector<Measure>::size_type aMeasure = 0) const {
			// Computation of the number of seconds (which is never negative)
			__time64_t seconds_difference = theMeasures[aMeasure].second.time - theMeasures[aMeasure].first.time;
			// Computation of the number of milli-seconds (which can be negative, see below)
			__time64_t milliseconds_difference = (theMeasures[aMeasure].second.millitm - theMeasures[aMeasure].first.millitm);

			// Adjust computations when number of milli-seconds is negative.
			// As an example, assume the following measures:
			//	- the first measure is 1 second and 891 milli-seconds,
			//	- the second measure is 2 seconds and 165 milli-seconds.
			// Then the number of seconds computed above (2 - 1) = 1 whereas the correct value is 0
			// Then the number of milli-seconds computed above is (165 - 891) = - 726 ms whereas the correct value is 1165 - 895 = 274 ms.
			// So, first, let's see whether we need to adjust the result:
			if (milliseconds_difference < 0)
			{
				// Get the correct number of seconds
				--seconds_difference;
				// Get the correct number of milli-seconds (1000 + 165 - 891 = 274 ms)
				milliseconds_difference += 1000;
			}

			// Returning the measured time in the floating format seconds.milliseconds
			return double(seconds_difference + (milliseconds_difference * 0.001000f));
		}
};
#endif					// _MSC_VER
//
// ------------------------------------------------------------------------------------------------ Runtimes_clock_t
//
class Runtimes_clock_t {
	// ----- Types
	private:
		typedef	std::pair<clock_t, clock_t>	Measure;

	// ----- Properties
	private:
		std::vector<Measure> theMeasures;   // Pairs of first and last measures so as to compute run times
		clock_t theStart;                   // Set by Start/0
		clock_t theFinish;                  // Set by Stop/0

	// ----- Constructors
	public:
		Runtimes_clock_t() {}

	// ----- Accessors
	public:
		inline const short Precision() const { return (-3 /* 10^(-3) = milli-second */); }
		inline const unsigned int GetCounterFrequency() const { return CLOCKS_PER_SEC; }

	// ----- Operations
	public:
		inline const double TimeStamp() const {
			return (clock() / (double) (CLOCKS_PER_SEC));
		}

		inline void Start() {
			theStart = clock();
		}

		inline void Stop() {
			theFinish = clock();
			theMeasures.push_back(Measure(theStart, theFinish));
		}

		inline void Clear() {
			theMeasures.clear();
		}
		
		inline const double LastRuntime() const {
			return Runtime(theMeasures.size() - 1);
		}
		
		const double Runtime(std::vector<Measure>::size_type aMeasure = 0) const {
			// Returning the measured time in the double format seconds.milliseconds
			return ((theMeasures[aMeasure].second - theMeasures[aMeasure].first) / (double) (CLOCKS_PER_SEC));
		}

};
//
// ------------------------------------------------------------------------------------------------ Runtimes_T_clock_t
//
template<unsigned char n = 1> class Runtimes_T_clock_t {
	// ----- Types
	private:
		typedef	std::pair<clock_t, clock_t>	Measure;

	// ----- Properties
	private:
		Measure theMeasures[n];
		unsigned char theNumberOfMeasures;  // Pairs of first and last measures so as to compute run times
		clock_t theStart;                   // Set by Start/0
		clock_t theFinish;                  // Set by Stop/0

	// ----- Constructors
	public:
		Runtimes_T_clock_t() : theNumberOfMeasures(0) {}

	// ----- Accessors
	public:
		inline const short Precision() const { return (-3 /* 10^(-3) = milli-second */); }
		inline const unsigned int GetCounterFrequency() const { return CLOCKS_PER_SEC; }

	// ----- Operations
	public:
		inline const double TimeStamp() const {
			return (clock() / (double) (CLOCKS_PER_SEC));
		}

		inline void Start() {
			theStart = clock();
		}

		inline void Stop() {
			theFinish = clock();
			theMeasures[theNumberOfMeasures++] = Measure(theStart, theFinish);
		}

		inline void Clear() {
			theNumberOfMeasures = 0;
		}
		
		inline const double LastRuntime() const {
			return Runtime(theNumberOfMeasures - 1);
		}
		
		const double Runtime(std::vector<Measure>::size_type aMeasure = 0) const {
			// Returning the measured time in the double format seconds.milliseconds
			return ((theMeasures[aMeasure].second - theMeasures[aMeasure].first) / (double) (CLOCKS_PER_SEC));
		}

};
//
// ------------------------------------------------------------------------------------------------ Runtimes_hrc
//
#if defined(_MSC_VER)	// LARGE_INTEGER and LONGLONG are specific to MS VC++
class Runtimes_hrc {
	// ----- Types
	private:
		typedef	std::pair<LONGLONG, LONGLONG>	Measure;

	// ----- Properties
	private:
		bool HighResolutionCounterSupported;    // True when the hardware supports a high-resolution performance counter
		LONGLONG thePerformanceFrequency;       // (== 3579545) The frequency of the high-resolution performance counter; set to zero by the constructor of this class when no such counter exists

		std::vector<Measure> theMeasures;       // Pairs of first and last measures so as to compute run times
		LARGE_INTEGER theStart;                 // Queried value of the high-resolution performance counter, Set by Start/0
		LARGE_INTEGER theFinish;                // Queried value of the high-resolution performance counter, Set by Stop/0

	// ----- Constructor
	public:
		Runtimes_hrc() {
			LARGE_INTEGER aPerformanceFrequency;
			
			// Check the frequency of the high-resolution performance counter
			aPerformanceFrequency.QuadPart = 0;
			QueryPerformanceFrequency(&aPerformanceFrequency);

			// The hardware supports a high-resolution performance counter when the frequency is NOT zero
			HighResolutionCounterSupported = (0 < aPerformanceFrequency.QuadPart);

			// Remember the frequency, whatever the value (possibly zero)
			thePerformanceFrequency = aPerformanceFrequency.QuadPart;
		}

	// ----- Accessors
	public:
		inline bool HighResolutionCounterIsSupported() const { return HighResolutionCounterSupported; }
		inline const short Precision() const { return (-6 /* 10^(-6) = micro-second */); }
		inline const LONGLONG GetCounterFrequency() const { return thePerformanceFrequency; }

	// ----- Operations
		inline void Clear() {
			theMeasures.clear();
		}

		inline const long double TimeStamp() {
			// IF the query of the performance counter fails THEN
			//		the counter value == 0
			//		checking of this zero counter value is left to the user of this function
			theStart.QuadPart = 0;

			QueryPerformanceCounter(&theStart);

			return (theStart.QuadPart / (long double) (thePerformanceFrequency));
		}

		inline void Start() {
			// IF the query of the performance counter fails THEN
			//		the counter value == 0
			//		checking of this zero counter value is made in Runtime/1
			theStart.QuadPart = 0;

			QueryPerformanceCounter(&theStart);
		}

		inline void Stop() {
			// IF the query of the performance counter fails THEN
			//		the counter value == 0
			//		checking of this zero counter value is made in Runtime/1
			theFinish.QuadPart = 0;

			QueryPerformanceCounter(&theFinish);

			theMeasures.push_back(Measure(theStart.QuadPart, theFinish.QuadPart));
		}
		
		inline const long double LastRuntime() const {
			return Runtime(theMeasures.size() - 1);
		}

		const long double Runtime(std::vector<Measure>::size_type aMeasure = 0) const {
			if ((0 < theMeasures[aMeasure].second) && (0 < theMeasures[aMeasure].first))
				// None of the query failed but we don't check whether the second measure is NOT LESS than the first measure
				// Compute and report the run time
				return ((theMeasures[aMeasure].second - theMeasures[aMeasure].first) / (long double) (thePerformanceFrequency));
			else
				// Report that at least one of the two queries failed or that aMeasure does not exist
				return 0;
		}

};
//
// ------------------------------------------------------------------------------------------------ Runtimes_T_hrc
//
template<unsigned char n = 1> class Runtimes_T_hrc {
	// ----- Types
	private:
		typedef	std::pair<LONGLONG, LONGLONG>	Measure;

	// ----- Properties
	private:
		bool HighResolutionCounterSupported;    // True when the hardware supports a high-resolution performance counter
		LONGLONG thePerformanceFrequency;       // (== 3579545) The frequency of the high-resolution performance counter; set to zero by the constructor of this class when no such counter exists

		Measure theMeasures[n];                 // Pairs of first and last measures so as to compute run times
		unsigned char theNumberOfMeasures;      // Allow a fast Clear/0, for instance
		LARGE_INTEGER theStart;                 // Queried value of the high-resolution performance counter, Set by Start/0
		LARGE_INTEGER theFinish;                // Queried value of the high-resolution performance counter, Set by Stop/0

	// ----- Constructor
	public:
		Runtimes_T_hrc() : theNumberOfMeasures(0) {
			LARGE_INTEGER aPerformanceFrequency;
			
			// Check the frequency of the high-resolution performance counter
			aPerformanceFrequency.QuadPart = 0;
			QueryPerformanceFrequency(&aPerformanceFrequency);

			// The hardware supports a high-resolution performance counter when the frequency is NOT zero
			HighResolutionCounterSupported = (0 < aPerformanceFrequency.QuadPart);

			// Remember the frequency, whatever the value (possibly zero)
			thePerformanceFrequency = aPerformanceFrequency.QuadPart;
		}

	// ----- Accessors
	public:
		inline bool HighResolutionCounterIsSupported() const { return HighResolutionCounterSupported; }
		inline const short Precision() const { return (-6 /* 10^(-6) = micro-second */); }
		inline const LONGLONG GetCounterFrequency() const { return thePerformanceFrequency; }

	// ----- Operations
		inline void Clear() {
			theNumberOfMeasures = 0;
		}

		inline const long double TimeStamp() const {
			// IF the query of the performance counter fails THEN
			//		the counter value == 0
			//		checking of this zero counter value is left to the user of this function
			theStart.QuadPart = 0;

			QueryPerformanceCounter(&theStart);

			return (theStart.QuadPart / (long double) (thePerformanceFrequency));
		}

		inline void Start() {
			// IF the query of the performance counter fails THEN
			//		the counter value == 0
			//		checking of this zero counter value is made in Runtime/1
			theStart.QuadPart = 0;

			QueryPerformanceCounter(&theStart);
		}

		inline void Stop() {
			// IF the query of the performance counter fails THEN
			//		the counter value == 0
			//		checking of this zero counter value is made in Runtime/1
			theFinish.QuadPart = 0;

			QueryPerformanceCounter(&theFinish);
			theMeasures[theNumberOfMeasures++] = Measure(theStart.QuadPart, theFinish.QuadPart);
		}
		
		inline const long double LastRuntime() const {
			return Runtime(theNumberOfMeasures - 1);
		}

		const long double Runtime(std::vector<Measure>::size_type aMeasure = 0) const {
			if ((0 < theMeasures[aMeasure].second) && (0 < theMeasures[aMeasure].first) && (aMeasure < theNumberOfMeasures))
				// None of the query failed but we don't check whether the second measure is NOT LESS than the first measure
				// Compute and report the run time
				return ((theMeasures[aMeasure].second - theMeasures[aMeasure].first) / (long double) (thePerformanceFrequency));
			else
				// Report that at least one of the two queries failed or that aMeasure does not exist: a negative value is returned
				// It reflects that no measure corresponds to the actual incorrect value of aMeasure
				return (0 - aMeasure);
		}

};
#endif                  // _MSC_VER

}                       // namespace SPHG
//
// ------------------------------------------------------------------------------------------------
//
#if defined(_MSC_VER)       // _MSC_VER reports the major and minor versions of the Microsoft C++ compiler
	#pragma warning(pop)    // Pops the last warning state pushed onto the stack : C4996 is ON again
#endif
//
// ------------------------------------------------------------------------------------------------
//
#endif                      // _SPHG_TOOLS_RUNTIMES_H
// ================================================================================================ End of file "Runtimes.h"