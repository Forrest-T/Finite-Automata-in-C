#include "stdlib.h"
#include "NFA.h"

int ERROR = 0;

NFA *NFA_create() {
    return NFA_create_accept(0);
}
NFA *NFA_create_accept(int startAccept) {
    NFA *n = (NFA*) malloc(sizeof(NFA));
    n->num_states = 1;
    n->buf_size = 2; // Arbitrary default size
    n->states = (State*) malloc(n->buf_size*sizeof(State));
    n->states[0].num_transitions = 0;
    n->states[0].buf_size = 0;
    n->states[0].transitions = NULL;
    n->states[0].accepting = startAccept;
    n->states[0].active = 1;
    n->start = 0;
    return n;
}

NFA *NFA_copy(NFA *a) {
    NFA *n = (NFA*) malloc(sizeof(NFA));
    n->start = a->start;
    n->num_states = a->num_states;
    n->buf_size = a->buf_size;
    n->states = malloc(n->buf_size*sizeof(State));
    State *ptr = n->states;
    for (int s = 0; s < a->num_states; ++s, ++ptr) {
        ptr->accepting = a->states[s].accepting;
        ptr->buf_size = a->states[s].buf_size;
        ptr->num_transitions = a->states[s].num_transitions;
        ptr->transitions = malloc(ptr->num_transitions*sizeof(Transition));
        for (int t = 0; t < ptr->num_transitions; t++)
            ptr->transitions[t] = a->states[s].transitions[t];
    }
    return n;
}

void NFA_destroy(NFA *n) {
    for (int i = 0; i < n->num_states; i++)
        if (n->states[i].transitions != NULL)
            free(n->states[i].transitions);
    free(n->states);
    free(n);
}

void NFA_reset(NFA *n) {
    for (int i = 0; i < n->num_states; ++i)
        n->states[i].active = 0;
    n->states[n->start].active = 1;
    NFA_epsilonTransitions(n);
}

void NFA_epsilonTransitions(NFA *n) {
    for (int i = 0; i < n->num_states; i++) {
        if (n->states[i].active)
            NFA_epsilonTransitions_helper(n, &n->states[i]);
    }
}
void NFA_epsilonTransitions_helper(NFA *n, State *s) {
    for (int t = 0; t < s->num_transitions; t++) {
        if (s->transitions[t].symbol == EPSILON &&
            !n->states[s->transitions[t].destination].active) {
            n->states[s->transitions[t].destination].active = 1;
            NFA_epsilonTransitions_helper(n,&n->states[s->transitions[t].destination]);
        }
    }
}

int NFA_addState(NFA *n, int accept) {
    n->num_states += 1;
    if (n->num_states > n->buf_size) {
        n->buf_size *= 2;
        n->states = realloc(n->states, n->buf_size*sizeof(State));
    }
    n->states[n->num_states-1].active = 0;
    n->states[n->num_states-1].accepting = accept;
    n->states[n->num_states-1].num_transitions = 0;
    n->states[n->num_states-1].buf_size = 0;
    n->states[n->num_states-1].transitions = NULL;
    return n->num_states-1;
}

void NFA_addTransition(NFA *n, int start, char symbol, int dest) {
    if (start >= n->num_states || dest >= n->num_states) {
        ERROR |= 1;
        return;
    }
    State *st = n->states+start;
    st->num_transitions += 1;
    if (st->buf_size == 0) {
        st->buf_size = 1;
        st->transitions = malloc(st->buf_size*sizeof(Transition));
    } else if (st->num_transitions > st->buf_size) {
        st->buf_size *= 2;
        st->transitions = realloc(st->transitions,
                                  st->buf_size*sizeof(Transition));
    }
    st->transitions[st->num_transitions-1].destination = dest;
    st->transitions[st->num_transitions-1].symbol = symbol;
}

void NFA_read(NFA *n, char symbol) {
    // second bit holds previous state
    for (int i = 0; i < n->num_states; i++)
        n->states[i].active <<= 1;
    for (int i = 0; i < n->num_states; i++)
        if (n->states[i].active & 0x2) {
            for (int t = 0; t < n->states[i].num_transitions; t++)
                if (n->states[i].transitions[t].symbol == symbol
                    || n->states[i].transitions[t].symbol == DOT)
                    n->states[n->states[i].transitions[t].destination].active |= 1;
            n->states[i].active &= 1;
        }
    NFA_epsilonTransitions(n);
}
void NFA_readStr(NFA *n, const char *str) {
    while (*str != '\0') {
        NFA_read(n, *str);
        ++str;
    }
}

int NFA_accept(NFA *n) {
    for (int i = 0; i < n->num_states; i++)
        if (n->states[i].active && n->states[i].accepting)
            return 1;
    return 0;
}
