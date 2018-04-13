#include "string.h"
#include "stdio.h"

#include "conversion.h"
#include "parser.h"
#include "NFA.h"
#include "DFA.h"

int main(int argc, char **argv) {
    if (argc < 2)
        die("Usage: ./parser {regexp}\n");
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
    DFA *d = NFA_convert(n, "abc", 3);
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

