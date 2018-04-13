#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "DFA.h"
#include "DFA_print.h"

void DFA_print(DFA *n) {
    FILE *f = fopen("output/output.tex", "w");
    if (f == NULL) {
        printf("could not open file for writing!\n");
        return;
    }
    fprintf(f, preamble);
    int width = ceil(sqrt(n->num_states));
    fprintf(f, "        ");
    fprintf(f, "\\node[state,initial");
    if (n->states[n->start].accepting) fprintf(f, ",accepting");
    fprintf(f, "] (q_%d) {$q_%d$};\n", n->start, n->start);
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
        if (!n->alphabet_size) continue;
        fprintf(f, "        ");
        fprintf(f, "(q_%d)", s);
        for (int t = 0; t < n->alphabet_size; t++) {
            int e = n->states[s].transitions[t];
            // check if we've already printed this one
            for (int t2 = t-1; t2 >= 0; t2--) {
                if (n->states[s].transitions[t2] == e) {
                    e = -1;
                    break;
                }
            }
            if (e  == -1) continue;
            fprintf(f, " edge ");
            if (e == s)
                fprintf(f, "[loop above] ");
            else if (e%width == s%width)
                fprintf(f, "[bend right] ");
            else if (e/width == s/width)
                fprintf(f, "[bend left] ");
            fprintf(f, "node {%c", n->alphabet[t]);
            for (int t2 = t+1; t2 < n->alphabet_size; t2++) {
                if (n->states[s].transitions[t2] == e)
                    fprintf(f, ",%c", n->alphabet[t2]);
            }
            fprintf(f, "} ");
            if (e == s)
                fprintf(f, "()");
            else
                fprintf(f, "(q_%d)", e);
        }
        fprintf(f, "\n");
    }
    fprintf(f, ";\n");
    fprintf(f, postamble);
    fclose(f);
    system(display);
    //system("vim output/output.tex");
}
