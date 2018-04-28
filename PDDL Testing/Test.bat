@echo off
rem   ======================================================================
rem                       BCP     DOS    COMMAND    FILE
rem
rem          Copyright (c) 2012-2017 Eric Jacopin, ejacopin@ymail.com
rem   ======================================================================
rem
rem	  Purpose: Testing the BCP planning system against several PDDL files.
rem
rem   The .exe of the BCP planning system must be the first and only 
rem   parameter of this command file.
rem
rem   To execute this command file, open a DOS Command Window, change to
rem   the appriate directory and type (adjust the name of your executable,
rem   if necessary):
rem
rem                     Test "BCP (dev 14.2).exe"
rem
rem   ======================================================================
rem   Version history:
rem
rem     [June 9th, 2015]
rem         .Bug fix: missing, wrong or mispelled application filename is
rem             now correctly handled with a correct error message.
rem             
rem     [December  3rd, 2012]
rem         .Bug fix: %1 is now expanded with %~1 to remove quotes; use of
rem             %1 entailed wrong detection of spaces in planner filename.
rem             Planner filenames without spaces (e.g. "BCP12.exe" instead
rem             of "BCP 12.exe") would work.
rem
rem     [November 20th, 2012]
rem         .Added error case when the executable is not given
rem             (user types "Test", forgetting the application filename)
rem         .Added error case when the executable does not exist
rem             (missing, wrong or mispelled application filename)
rem         .In the starting banner, added the printing of the date and time
rem             of the test
rem
rem     [October 12th, 2012]
rem         .Comments (slightly) improved
rem         .Bug fix: use of character '>' prevented any @echo in the new
rem             termination from 27/09 banner when an error occurred
rem         .New format for the termination banner when an error occurred
rem         .Empty line added previous to this banner
rem
rem     [September 27th, 2012]
rem         .Added and deleted new banner lines
rem         .New termination banners when all tests succeed or else
rem             when one error occurred
rem
rem     [July 27th, 2012]
rem         .Command file now stops when %errorlevel% is < 0 or else > 15
rem         .goto section :ERROR switched with goto section :SUCCESS so that
rem             goto label :SUCCESS could be removed
rem         .Empty lines inserted in several places
rem
rem     [June 19th, 2012]
rem         .New banner using "/" instead of "=" for better readability in
rem             the command window
rem         .goto labels upcased and renamed for better readability
rem         .Bug fix: (:ERROR) the domain file was reported as incorrect
rem             instead of the problem file
rem
rem     [May 1st, 2012]
rem         .Problem files tested in a new order
rem         .Spaces, lines inserted or deleted and variable renaming
rem
rem     [January 22nd, 2012]
rem         .First working version
rem   ======================================================================

setlocal

if "%~1" == ""   (goto ERROR)
if not exist %1  (goto ERROR)

set planner=%1

@echo.
@echo //////////////////////////////////////////////////////////////////////
@echo //
@echo //               Testing %planner%.
@echo //
@echo // %planner% date and time: %~t1
@echo //////////////////////////////////////////////////////////////////////
@echo //
@echo ======================================================================
set d="blocks-world.pddl"
set p="Sussman-Anomaly.problem.pddl"

%planner% %d% %p% -r

if %errorlevel% lss 0   (goto ERROR)
if %errorlevel% gtr 10  (goto ERROR)

@echo.
@echo ======================================================================
set d="sam4.domain.pddl"
set p="sam07.problem.pddl"

%planner% %d% %p% -r

if %errorlevel% lss 0   (goto ERROR)
if %errorlevel% gtr 10  (goto ERROR)

@echo.
@echo ======================================================================
set d="logistics.domain.pddl"
set p="logistics.problem.att_log3.1p.pddl"

%planner% %d% %p% -r

if %errorlevel% lss 0   (goto ERROR)
if %errorlevel% gtr 10  (goto ERROR)

@echo.
@echo ======================================================================
set p="logistics.problem.att_log3.2p.pddl"

%planner% %d% %p% -r

