#include "stdlib.h"
#include "NFA.h"

#define MAX(a,b) (((a)<(b))?(b):(a))

/* Invariant for these constructions:
 *  states[0] = non-accepting start state
 *  states[1] = sole accepting end state 
 */

NFA *NFA_create_character(char c) {
    NFA *n = NFA_create();
    int q = NFA_addState(n, 1);
    NFA_addTransition(n, n->start, c, q);
    return n;
}

NFA *NFA_create_emptySet() {
    NFA *n = NFA_create();
    NFA_addState(n, 1);
    return n;
}

NFA *NFA_create_epsilon() {
    return NFA_create_character(EPSILON);
}

NFA *NFA_combine_helper(NFA *a, NFA *b) {
    NFA *n = NFA_create_emptySet();
    n->num_states += a->num_states + b->num_states;
    if (n->buf_size < n->num_states) {
        n->buf_size = n->num_states;
        n->states = realloc(n->states, n->buf_size*sizeof(State));
    }
    State *ptr = n->states+2;
    for (int s = 0; s < a->num_states; ++s, ++ptr) {
        *ptr = a->states[s];
        ptr->accepting = 0;
        if (ptr->buf_size) 
            ptr->transitions = malloc(ptr->buf_size*sizeof(Transition));
        for (int t = 0; t < ptr->num_transitions; t++) {
            ptr->transitions[t] = a->states[s].transitions[t];
            ptr->transitions[t].destination += 2;
        }
    }
    for (int s = 0; s < b->num_states; ++s, ++ptr) {
        *ptr = b->states[s];
        ptr->accepting = 0;
        if (ptr->buf_size) 
            ptr->transitions = malloc(ptr->buf_size*sizeof(Transition));
        for (int t = 0; t < ptr->num_transitions; t++) {
            ptr->transitions[t] = b->states[s].transitions[t];
            ptr->transitions[t].destination += 2+a->num_states;
        }
    }
    return n;
}

NFA *NFA_create_concat(NFA *a, NFA *b) {
    // Note: creates extra start and end state for compatibility
    NFA *n = NFA_combine_helper(a, b);
    NFA_addTransition(n, 0, EPSILON, 2);
    NFA_addTransition(n, 3, EPSILON, a->num_states+2);
    NFA_addTransition(n, a->num_states+3, EPSILON, 1);
    NFA_reset(n);
    return n;
}

NFA *NFA_create_star(NFA *a) {
    NFA *n = NFA_create_epsilon();
    n->num_states += a->num_states;
    if (n->buf_size < n->num_states) {
        n->buf_size = n->num_states;
        n->states = realloc(n->states, n->buf_size*sizeof(State));
    }
    State *ptr = n->states+2;
    for (int s = 0; s < a->num_states; ++s, ++ptr) {
        *ptr = a->states[s];
        ptr->accepting = 0;
        if (ptr->num_transitions)
            ptr->transitions = malloc(ptr->buf_size*sizeof(Transition));
        for (int t = 0; t < ptr->num_transitions; t++) {
            ptr->transitions[t] = a->states[s].transitions[t];
            ptr->transitions[t].destination += 2;
        }
    }
    NFA_addTransition(n, 0, EPSILON, 2);
    NFA_addTransition(n, 3, EPSILON, 1);
    NFA_addTransition(n, 3, EPSILON, 2);
    NFA_reset(n);
    return n;
}

NFA *NFA_create_or(NFA *a, NFA *b) {
    NFA *n = NFA_combine_helper(a, b);
    NFA_addTransition(n, 0, EPSILON, 2);
    NFA_addTransition(n, 0, EPSILON, a->num_states+2);
    NFA_addTransition(n, 3, EPSILON, 1);
    NFA_addTransition(n, a->num_states+3, EPSILON, 1);
    NFA_reset(n);
    return n;
}
