#include "NFA.h"
#include "stdio.h"
#include "string.h"
#include "parser.h"
#include "NFA_print.h"

static int failed = 0;

NFA *setup(char *s) {
    return parse(s, strlen(s));
}

void test(NFA *n, const char *s, int accept) {
    if (NFA_reset(n), NFA_readStr(n, s), accept == NFA_accept(n))
        printf("\t\033[32mCORRECT\033[37m\t %s \"%s\"\n", accept?"accepted":"rejected", s);
    else
        (printf("\t\033[31mWRONG\033[37m\t %s \"%s\"\n", accept?"rejected":"accepted", s), failed++);
}

void test_a_star() {
    NFA *n = setup("a*");
    test(n, "", 1);
    test(n, "a", 1);
    test(n, "aa", 1);
    test(n, "aaa", 1);
    test(n, "aaaa", 1);
    test(n, "aaaaa", 1);
    NFA_destroy(n);
}

void test_a_plus() {
    NFA *n = setup("a+");
    test(n, "", 0);
    test(n, "a", 1);
    test(n, "aa", 1);
    test(n, "aaa", 1);
    test(n, "aaaa", 1);
    test(n, "aaaaa", 1);
    NFA_destroy(n);
}

void test_string() {
    NFA *n = setup("abcde");
    test(n, "", 0);
    test(n, "a", 0);
    test(n, "ab", 0);
    test(n, "abc", 0);
    test(n, "abcde", 1);
    test(n, "abcdea", 0);
    test(n, "e", 0);
    NFA_destroy(n);
}

void test_a_star_b() {
    NFA *n = setup("a*b");
    test(n, "", 0);
    test(n, "b", 1);
    test(n, "ab", 1);
    test(n, "ba", 0);
    test(n, "aab", 1);
    test(n, "aaaaaab", 1);
    test(n, "aaba", 0);
    test(n, "aaaaaa", 0);
    NFA_destroy(n);
}

void test_a_or_b_or_c() {
    NFA *n = setup("a|b|c");
    test(n, "", 0);
    test(n, "a", 1);
    test(n, "b", 1);
    test(n, "c", 1);
    test(n, "aa", 0);
    test(n, "ba", 0);
    test(n, "cb", 0);
    test(n, "ac", 0);
    NFA_destroy(n);
}

void test_aa_plus() {
    NFA *n = setup("ba+");
    test(n, "", 0);
    test(n, "b", 0);
    test(n, "ba", 1);
    test(n, "baa", 1);
    test(n, "baaa", 1);
    test(n, "baaaa", 1);
    test(n, "baaaaa", 1);
    NFA_destroy(n);
}

void test_a_q() {
    NFA *n = setup("a?");
    test(n, "", 1);
    test(n, "a", 1);
    test(n, "aa", 0);
    NFA_destroy(n);
}

void test_parens() {
    NFA *n = setup("(a|b)c");
    test(n, "", 0);
    test(n, "c", 0);
    test(n, "ac", 1);
    test(n, "bc", 1);
    test(n, "aababba", 0);
    test(n, "aababbac", 0);
    NFA_destroy(n);
}
void test_parens2() {
    NFA *n = setup("(a)(b)");
    test(n, "", 0);
    test(n, "a", 0);
    test(n, "b", 0);
    test(n, "ab", 1);
    NFA_destroy(n);
}

void test_combined() {
    NFA *n = setup("(a|b)*c(aa|bb)c(a|b)?");
    test(n, "", 0);
    test(n, "caac", 1);
    test(n, "bcbbca", 1);
    test(n, "bcbbcaa", 0);
    test(n, "babbabbacc", 0);
    test(n, "babbabbacaac", 1);
    test(n, "babbabbacaaca", 1);
    NFA_destroy(n);
}

void test_combined2() {
    NFA *n = setup("(a|b)(b|c).*a+b?");
    test(n, "", 0);
    test(n, "a", 0);
    test(n, "ab", 0);
    test(n, "aab", 0);
    test(n, "baaa", 0);
    test(n, "aba", 1);
    test(n, "abab", 1);
    test(n, "abababa", 1);
    test(n, "bca", 1);
    test(n, "bcab", 1);
    test(n, "bbaalskdfad", 0);
    test(n, "aca", 1);
    NFA_destroy(n);
}

int main() {
    printf("Testing /a*/\n");
    test_a_star();
    printf("Testing /a+/\n");
    test_a_plus();
    printf("Testing /abcde/\n");
    test_string();
    printf("Testing /a*b/\n");
    test_a_star_b();
    printf("Testing /a|b|c/\n");
    test_a_or_b_or_c();
    printf("Testing /aa+/\n");
    test_aa_plus();
    printf("Testing /a?/\n");
    test_a_q();
    printf("Testing /(a)(b)/\n");
    test_parens2();
    printf("Testing /(a|b)*c/\n");
    test_parens();
    printf("Testing /(a|b)*c(aa|bb)c(a|b)?/\n");
    test_combined();
    printf("Testing /(a|b)(b|c).*a+b?/\n");
    test_combined2();

    printf("Number of failed tests: %d\n", failed);
}
