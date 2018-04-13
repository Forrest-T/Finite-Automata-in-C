#ifndef _CONVERSION_H_
#define _CONVERSION_H_

#include "NFA.h"
#include "DFA.h"

// returns an NFA with all epsilon edges removed
NFA *remove_epsilons(NFA *e);
// returns an equivalent NFA with epsilon closure
NFA *epsilon_closure(NFA *e);
void epsilon_closure_helper(NFA*, int);
// converts an NFA to a DFA
DFA *NFA_convert(NFA *n, char*, int);
DFA *DFA_minimize(DFA *d);              // TODO

#endif
