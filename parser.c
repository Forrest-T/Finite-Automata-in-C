#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#include "parser.h"
#include "NFA.h"

NFA *parse(char *c, int len) {
    if (len == 0) return NFA_create_epsilon();
    if (len == 1) return NFA_create_character(*c);
    NFA *e = NFA_create_epsilon(); // temporary NFA for inductive constructions
    // Note: this ^ allows regexes to match [*+?|] via (*), (+), etc.
    Info info[len]; // keep track of what we're parsing at each index

    for (int i = 0; i < len; i++) {
        info[i].type = UNKNOWN; // initialise all to unknown
        info[i].n = NULL;
    }
    // First, handle normal characters and parens
    for (int i = 0; i < len; i++) {
        // recursively handle parenthesis
        if (c[i] == '(') {
            int d = distanceToMatch(c+i);
            info[i].type = REGEX;
            info[i].n = parse(c+i+1, d-1);
            for (int j = 0; j < d; j++)
                info[i+j+1].type = SKIP;
            i += d;
            continue;
        }
        // make single letter NFAs, ignoring special regex characters
        if (c[i]!='*' && c[i]!='|' && c[i]!='+' && c[i]!='?') {
            info[i].type = REGEX;
            info[i].n = NFA_create_character(c[i]);
        }
    }

    // At this point, each symbol and paren group has a corresponding an NFA
    // We now handle [*+?], next highest in precedence after parens
    for (int i = 0; i < len; i++) {
        if (info[i].type != UNKNOWN || c[i] == '|') continue;
        int j = i-1; // index of whatever we're binding to
        while (info[j].type == SKIP && j>=0) j--;
        if (info[j].type == UNKNOWN || j<0) die ("could not parse regex");
        info[j].type = SKIP; // it can only be bound once
        info[i].type = REGEX;
        // which one is it?
        if (c[i] == '*') {
            info[i].n = NFA_create_star(info[j].n);
        } else if (c[i] == '+') { // because /a+/ is equivalent to /aa*/
            NFA *s = NFA_create_star(info[j].n);
            info[i].n = NFA_create_concat(info[j].n, s);
            NFA_destroy(s);
        } else if (c[i] == '?') { // because /a?/ is equivalent to /a|ɛ/
            info[i].n = NFA_create_or(info[j].n, e);
        }
    }

    // Now, we handle concatenation of adjacent regexes
    int p = -1; // index of the previous NFA we're concatenating to
    for (int i = 0; i < len; i++) {
        if (info[i].type == SKIP) continue;
        if (info[i].type == UNKNOWN) {p = -1; continue;}
        // by process of elimination, we have a REGEX
        if (p != -1) { // we've already seen one, so concatenate
            info[p].type = SKIP;
            NFA *temp = info[i].n;
            info[i].n = NFA_create_concat(info[p].n, temp);
            NFA_destroy(temp);
        }
        p = i;
    }

    // Finally, we handle |. If nothing is found on the side, epsilon is assumed.
    for (int i = 0; i < len; i++) {
        if (info[i].type != UNKNOWN) continue;
        if (c[i] != '|') continue;
        int prev = i-1, next = i+1;
        while (info[prev].type == SKIP && prev >= 0) prev--;
        while (info[next].type == SKIP && next < len) next++;
        if (prev >= 0) info[prev].type = SKIP;
        if (next < len) info[next].type = SKIP;
        NFA *l = (prev<0)? e : info[prev].n;
        NFA *r = (next==len)? e : info[next].n;
        info[i].n = NFA_create_or(l,r);
        info[i].type = REGEX;
    }

    NFA_destroy(e);
    NFA *n = NULL;
    for (int i = 0; i < len; i++) {
        if (info[i].type == REGEX) {
            if (n == NULL) n = info[i].n; // save the one we want to return
            // there should only be one left
            else die("If you see this error message, Forrest is an idiot");
            continue;
        }
        // free temporary NFAs from the construction
        if (info[i].n != NULL) NFA_destroy(info[i].n);
    }
    return n;
}

void die(const char *s) {
    printf("%s\n", s);
    exit(1);
}

int distanceToMatch(const char *in) {
    const char *s = in;
    int unmatched = 1;
    while(unmatched > 0) {
        switch(*++s) {
            case '(': unmatched++; break;
            case ')': unmatched--; break;
        }
    }
    return s-in;
}

int verifyParens(const char *s) {
    int i = 0;
    while (*s != '\0') {
        switch(*s) {
            case '(': i++; break;
            case ')': i--; break;
        }
        if (i < 0) return 0;
        s++;
    }
    return i == 0;
}
