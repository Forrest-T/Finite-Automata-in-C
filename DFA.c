#include "stdlib.h"
#include "DFA.h"

int DFA_INVALID = 0;

DFA *DFA_create(int alph_size, char *alph) {
    DFA *d = (DFA*) malloc(sizeof(DFA));
    d->start = 0;
    d->num_states = 0;
    d->buf_size = 2; // Arbitrary default size
    d->states = (DFA_State*) malloc(d->buf_size*sizeof(DFA_State));
    d->alphabet_size = alph_size;
    d->alphabet = malloc(d->alphabet_size*sizeof(char));
    for (int c = 0; c < alph_size; c++)
        d->alphabet[c] = alph[c];
    return d;
}

DFA *DFA_copy(DFA *a) {
    DFA *d = (DFA*) malloc(sizeof(DFA));
    d->start = a->start;
    d->num_states = a->num_states;
    d->buf_size = a->buf_size;
    d->alphabet_size = a->alphabet_size;
    d->alphabet = malloc(d->alphabet_size*sizeof(char));
    for (int c = 0; c < d->alphabet_size; c++)
        d->alphabet[c] = a->alphabet[c];
    d->states = malloc(d->buf_size*sizeof(DFA_State));
    DFA_State *ptr = a->states;
    for (int s = 0; s < a->num_states; ++s, ++ptr) {
        *ptr = a->states[s];
        ptr->transitions = malloc(d->alphabet_size*sizeof(int));
        for (int t = 0; t < d->alphabet_size; t++)
            ptr->transitions[t] = a->states[s].transitions[t];
    }
    DFA_reset(d);
    return d;
}

void DFA_destroy(DFA *d) {
    for (int i = 0; i < d->num_states; i++)
        free(d->states[i].transitions);
    free(d->states);
    free(d->alphabet);
    free(d);
}

void DFA_reset(DFA *d) {
    d->active = d->start;
}

int DFA_addState(DFA *d, int accept) {
    d->num_states += 1;
    if (d->num_states > d->buf_size) {
        d->buf_size *= 2;
        d->states = realloc(d->states, d->buf_size*sizeof(DFA_State));
    }
    d->states[d->num_states-1].accepting = accept;
    d->states[d->num_states-1].buf_size = 0;
    d->states[d->num_states-1].transitions = malloc(d->alphabet_size*sizeof(int));
    for (int i = 0; i < d->alphabet_size; i++)
        d->states[d->num_states-1].transitions[i] = 0;
    return d->num_states-1;
}

void DFA_updateTransition(DFA *d, int start, char symbol, int dest) {
    int index = -1;
    for (int i = 0; i < d->alphabet_size; i++) {
        if (d->alphabet[i] == symbol) {
            index = i;
            break;
        }
    }
    if (start >= d->num_states || dest >= d->num_states 
        || start < 0 || dest < 0 || index == -1) {
        DFA_INVALID |= 1;
        return;
    }
    d->states[start].transitions[index] = dest;
}

void DFA_read(DFA *d, char symbol) {
    int index = -1;
    // find index of transition corresponding to symbol
    for (int i = 0; i < d->alphabet_size; i++) {
        if (d->alphabet[i] == symbol) {
            index = i;
            break;
        }
    }
    // make sure it is valid
    if (index == -1) {
        DFA_INVALID |= 1;
        return;
    }
    // update
    d->active = d->states[d->active].transitions[index];
}
void DFA_readStr(DFA *d, const char *str) {
    while (*str != '\0') {
        DFA_read(d, *str);
        ++str;
    }
}

int DFA_accept(DFA *d) {
    return d->states[d->active].accepting;
}

int DFA_valid() { return !DFA_INVALID; }
