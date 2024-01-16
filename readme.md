

# Megastructure.

by Ed Deighton (c) 2024.

# Project Goals

Megastructure is a domain specific language for game development that deeply integrates with C++ but enables many powerful features including:

1. A new way of meta programming using a compile time multi stage database to capture and analyse program semantics.

2. The ability to host the simulation in a "creative" mode that enables "live" programming and changing the program as it executes with very fast compilation.

3. The ability to easily bind the program to game engines i.e. Unity such that megastructure can completely control the Unity s cene graph but appropriate work is gracefully offloaded to Unity itself.

4. A novel approach to developer workflow support based on change detector testing and providing HTML live reports about the program.

# Deployment

The Megastructure creative system only works on linux and is implemented as a set of independent executables including:

1. The root.  Provides the persistent symbol table that ALL programs use.
2. The Daemon.  A per machine process that enables connecting up any number of machines into a Megastructure cluster.
3. Build server.  A simple job server that enables distributed compilation.
4. Executor.  A process that can actually execute Megastructure programs and be reprogrammed as it executes.
5. Terminal.  A utility command line tool that has many commands to control the entire Megastructure network.
6. Plugin.  A dll / shared object that enables game engine integration into the network.
7. Tool.  A simple node that enables easy integration of tests or other endpoints into the system.

Megastructure can be used to run a simulation on mass using unlimited compute power and replications of a simulation in order to train AI on a large scale.

# Underlying Technology.

The domain specific language of Megastructure provides many novel features for creative programming and game development including:

1. Advanced state chart syntax.
2. A compile time checked and extremely powerful event system.
3. Compile time structural inheritance type system that completely integrates with C++ yet completely avoid compilation time overhead.
4. Deep integration into C++ via a customisation of the Clang compiler that enables C++ member function calls on Megastructure objects to be translated into Megastructure invocations - a super powerful compile time form of polymorphic function call.

# Python Integration

Python is automatically supported and the entire system can be easily interactively controlled from python.  Megastructure objects can be automatically used directly from python using the same Megastructure invocation system.

# User Defined Types

User defined types can be easily integrated with default and customisation point based C++ traits.  Types can then be used as data members, function parameters and return types.

# Project Status.

The underlying database technology for the compiler is complete and working and one can see from the HTML reports in the tests that advanced features of the language work.

The project is currently on hold.  Example programs can be found in the "basic" repo.

If you are interested in Megastructure then please contact me at eddeighton@hotmail.co.uk.

# Build Instructions

Info available on request.



