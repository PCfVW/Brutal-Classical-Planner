// ================================================================================================ Beginning of file "Readme (PDDL Testing).txt"
// Copyright (c) 2012-2018, Eric Jacopin, ejacopin@ymail.com
// ------------------------------------------------------------------------------------------------
Please recall that you should not hesitate to contact me!

This directory, "PDDL Testing/" is a test directory for the BCP planner; it contains:
   - This file
   - One .exe file of the "BCP (dev 14.3)" planner
   - One .PDF file ("BNF 3.1.PDDL.pdf") which details the Complete BNF description of PDDL 3.1
   - Several .pddl domains files (domain constants, predicates and operators)
   - Several .pddl problems files (initial situations and goals)
   - A windows command file ("Test.bat") which provides the PDDL files to the BCP planner
   - A windows command file ("GetTheConsole.bat") which calls the windows terminal in this directory when double-clicked

Quick start:
   1. Double-click on "GetTheConsole.bat" to open a windows terminal
   2. Type Test "BCP (dev 14.3).exe" in the windows terminal to run the tests
   
Here are the PDDL files (domains and their problems) used in "Test.bat":

================================================================
      Domains                         Problems
----------------------------------------------------------------
 blocks-wordl.pddl           Sussman-Anomaly.problem.pddl
                             Identity.problem.pddl
 
----------------------------------------------------------------
 logistics.domain.pddl       logistics.problem.att_log3.1p.pddl
                             logistics.problem.att_log3.2p.pddl
                             logistics.problem.att_log3.3p.pddl
                             logistics.problem.att_log3.4p.pddl
                             logistics.problem.att_log3.5p.pddl
                             logistics.problem.att_log3.6p.pddl
                             logistics.problem.att_log3.7p.pddl
                             logistics.problem.att_log3.8p.pddl
 
----------------------------------------------------------------
 my_pom.v8_3.domain.pddl     my_pom.v8_2.problem.pddl
 
----------------------------------------------------------------
 pengi.V3.domain.pddl        plan_auto_7_actions_3_weapons.pddl
 
----------------------------------------------------------------
 sam4.domain.pddl            sam07.problem.pddl

----------------------------------------------------------------
 sam5.domain.pddl            pickandplace-full2.pddl

================================================================

Do no hesitate to give a look at the file "Test.bat" (e.g. open it from Visual Studio) to see how these
PDDL files are given to a planner.

Do not hesitate to give a look at these PDDL files (e.g. open them from Visual Studio) to see
the predicates and actions (domain files) and the initial and goal states (problem files).

Please note that the Visual Studio Console (Release) project of "BCP (dev 14.3)" DOES NOT outputs the
executable file in the "PDDL Testing/" directory. Thus, copy the executable file in this directory for
testing purposes or configure Visual Studio to output the executable file where you want it to be.

In order to run the tests:
   1. Open a Windows terminal and change to the "PDDL Testing/" directory; alternatively, you can 
          double-click on "PDDL Testing/GetTheConsole.bat".
   2. Adjust the properties of the Windows terminal and increase buffer values and window sizes.
          (Right-Click on the window title and then "properties")
          Here are the settings I use in the configuration panel:
				- for the screen buffer: 100 x 600 (width x height)
				- for the window sizes: 100 x 60 (width x height)
		  600 for the height of the screen buffer is enough to keep all the results from Test.bat.
   3. Make sure the executable is in this directory and then type:
          
		                   Test "BCP (dev 14.3).exe"
		  
		  to run the tests. These tests are detailed in the windows command file "Test.bat".

Depending on your computer, the total run time may be up to a one minute or more. Be patient.

// ================================================================================================ End of file "Readme (PDDL Testing).txt"