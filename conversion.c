#include "stdlib.h"
#include "conversion.h"
#include "NFA.h"
#include "DFA.h"

NFA *remove_epsilons(NFA *n) {
    Transition temp;
    char c, c2;
    int sub;
    // now, make the epsilons redundant
    for (int s = 0; s < n->num_states; s++) { // states
        for (int t = 0; t < n->states[s].num_transitions; t++) { // parent transitions
            if (n->states[s].transitions[t].symbol != EPSILON) continue;
            temp = n->states[s].transitions[t]; // the parent transition
            for (int t2 = 0; t2 < n->states[temp.destination].num_transitions; t2++) { // child transitions
                c = n->states[temp.destination].transitions[t2].symbol; // child transition symbol
                if (c == EPSILON) continue; // we already have a direct edge from the closure
                c2 = 1;
                for (int i = 0; i < n->states[s].num_transitions; i++) { // parent transitions
                    if (n->states[s].transitions[i].symbol == c
                        && n->states[s].transitions[i].destination
                        == n->states[temp.destination].transitions[t2].destination) {
                        c2 = 0;
                        break;
                    }
                }
                if (c2) {
                    NFA_addTransition(n, s, c,
                        n->states[temp.destination].transitions[t2].destination);
                }
                // we need to propogate the transition along q1's epsilon edges
                sub = n->states[temp.destination].transitions[t2].destination;
                for (int i = 0; i < n->states[sub].num_transitions; i++) { // sub-child transitions
                    if (n->states[sub].transitions[i].symbol != EPSILON) continue;
                    c2 = 1;
                    for (int j = 0; j < n->states[s].num_transitions; j++) { // parent transitions
                        if (n->states[s].transitions[j].symbol == c
                            && n->states[s].transitions[j].destination
                            == n->states[sub].transitions[i].destination) {
                            c2 = 0;
                            break;
                        }
                    }
                    if (c2) {
                        NFA_addTransition(n, s, c, 
                                n->states[sub].transitions[i].destination);
                    }
                }
            }
        }
    }
    // finally, make a copy without the epsilon edges
    NFA *r = malloc(sizeof(NFA));
    r->start = n->start;
    r->num_states = n->num_states;
    r->buf_size = n->buf_size;
    r->states = malloc(r->buf_size*sizeof(State));
    for (int s = 0; s < r->num_states; s++) {
        r->states[s].active = 0;
        r->states[s].num_transitions = 0;
        r->states[s].accepting = n->states[s].accepting;
        r->states[s].buf_size = n->states[s].buf_size;
        r->states[s].transitions = malloc(r->buf_size*sizeof(Transition));
        for (int t = 0; t < n->states[s].num_transitions; t++) {
            if (n->states[s].transitions[t].symbol != EPSILON)
                NFA_addTransition(r, s,
                        n->states[s].transitions[t].symbol,
                        n->states[s].transitions[t].destination);
        }
    }
    return r;
}

NFA *epsilon_closure(NFA *e) {
    NFA *n = NFA_copy(e);
    // use "active" as a flag for avoiding loops
    for (int s = 0; s < n->num_states; s++)
        n->states[s].active = 0;
    for (int i = 0; i < n->num_states; i++) {
        epsilon_closure_helper(n, i);
    }
    return n;
}
void epsilon_closure_helper(NFA *n, int index) {
    State *s = n->states+index;
    Transition t;
    s->active = 1;
    char c;
    // loop over states that s has epsilon transitions to
    for (int i = 0; i < s->num_transitions; i++) { // DFS
        t = s->transitions[i];
        if (t.symbol != EPSILON) continue;
        if (!n->states[t.destination].active)
            epsilon_closure_helper(n, t.destination);
        if (n->states[t.destination].accepting) s->accepting = 1;
        // loop over transitions of node pointed to from s
        for (int j = 0; j < n->states[t.destination].num_transitions; j++) {
            c = n->states[t.destination].transitions[j].symbol;
            if (c != EPSILON) continue;
            // check existing transitions to not add duplicates
            for (int k = 0; k < s->num_transitions; k++) {
                if (s->transitions[k].symbol == EPSILON // check if the epsilon
                    && s->transitions[k].destination    // edge is already there
                    == n->states[t.destination].transitions[j].destination)
                {
                    c = 0; // flag c for skipping creation of a new edge
                    break;
                }
            }
            if (c) {
                NFA_addTransition(n, index, EPSILON,
                        n->states[t.destination].transitions[j].destination);
            }
        }
    }
    s->active = 0;
}

