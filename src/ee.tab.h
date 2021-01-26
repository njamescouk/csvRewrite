#define UNARYMINUS 257
#define NUMBER 258
#define LEXERR 259
typedef union
{
   /* types available to grammar */
   double uNumber;
} YYSTYPE;
extern YYSTYPE eelval;
