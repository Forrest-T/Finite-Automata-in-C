# Finite-Automata-in-C

This project was my first exploration into object-oriented programming in the C language. The goal was to accept a regular expression and alphabet as input, which is used to search lines of an input file for partial matches, a simpler version of a program such as lex or grep.

I have split the work into several modules, and the overall process is as follows:

1. Parsing the regular expression and constructing an epsilon-NFA recursively
2. Computing the epsilon-closure of the NFA
3. Removing all epsilon edges by propagation backwards of accepting states and non-epsilon transitions
4. Converting the NFA into an equivalent DFA
5. Minimization of the DFA via the table-filling algorithm

Two functions are included for printing NFAs and DFAs. These work by generating LaTeX code to fill into a template at runtime, and then using a C library function to fork a shell script to compile and view the generated graph. The shell commands are saved as strings in the header files for printing, and can be modified to fit whatever system they are being run on.

The NFA and DFA representations are defined separately from the generation function, and as such arbitrarily constructed automata (i.e. not constructed by the parser) are not guaranteed to work with the minimal-DFA creation pipeline. In particular, the recursive generation function guarantees the existence and index of two special states utilized by the constructive definition of the epsilon NFA from a regular expression.

Some further goals intended for when I have time to return to this project are to improve the efficiency of the pipeline, in particular the removal of epsilon edges and conversion from NFA to DFA. I also plan to modularize the NFA to DFA conversion function to improve readability. Without any library support for ordered sets, I opted to store everything in dynamically allocated buffers and traverse them linearly to avoid duplication. My hope is that implementing an ordered set "object" will both improve readability and reduce the number of heap allocations and resizings. This is mainly an exercise to myself, as the majority of the running time is actually spent compiling the LaTeX rather than working with automata.