if %errorlevel% lss 0   (goto ERROR)
if %errorlevel% gtr 10  (goto ERROR)

@echo.
@echo ======================================================================
set p="logistics.problem.att_log3.3p.pddl"

%planner% %d% %p% -r

if %errorlevel% lss 0   (goto ERROR)
if %errorlevel% gtr 10  (goto ERROR)

@echo.
@echo ======================================================================
set p="logistics.problem.att_log3.4p.pddl"

%planner% %d% %p% -r

if %errorlevel% lss 0   (goto ERROR)
if %errorlevel% gtr 10  (goto ERROR)

@echo.
@echo ======================================================================
set p="logistics.problem.att_log3.5p.pddl"

%planner% %d% %p% -r

if %errorlevel% lss 0   (goto ERROR)
if %errorlevel% gtr 10  (goto ERROR)

@echo.
@echo ======================================================================
set p="logistics.problem.att_log3.6p.pddl"

%planner% %d% %p% -r

if %errorlevel% lss 0   (goto ERROR)
if %errorlevel% gtr 10  (goto ERROR)

@echo.
@echo ======================================================================
set p="logistics.problem.att_log3.7p.pddl"

%planner% %d% %p% -r

if %errorlevel% lss 0   (goto ERROR)
if %errorlevel% gtr 10  (goto ERROR)

@echo.
@echo ======================================================================
set p="logistics.problem.att_log3.8p.pddl"

%planner% %d% %p% -r

if %errorlevel% lss 0   (goto ERROR)
if %errorlevel% gtr 10  (goto ERROR)

@echo.
@echo ======================================================================
set d="sam5.domain.pddl"
set p="pickandplace-full2.pddl"

%planner% %d% %p% -r

if %errorlevel% lss 0   (goto ERROR)
if %errorlevel% gtr 10  (goto ERROR)

@echo.
@echo ======================================================================
set d="pengi.V3.domain.pddl"
set p="plan_auto_7_actions_3_weapons.pddl"

%planner% %d% %p% -r

if %errorlevel% lss 0   (goto ERROR)
if %errorlevel% gtr 10  (goto ERROR)

@echo.
@echo ======================================================================
set d="my_pom.v8_3.domain.pddl"
set p="my_pom.v8_2.problem.pddl"

%planner% %d% %p% -r

if %errorlevel% lss 0   (goto ERROR)
if %errorlevel% gtr 10  (goto ERROR)



rem   ======================================================================
rem                          ALL    TESTS    SUCCEEDED
rem   ======================================================================

@echo.
@echo ======================================================================
@echo //
@echo //////////////////////////////////////////////////////////////////////
@echo //
@echo //                     All tests succeeded.
@echo //
@echo //////////////////////////////////////////////////////////////////////
@echo.

goto FIN



rem    ======================================================================
rem               AN    ERROR    HAPPENED    DURING    ONE    TEST
rem    ======================================================================

:ERROR

@echo.
@echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
@echo !!
@echo !!                         E  R  R  O  R   !
@echo !! 
@echo !! error number: %errorlevel%

if %errorlevel% lss 0  (@echo !! Invalid termination %errorlevel%)

if %errorlevel% equ 0  (
if "%~1" == "" (
@echo !! No application filename!
) else (
@echo !! %1 is not a valid application filename.
)
)

if %errorlevel% equ 11 (@echo !! %d% is not a valid PDDL Domain file.)
if %errorlevel% equ 12 (@echo !! %p% is not a valid PDDL Problem file.)
if %errorlevel% equ 13 (@echo !! Wrong option.)
if %errorlevel% equ 14 (@echo !! "-" is missing before an option.)
if %errorlevel% equ 15 (@echo !! Incorrect number of arguments.)

if %errorlevel% gtr 15 (@echo !! Invalid termination %errorlevel%)

@echo !!
@echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

goto FIN



rem    ======================================================================
rem                         END    OF    COMMAND    FILE
rem    ======================================================================

:FIN

endlocal
