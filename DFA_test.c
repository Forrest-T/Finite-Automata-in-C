#include "DFA.h"
#include "stdio.h"

static int failed = 0;

void test(DFA *n, const char *s, int accept) {
    if (DFA_reset(n), DFA_readStr(n, s), accept == DFA_accept(n))
        printf("\t\033[32mCORRECT\033[37m\t %s \"%s\"\n", accept?"accepted":"rejected", s);
    else
        (printf("\t\033[31mWRONG\033[37m\t %s \"%s\"\n", accept?"rejected":"accepted", s), failed++);
}

void test_even() {
    DFA *d = DFA_create(2, "ab");
    int q0 = DFA_addState(d, 1);
    d->start = q0;
    int q1 = DFA_addState(d, 0);
    DFA_updateTransition(d, q0, 'a', q1);
    DFA_updateTransition(d, q1, 'a', q0);
    DFA_updateTransition(d, q0, 'b', q1);
    DFA_updateTransition(d, q1, 'b', q0);
    test(d, "", 1);
    test(d, "a", 0);
    test(d, "b", 0);
    test(d, "aa", 1);
    test(d, "bbb", 0);
    DFA_destroy(d);
}

void test_end_a() {
    DFA *d = DFA_create(5, "asdfg");
    int q0 = DFA_addState(d, 0);
    int q1 = DFA_addState(d, 1);
    d->start = q0;
    DFA_updateTransition(d, q0, 'a', q1);
    DFA_updateTransition(d, q1, 'a', q1);
    DFA_updateTransition(d, q0, 's', q0);
    DFA_updateTransition(d, q1, 's', q0);
    DFA_updateTransition(d, q0, 'd', q0);
    DFA_updateTransition(d, q1, 'd', q0);
    DFA_updateTransition(d, q0, 'f', q0);
    DFA_updateTransition(d, q1, 'f', q0);
    DFA_updateTransition(d, q0, 'g', q0);
    DFA_updateTransition(d, q1, 'g', q0);
    DFA_print(d);
    test(d, "", 0);
    test(d, "a", 1);
    test(d, "s", 0);
    test(d, "agf", 0);
    test(d, "sdfasdgasdfasdfa", 1);
    DFA_destroy(d);
}

int main() {
    printf("Testing even number of characters\n");
    test_even();
    printf("Testing ends in a\n");
    test_end_a();

    printf("Number of failed tests: %d\n", failed);
}

