#include "NFA.h"
#include "stdio.h"

static int failed = 0;

void test(NFA *n, const char *s, int accept) {
    if (NFA_reset(n), NFA_readStr(n, s), accept == NFA_accept(n))
        printf("\t\033[32mCORRECT\033[37m\t %s \"%s\"\n", accept?"accepted":"rejected", s);
    else
        (printf("\t\033[31mWRONG\033[37m\t %s \"%s\"\n", accept?"rejected":"accepted", s), failed++);
}

void test_oddNumBs() {
    NFA *n = NFA_create();
    int q1 = n->start;
    int q2 = NFA_addState(n, 1);
    NFA_addTransition(n, q1, 'a', q1);
    NFA_addTransition(n, q1, 'b', q2);
    NFA_addTransition(n, q2, 'a', q2);
    NFA_addTransition(n, q2, 'b', q1);
    test(n, "", 0);
    test(n, "a", 0);
    test(n, "aa", 0);
    test(n, "aabb", 0);
    test(n, "aababaaababa", 0);
    test(n, "b", 1);
    test(n, "bbb", 1);
    test(n, "baabaab", 1);
    NFA_destroy(n);
}

void test_endsInAB() {
    NFA *n = NFA_create();
    int q0 = n->start;
    int q1 = NFA_addState(n, 0);
    int q2 = NFA_addState(n, 1);
    NFA_addTransition(n, q0, 'a', q0);
    NFA_addTransition(n, q0, 'b', q0);
    NFA_addTransition(n, q0, 'a', q1);
    NFA_addTransition(n, q1, 'b', q2);
    test(n, "", 0);
    test(n, "aa", 0);
    test(n, "aababa", 0);
    test(n, "ab", 1);
    test(n, "ba", 0);
    test(n, "aabababababbabababab", 1);
    NFA_destroy(n);
}

void test_epsilon() {
    NFA *n = NFA_create();
    int q0 = n->start;
    int q1 = NFA_addState(n, 0);
    int q2 = NFA_addState(n, 0);
    int q3 = NFA_addState(n, 0);
    int q4 = NFA_addState(n, 1);
    NFA_addTransition(n, q0, EPSILON, q1);
    NFA_addTransition(n, q1, EPSILON, q2);
    NFA_addTransition(n, q2, EPSILON, q3);
    NFA_addTransition(n, q3, EPSILON, q4);
    test(n, "", 1);
    NFA_destroy(n);
}

void test_parity() {
    NFA *n = NFA_create_accept(1);
    int q1 = n->start;
    int q2 = NFA_addState(n, 0);
    NFA_addTransition(n, q1, DOT, q2);
    NFA_addTransition(n, q2, DOT, q1);
    test(n, "", 1);
    test(n, "a", 0);
    test(n, ".", 0);
    test(n, "b", 0);
    test(n, "akfj", 1);
    test(n, "albkdjf", 0);
    NFA_destroy(n);
}

void test_reg_empty() {
    NFA *n = NFA_create_emptySet();
    test(n, "", 0);
    test(n, "a", 0);
    NFA_destroy(n);
}

void test_reg_character() {
    NFA *n = NFA_create_character('f');
    test(n, "", 0);
    test(n, "a", 0);
    test(n, "f", 1);
    NFA_destroy(n);
}

void test_reg_epsilon() {
    NFA *n = NFA_create_epsilon();
    test(n, "", 1);
    test(n, "a", 0);
    NFA_destroy(n);
}

void test_reg_concat() {
    NFA *a = NFA_create_character('a');
    NFA *b = NFA_create_character('b');
    NFA *n = NFA_create_concat(a, b);
    NFA_destroy(a);
    NFA_destroy(b);
    test(n, "", 0);
    test(n, "a", 0);
    test(n, "b", 0);
    test(n, "ab", 1);
    NFA_destroy(n);
}

void test_reg_star() {
    NFA *a = NFA_create_character('a');
    NFA *b = NFA_create_character('b');
    NFA *ab = NFA_create_concat(a, b);
    NFA_destroy(a);
    NFA_destroy(b);
    NFA *n = NFA_create_star(ab);
    NFA_destroy(ab);
    test(n, "", 1);
    test(n, "a", 0);
    test(n, "b", 0);
    test(n, "ab", 1);
    test(n, "aba", 0);
    test(n, "abab", 1);
    test(n, "baba", 0);
    NFA_destroy(n);
}

void test_reg_or() {
    NFA *a = NFA_create_character('a');
    NFA *b = NFA_create_character('b');
    NFA *n = NFA_create_or(a, b);
    NFA_destroy(a);
    NFA_destroy(b);
    test(n, "", 0);
    test(n, "a", 1);
    test(n, "b", 1);
    test(n, "ab", 0);
    NFA_destroy(n);
}

int main() {
    printf("Testing DFA that recognizes odd number of b's\n");
    test_oddNumBs();
    printf("Testing NFA that recognizes strings ending in ab\n");
    test_endsInAB();
    printf("Testing epsilon NFA\n");
    test_epsilon();
    printf("Testing parity with dots\n");
    test_parity();
    printf("Testing regex empty set\n");
    test_reg_empty();
    printf("Testing regex epsilon\n");
    test_reg_epsilon();
    printf("Testing regex character\n");
    test_reg_character();
    printf("Testing regex concat\n");
    test_reg_concat();
    printf("Testing regex or\n");
    test_reg_or();
    printf("Testing regex star\n");
    test_reg_star();

    printf("Number of failed tests: %d\n", failed);
}
