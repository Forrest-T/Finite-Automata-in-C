#ifndef _NFA_H_
#define _NFA_H_

#define EPSILON '\1'
#define DOT '.'
extern int ERROR;

typedef struct {
    int destination;
    char symbol;
} Transition;

typedef struct {
    int accepting;
    int active;
    int num_transitions;
    int buf_size;
    Transition *transitions;
} State;

typedef struct {
    int start;       // initial state
    int num_states;  // total number of states
    int buf_size;    // size of allocated states array
    State *states;   // array of states
} NFA;

/* create an NFA with one starting state */
NFA *NFA_create();
/* specify whether the starting state is accepting */
NFA *NFA_create_accept(int startAccept);
/* regexp inductive construction */
NFA *NFA_create_character(char c);
NFA *NFA_create_emptySet();
NFA *NFA_create_epsilon();
NFA *NFA_create_concat(NFA *a, NFA *b);
NFA *NFA_create_star(NFA *a);
NFA *NFA_create_or(NFA *a, NFA *b);
/* returns an NFA containing all states and transitions from A and B */
NFA *NFA_combine_helper(NFA *a, NFA *b);
/* creates a copy of an NFA */
NFA *NFA_copy(NFA *a);
/* destroy an NFA (free memory) */
void NFA_destroy(NFA *n);
/* reset n for reading a new string */
void NFA_reset(NFA *n);
/* adds state, returns index */
int NFA_addState(NFA *n, int accept);
/* adds a transition */
void NFA_addTransition(NFA *n, int start, char symbol, int dest);
/* propagates activation along epsilon edges */
void NFA_epsilonTransitions(NFA *n);
void NFA_epsilonTransitions_helper(NFA *n, State *s);
/* reads a character and updates the NFA */
void NFA_read(NFA *n, char symbol);
void NFA_readStr(NFA *n, const char *str);
/* determines whether n is in an accepting state */
int NFA_accept(NFA *n);
/* prints the NFA */
void NFA_print(NFA *n);

#endif
