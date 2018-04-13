#ifndef _PARSER_H_
#define _PARSER_H_

#include "NFA.h"

/* we will use a flag buffer of types to avoid copying the string as we reduce it */
typedef enum { REGEX, SKIP, UNKNOWN } type;

typedef struct {
    type type;
    NFA *n;
} Info;

/* forcefully exit with an error message */
void die(const char *);
/* checks for balanced parenthesis in a null-terminated string */
int verifyParens(const char *);
/* number of characters to the closing paren */
int distanceToMatch(const char *s);

NFA *parse(char *c, int len);

#endif
