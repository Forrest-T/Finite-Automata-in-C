#ifndef _DFA_H_
#define _DFA_H_

#define DOT '.'
extern int DFA_INVALID;

typedef struct {
    int accepting;
    int buf_size;
    int *transitions;
} DFA_State;

typedef struct {
    int start;       // initial state
    int active;      // active state
    int num_states;  // total number of states
    int buf_size;    // size of allocated states array
    int alphabet_size;
    char *alphabet;
    DFA_State *states;   // array of states
} DFA;

/* create an empty DFA */
DFA *DFA_create(int alph_size, char *alph);
/* creates a copy of an DFA */
DFA *DFA_copy(DFA *a);
/* destroy an DFA (free memory) */
void DFA_destroy(DFA *d);
/* reset n for reading a new string */
void DFA_reset(DFA *d);
/* adds state, returns index */
int DFA_addState(DFA *d, int accept);
/* adds a transition */
void DFA_updateTransition(DFA *d, int start, char symbol, int dest);
/* reads a character and updates the DFA */
void DFA_read(DFA *d, char symbol);
void DFA_readStr(DFA *d, const char *str);
/* determines whether n is in an accepting state */
int DFA_accept(DFA *d);
/* prints the DFA */
void DFA_print(DFA *d);
int DFA_valid();

#endif