DFA *NFA_convert(NFA *n, char *alph, int alph_size) {
    DFA *d = DFA_create(alph_size, alph);
    // number of substates (from n) in a composite state from d
    int *dstate_size = malloc(1*sizeof(int));
    // array of substates (from n) in a composite state from d
    int **dstate_sub = malloc(1*sizeof(int*));
    // map_size[i] is #(states in d containing i states from n)
    int *map_size = malloc((n->num_states+1)*sizeof(int));
    // consider map[i][j][k]:
    // i = # of states from n combined into one
    // j = jth state in d that is a combination of i states from n
    // k = kth index (in n) of the aforementioned jth state in d
    int ***map = malloc((n->num_states+1)*sizeof(int**));
    for (int i = 0; i <= n->num_states; i++)
        map[i] = NULL;
    for (int i = 0; i <= n->num_states; i++)
        map_size[i] = 0;
    int newstate = DFA_addState(d, n->states[n->start].accepting);
    int garbage, total, found, accepting;
    int *set = malloc(n->num_states*sizeof(int));
    map_size[1] = 1;
    dstate_size[newstate] = 1;
    dstate_sub[newstate] = malloc(1*sizeof(int));
    dstate_sub[newstate][0] = n->start;
    map[1] = malloc(1*sizeof(int*));
    map[1][0] = malloc(1*sizeof(int));
    map[1][0][0] = n->start;
    // loop over states in d as we create them
    for (int dstate = 0; dstate < d->num_states; dstate++) {
        // loop over symbols in alphabet
        for (int a = 0; a < alph_size; a++) {
            total = found = accepting = 0;
            // for each substate of composite state dstate
            for (int sub = 0; sub < dstate_size[dstate]; sub++) {
                // get every destination on a
                for (int t = 0; t < n->states[dstate_sub[dstate][sub]].num_transitions; t++) {
                    if (n->states[dstate_sub[dstate][sub]].transitions[t].symbol != alph[a]) continue;
                    found = 0;
                    accepting |= n->states[n->states[dstate_sub[dstate][sub]].transitions[t].destination].accepting;
                    // check for duplicates
                    for (int t2 = 0; t2 < total; t2++) {
                        if (set[t2] == n->states[dstate_sub[dstate][sub]].transitions[t].destination) {
                            found = 1;
                            break;
                        }
                    }
                    if (!found)
                        set[total++] = n->states[dstate_sub[dstate][sub]].transitions[t].destination;
                }
            }
            int out;
            if (!total) { // go to garbage state
                if (map_size[0])
                    out = garbage;
                else { // create it if it isn't made yet
                    map_size[0]++;
                    out = DFA_addState(d, 0);
                    garbage = out;
                    dstate_size = realloc(dstate_size, d->num_states*sizeof(int));
                    dstate_size[out] = total;
                    dstate_sub = realloc(dstate_sub, d->num_states*sizeof(int*));
                    dstate_sub[out] = malloc(total*sizeof(int));
                    map[0] = malloc(map_size[total]*sizeof(int*));
                    map[0][0] = malloc(total*sizeof(int));
                }
                found = 1;
            }
            // check if the set of states exists as a composite state
            else for (int composite = (found = 0); composite < map_size[total] && found != -1; composite++) {
                // for each substate in the composite state
                for (int sub = 0; sub < total; sub++) {
                    found = 0;
                    // check if the substate matches one in "set"
                    for (int set_index = 0; set_index < total; set_index++) {
                        if (map[total][composite][sub] == set[set_index]) {
                            accepting |= n->states[map[total][composite][sub]].accepting;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) break; // one of the substates didn't match
                }
                if (found) {
                    out = composite; // TODO: "composite" is NOT the index in d
                    break;
                }
            }
            if (!found) {
                // add the new state, save index to "out"
                out = DFA_addState(d, accepting);
                map_size[total]++;
                dstate_size = realloc(dstate_size, d->num_states*sizeof(int));
                dstate_size[out] = total;
                dstate_sub = realloc(dstate_sub, d->num_states*sizeof(int*));
                dstate_sub[out] = malloc(total*sizeof(int));
                map[total] = realloc(map_size[total]>>1?map[total]:NULL, map_size[total]*sizeof(int*));
                map[total][map_size[total]-1] = malloc(total*sizeof(int));
                for (int i = 0; i < total; i++)
                    map[total][map_size[total]-1][i] = dstate_sub[out][i] = set[i];
            }
            // add transition from dstate to "out"
            DFA_updateTransition(d, dstate, alph[a], out);
        }
    }
    // free everything
    for (int i = 0; i <= n->num_states; i++) {
        for (int j = 0; j < map_size[i]; j++) {
            if (dstate_size[j]) free(map[i][j]);
        }
        free(map[i]);
    }
    for (int i = 0; i < d->num_states; i++)
        free(dstate_sub[i]);
    free(dstate_sub);
    free(dstate_size);
    free(map_size);
    free(map);
    free(set);
    return d;
}

DFA *DFA_minimize(DFA *d) {
    int dim = d->num_states-1;
    char *table = malloc(dim*dim*sizeof(char));
    int *map = malloc(d->num_states*sizeof(int));
    // initialize lower left triangular table to all 0s
    // rows: 1-->n, cols: 0-->n-1
    for (int i = 0; i < dim; i++)
        for (int j = 0; j <= i; j++)
            table[dim*i+j] = 0;
    // base case: accepting disparity
    for (int i = 0; i < dim; i++)
        for (int j = 0; j <= i; j++)
            if (d->states[i+1].accepting != d->states[j].accepting)
                table[dim*i+j] = 1;
    int found = 1, big;
    int dest[2];
    // recursive case: check for differences on each edge
    while (found) {
        found = 0;
        for (int i = 0; i < dim; i++) {
            for (int j = 0; j <= i; j++) {
                for (int k = 0; !table[dim*i+j] && k < d->alphabet_size; k++) {
                    dest[0] = d->states[i+1].transitions[k];
                    dest[1] = d->states[j].transitions[k];
                    big = dest[0] < dest[1];
                    if (dest[0] != dest[1] && table[dim*(dest[big]-1)+dest[1-big]]) {
                        table[dim*i+j] = 1;
                        found = 1;
                    }
                }
            }
        }
    }
    // table complete, construct states for new DFA
    DFA *d2 = DFA_create(d->alphabet_size, d->alphabet);
    DFA_addState(d2, d->states[0].accepting);
    map[0] = 0;
    for (int r = 1; r < d->num_states; r++) {
        found = 0;
        for (int c = 0; c < r; c++)
            if (!table[dim*(r-1)+c]) {
                found = 1;
                map[r] = map[c];
            }
        if (!found) {
            map[r] = d2->num_states;
            DFA_addState(d2, d->states[r].accepting);
        }
    }
    // copy edges to new combined states
    for (int s = 0; s < d->num_states; s++)
        for (int a = 0; a < d->alphabet_size; a++)
            DFA_updateTransition(d2, map[s], d->alphabet[a], map[d->states[s].transitions[a]]);
    free(table);
    return d2;
}
