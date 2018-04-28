// ================================================================================================ Beginning of file "Readme (BCP (dev 14.3)).txt"
// Copyright (c) 2012-2018, Eric Jacopin, ejacopin@ymail.com
// ------------------------------------------------------------------------------------------------

   Hello!

First of all, thank you for considering this planning work and, should you have any question about it,
do not hesitate to contact me!

This repo contains the C++ code related to the chapter titled "Optimizing Practical Planning for Game AI"
of the Game AI Pro 2 book ((c) 2015 CRC Press):

https://www.crcpress.com/product/isbn/9781482254792

This archive contains a PDF version of the chapter and all the files of a Visual Studio C++ 2017
((c) 2017 Microsoft Corporation) project.

This project implements a forward-chaining state-space planner which gets planning problems written in a
subset of PDDL 3.1 (cf. "PDDL Testing/BNF 3.1.PDDL.PDF") and tries to build a plan solving these problems
using breadth-first search.

This planner has been optimized for small problems so that, for these small problems, it's very fast and
its memory consumption is very low; for instance, run time and memory consumption for the sussman anomaly
on a (MSI GS63VR 7RG) laptop is 51 micro-seconds and 1079 bytes, respectively.

You'll find the sources of the project in the "Headers & Sources" folder, and all about testing
in the "PDDL Testing" folder (including a "Readme (PDDL Testing)" file) where there are several planning
domains and problems. The remaining folders are as usual for a Visual Studio project.

This project is given AS IS. There certainly are bugs. And some features certainly are missing.

But the whole project is the result of several iterations: I wrote more than 20 planners since the
beginning of the 90s and this one is the result of many tuning sessions and many optimization profiles.
However, as usual with somebody else's code, be careful!

:-)

If you need to adapt the code for your project, want to reuse part of it or..., then do not hesitate
to contact me! I'll be happy not only to know about it but also to help you.

Double-click on the "BCP (dev 14.3).sln" file in this folder and enjoy!

    Bien sincèrement,

-- Éric Jacopin.
// ================================================================================================ End of file "Readme (BCP (dev 14.3)).txt"