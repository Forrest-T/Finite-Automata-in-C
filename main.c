#include "string.h"
#include "stdlib.h"
#include "stdio.h"

#include "conversion.h"
#include "parser.h"
#include "NFA.h"
#include "DFA.h"

int debug_intermediate(char **argv) {
    printf("\tregex:  /%s/\n", argv[1]);
    int length = strlen(argv[1]);
    printf("\tlength: %d\n", length);
    printf("\tparens: %s\n", verifyParens(argv[1])?"balanced":"unbalanced!!!");
    // create NFA
    printf("Creating NFA for /%s/... ", argv[1]);
    fflush(stdout);
    NFA *ne = parse(argv[1], length);
    NFA_print(ne);
    printf("Done!\n");
    // epsilon closure
    printf("Calculating epsilon closure... ");
    fflush(stdout);
    NFA *ec = epsilon_closure(ne);
    NFA_print(ec);
    printf("Done!\n");
    // remove epsilons
    printf("Removing epsilon transitions... ");
    fflush(stdout);
    NFA *n = remove_epsilons(ec);
    NFA_print(n);
    printf("Done!\n");
    // convert to DFA
    printf("Converting to DFA... ");
    fflush(stdout);
    DFA *d = NFA_convert(n, argv[2], strlen(argv[2]));
    DFA_print(d);
    printf("Done!\n");
    // minimize
    printf("Minimizing DFA... ");
    fflush(stdout);
    DFA *dm = DFA_minimize(d);
    DFA_print(dm);
    printf("Done!\n");
    // clean up
    NFA_destroy(ne);
    NFA_destroy(ec);
    NFA_destroy(n);
    DFA_destroy(d);
    DFA_destroy(dm);
    return 0;
}

int main(int argc, char **argv) {
    if (argc == 3)
        return debug_intermediate(argv);
    char *alph = NULL;
    size_t buf_len = 0;
    if (getline(&alph, &buf_len, stdin) == -1) {
        free(alph);
        die("unable to read alphabet");
    }
    char *regex = NULL;
    if (getline(&regex, &buf_len, stdin) == -1) {
        free(alph);
        free(regex);
        die("unable to read regex");
    }
    buf_len = 0;
    NFA *ne = parse(regex, strlen(regex)-1);
    // ignore the '\n'                  ^^
    NFA *ec = epsilon_closure(ne);
    NFA *n = remove_epsilons(ec);
    DFA *d = NFA_convert(n, alph, strlen(alph)-1);
    DFA *dm = DFA_minimize(d);
    DFA_reset(dm);
#ifdef DEBUG
    NFA_print(ne);
    NFA_print(ec);
    NFA_print(n);
    DFA_print(d);
    DFA_print(dm);
#endif
    // read lines from stdin and run "dm"
    char *line = NULL;
    buf_len = 0;
    while (getline(&line, &buf_len, stdin) != -1) {
        int line_len = strlen(line);
        // if dm accepts the empty string, no need to check
        int accept = (DFA_reset(dm), DFA_accept(dm));
        // loop over starting indices to search for substrings
        for (int i = 0; i < line_len && !accept; i++) {
            DFA_reset(dm);
            for (int j = 0; j < line_len-i && !accept; j++) {
                DFA_read(dm, line[i+j]);
                if (DFA_accept(dm)) accept = 1;
            }
        }
        // print lines that are matched
        if (accept) printf("%s", line);
        free(line);
        line = NULL;
    }
    // clean up
    free(line);
    free(alph);
    free(regex);
    NFA_destroy(ne);
    NFA_destroy(ec);
    NFA_destroy(n);
    DFA_destroy(d);
    DFA_destroy(dm);
    return 0;
}
