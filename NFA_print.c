#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "NFA.h"
#include "NFA_print.h"

void NFA_print(NFA *n) {
    FILE *f = fopen("output/output.tex", "w");
    if (f == NULL) {
        printf("could not open file for writing!\n");
        return;
    }
    fprintf(f, preamble);
    int width = ceil(sqrt(n->num_states));
    fprintf(f, "        ");
    fprintf(f, "\\node[state,initial] (q_%d) {$q_%d$};\n", n->start, n->start);
    for (int s = 1; s < n->num_states; s++) {
        fprintf(f, "        ");
        fprintf(f, "\\node[state");
        if (s == n->start) fprintf(f, ",initial");
        if (n->states[s].accepting) fprintf(f, ",accepting");
        fprintf(f, "] (q_%d) [", s);
        if (s%width == 0) {
            fprintf(f, "below=of q_%d]", s-width);
        } else {
            fprintf(f, "right=of q_%d]", s-1);
        }
        fprintf(f, " {$q_%d$};\n",s);
    }
    fprintf(f, "\\path[every node/.style={font=\\sffamily\\small}]\n");
    for (int s = 0; s < n->num_states; s++) {
        if (!n->states[s].num_transitions) continue;
        fprintf(f, "        ");
        fprintf(f, "(q_%d)", s);
        for (int t = 0; t < n->states[s].num_transitions; t++) {
            fprintf(f, " edge ");
            if (n->states[s].transitions[t].destination == s)
                fprintf(f, "[loop above] ");
            else {
                if (n->states[s].transitions[t].destination%width == s%width)
                    fprintf(f, "[bend right] ");
                if (n->states[s].transitions[t].destination/width == s/width)
                    fprintf(f, "[bend left] ");
            }
            if (n->states[s].transitions[t].symbol == EPSILON)
                fprintf(f, "node {%s} ", "$\\varepsilon$");
            else
                fprintf(f, "node {%c} ", n->states[s].transitions[t].symbol);
            if (n->states[s].transitions[t].destination == s)
                fprintf(f, "()");
            else
                fprintf(f, "(q_%d)", n->states[s].transitions[t].destination);
        }
        fprintf(f, "\n");
    }
    fprintf(f, ";\n");
    fprintf(f, postamble);
    fclose(f);
    system(display);
}
