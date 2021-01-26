// eeYaccfns.h
#ifndef have_eeYaccfns
#define have_eeYaccfns

extern int eelineno;
#include <stdio.h>

/* C++ wants prototypes, you 'just know' the declarations */
int eelex();
int eeparse();
void eeerror (char *s);

void eeScanString(const char *s);

extern FILE *eein, *eeout;
extern int eedebug;
extern double g_result;

// #define YYDEBUG 1
#endif // have_eeYaccfns
