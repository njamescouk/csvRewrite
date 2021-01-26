%{
/* C definitions */
#pragma warning( disable : 4786 )
#pragma warning( disable : 4996 )
#include <stdlib.h>
#include <vector>
#include <string>
#include "ParsedCsv.h"
#include "pcsv.tab.h"
#include "pcsvfns.h"

#ifdef WIN32
#include <io.h>
#endif

#define YY_USE_PROTOS
int lineno = 1;
extern int yydebug;
int seenEOF = 0;

void scanString(const char *s)
{
    yy_scan_string(s);
}

%}

%s IN_QUOTE

/* regular definitions */
space [ \t]
ws    {space}+

%%
\n              { 
                    // fprintf(stderr, "newline\n");
                    lineno++; 
                    if (YY_START == INITIAL)
                    {
                        //fprintf(stderr, "NEWLINE %d\n", lineno);
                        return NEWLINE;
                    }
                    else if (YY_START == IN_QUOTE)
                    {
                        //fprintf(stderr, "NEWLINE_IN_QUOTE %d\n", lineno);
                        return NEWLINE_IN_QUOTE;
                    }
                    else
                    {
                        //fprintf(stderr, "NEWLINE LEXERR %d\n", lineno);
                        return LEXERR;
                    }
                }
                
{ws}            { 
                    // fprintf(stderr, "ws ");
                    pcsvlval.csvStr = new std::string;
                    *pcsvlval.csvStr = yytext;
                    
                    if (YY_START == INITIAL)
                    {
                        /* WS */
                        return WS;
                    }
                    else if (YY_START == IN_QUOTE)
                    {
                        /* WS_IN_QUOTE */
                        return WS_IN_QUOTE;
                    }
                    else
                    {
                        return LEXERR;
                    }
                }

\x22            { 
                    /* fprintf(stderr, "quote "); */
                    if (YY_START == INITIAL)
                    {
                        BEGIN (IN_QUOTE);
                        /* fprintf(stderr, "INITIAL quote\n"); */
                        return QUOTE; 
                    }
                    else if (YY_START == IN_QUOTE)
                    {
                        /* 
                        we've seen an opening quote ages ago 
                        and have just got another one.
                        if it's doubled ("") it's an escaped quote
                        otherwise end of quote
                        */
                        int newState = 0;
                        int c = yyinput();
                        if (c == '"')
                        {
                            /* ie we've got "" */
                            //fprintf(stderr, "ESCAPED_QUOTE\n");
                            newState = ESCAPED_QUOTE;
                        }
                        else
                        {
                            if (c > 0)
                               unput(c);
                            newState = END_QUOTE;
                            //fprintf(stderr, "END_QUOTE\n");
                            BEGIN (INITIAL);
                        }

                        return newState; 
                    }

                    return LEXERR; 
                }
    
,               { 
                    // fprintf(stderr, "comma ");
                    if (YY_START == INITIAL)
                    {
                        return COMMA;
                    }
                    else if (YY_START == IN_QUOTE)
                    {
                        return COMMA_IN_QUOTE;
                    }
                    
                    return LEXERR;
                }
            
[^\x22\n \t,]+  { 
                    // fprintf(stderr, "str ");
                    /* 
                    a string is a run of chars that aren't whitespace, 
                    quote or comma 
                    these chars are dealt with above
                    */
                    pcsvlval.csvStr = new std::string;
                    *pcsvlval.csvStr = yytext;
                    return STR;
                }
                
<<EOF>>         {
                    // fprintf(stderr, "eof\n");

                    /* we fake this to get round the no newline 
                    at end of file problem */
                    if (seenEOF == 0)
                    {
                        seenEOF = 1;
                        return END_OF_FILE;
                    }
                    else
                    {
                        yyterminate();
                    }
                }

%%
/* user code */

int yywrap (void)
{
   return 1;
}   
