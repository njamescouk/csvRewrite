%{

/* C declarations */
#pragma warning( disable : 4786 )
#pragma warning( disable : 4996 )
#include <stdio.h>
#include <stdlib.h>
#include "ParsedCsv.h"
#include "JulianDay.h"
#include <vector>
#include <string>
#include "pcsvfns.h"

extern int lineno;
void yyerror (char *s);
extern char *pcsvtext;
int parserVersion = 1;
ParsedCsv *pgResult = 0;

%}
/* token declarations */
%token COMMA_IN_QUOTE
%token COMMA
%token END_QUOTE
%token ESCAPED_QUOTE
%token QUOTE
%token STR
%token NEWLINE_IN_QUOTE
%token NEWLINE
%token WS_IN_QUOTE
%token WS
%token END_OF_FILE
%token LEXERR


%left WS
%nonassoc COMMA

%union 
{
    std::string *csvStr;
    CsvField *csvField;
    CsvRecord *csvRecord;
}

%type <csvStr> STR
%type <csvStr> WS
%type <csvStr> WS_IN_QUOTE
%type <csvStr> emptyField
%type <csvStr> fieldComponent
%type <csvField> field
%type <csvField> quotedField
%type <csvField> quotedFieldAndSpace
%type <csvRecord> fieldList
%type <csvRecord> record

%%
/* translation rules */
csv
    : recordList
    {
    }
    ;

recordList
    : record
    {
        if (!$1->isEmpty())
            pgResult->push_back(*$1);
        
        delete $1;
    }
    | recordList record
    {
        if (!$2->isEmpty())
             pgResult->push_back(*$2);
        
        delete $2;
    }
    ;

record
    : fieldList NEWLINE
    {
        $$ = $1;
        
        // fprintf(stderr, "line: %d\nrecord: fieldList NEWLINE\n", lineno);
    }
    | fieldList END_OF_FILE
    {
        $$ = $1;
        
        // fprintf(stderr, "line: %d\nrecord: fieldList END_OF_FILE\n", lineno);
    }
    ;
    
fieldList
    : field
    {
        /* field : field */
        $$ = new CsvRecord;
        $$->push_back(*$1);
        
        delete $1;

        // fprintf(stderr, "line: %d\nfieldList: field\n", lineno);
    }
    | emptyField
    {
        /* field : emptyField */
        $$ = new CsvRecord;
        $$->push_back(CsvField(*$1, false, CSV_FIELD_TYPE_ANY));
        
        delete $1;

        // fprintf(stderr, "line: %d\nfieldList: emptyField\n", lineno);
    }
    | quotedFieldAndSpace
    {
        /* field : quotedFieldAndSpace */
        $$ = new CsvRecord;
        $$->push_back(*$1);
        
        delete $1;

        // fprintf(stderr, "line: %d\nfieldList: quotedFieldAndSpace\n", lineno);
    }
    | fieldList COMMA emptyField
    {
        /* field : fieldList COMMA emptyField */
        $$ = $1;
        $$->push_back(CsvField(*$3, false, CSV_FIELD_TYPE_ANY));
        
        delete $3;
        
        // fprintf(stderr, "line: %d\nfieldList: fieldList COMMA emptyField\n", lineno);
    }
    | fieldList COMMA field
    {
        /* field : fieldList COMMA field */
        $$ = $1;
        $$->push_back(*$3);
        
        delete $3;
        
        // fprintf(stderr, "line: %d\nfieldList: fieldList COMMA field\n", lineno);
    }
    | fieldList COMMA quotedFieldAndSpace
    {
        /* field : fieldList COMMA quotedFieldAndSpace */
        $$ = $1;
        $$->push_back(*$3);
        
        delete $3;
        
        // fprintf(stderr, "line: %d\nfieldList: fieldList COMMA quotedFieldAndSpace\n", lineno);
    }
    ;
    
quotedFieldAndSpace
    : quotedField
    {
        $$ = $1;
    }
    | WS quotedField
    {
        $$ = $2;
    }
    | quotedField WS
    {
        $$ = $1;
    }
    | WS quotedField WS
    {
        $$ = $2;
    }
    ;
    
quotedField
    : QUOTE field END_QUOTE
    {
        /* quotedField : QUOTE field END_QUOTE */
        if ($2 != 0)
        {
            CSV_FIELD_TYPE ty = CSV_FIELD_TYPE_DATE;
            if ($2->getType() != CSV_FIELD_TYPE_DATE)
                ty = CSV_FIELD_TYPE_TEXT;

            $$ = new CsvField($2->c_str(), true, ty);
            delete $2;
        }
        
        // fprintf(stderr, "line: %d\nquotedField: QUOTE field END_QUOTE\n", lineno);
        // fprintf(stderr, "$$ = >|%s|<\n", $$->c_str());
    }
    | QUOTE END_QUOTE
    {
        /* quotedField : QUOTE END_QUOTE */
        $$ = new CsvField("", true, CSV_FIELD_TYPE_EMPTY_TEXT);
        
        // fprintf(stderr, "line: %d\nquotedField: QUOTE END_QUOTE\n", lineno);
        // fprintf(stderr, "$$ = >|%s|<\n", $$->c_str());
    }
    ;
    
field
    : fieldComponent
    {
        /* field : fieldComponent */
        if ($1 != 0)
        {
            CSV_FIELD_TYPE ty = CSV_FIELD_TYPE_TEXT;
            std::string fc = *$1;

            bool hasCharsInconsistentWithInteger 
                 = (fc.find_first_not_of("-0123456789") < std::string::npos);
            if (hasCharsInconsistentWithInteger)
            {
                // might be real
                bool hasCharsInconsistentWithReal 
                     = (fc.find_first_not_of("-0123456789eE.") < std::string::npos);
                if (!hasCharsInconsistentWithReal)
                {
                    char *end = 0;
                    strtod(fc.c_str(), &end); 
                    if (end - fc.c_str() == fc.end() - fc.begin())  // nnnyeees
                        ty = CSV_FIELD_TYPE_REAL;
                }
            }
            else
            {
                JulianDay jd(*$1); // not ideal - we're doing this again in CsvField()
                if (jd.getErr().empty())
                {
                    ty = CSV_FIELD_TYPE_DATE;
                }
                else
                {
                    size_t dashPos = fc.find("-");
                    if (dashPos == std::string::npos || dashPos == 0)
                        ty = CSV_FIELD_TYPE_INTEGRAL;
                    else
                        ty = CSV_FIELD_TYPE_TEXT;
                }
            }

            $$ = new CsvField($1->c_str(), false, ty);
            delete $1;
        }    
        // fprintf(stderr, "line: %d\nfield: fieldComponent\n", lineno);
    }
    | field fieldComponent
    {
        /* field : field fieldComponent */
        /* field : fieldComponent */
        if ($1 != 0 && $2 != 0)
        {
            $$ = $1;
            *$$ += *$2;
            $$->setType(CSV_FIELD_TYPE_TEXT);
            delete $2;
        }

        // fprintf(stderr, "line: %d\nfield: field fieldComponent\n", lineno);
    }
    ;
    
fieldComponent
    : WS
    {
        $$ = new std::string;
        *$$ = *$1;
        
        delete $1;
        
        // fprintf(stderr, "line: %d\nfieldComponent: WS\n", lineno);
    }
    | STR
    {
        $$ = new std::string;
        *$$ = *$1;
        
        delete $1;
        
        // fprintf(stderr, "line: %d\nfieldComponent: STR\n", lineno);
    }
    | ESCAPED_QUOTE
    {
        $$ = new std::string;
        *$$ = "\"";
        
        // fprintf(stderr, "line: %d\nfieldComponent: ESCAPED_QUOTE\n", lineno);
    }
    | COMMA_IN_QUOTE
    {
        $$ = new std::string;
        *$$ = ",";
        
        // fprintf(stderr, "line: %d\nfieldComponent: COMMA_IN_QUOTE\n", lineno);
    }
    | WS_IN_QUOTE
    {
        $$ = new std::string;
        *$$ = *$1;
        
        delete $1;
        
        // fprintf(stderr, "line: %d\nfieldComponent: WS_IN_QUOTE\n", lineno);
    }
    | NEWLINE_IN_QUOTE
    {
        $$ = new std::string;
        *$$ += "\n";
        
        // fprintf(stderr, "line: %d\nfieldComponent: NEWLINE_IN_QUOTE\n", lineno);
    }
    ;
    
emptyField
    : /* empty */
    {
        $$ = new std::string;
        
        // fprintf(stderr, "line: %d\nemptyField: empty\n", lineno);
    }
    ;
    
%%
/* user code */

void yyerror (char *s)
{
   fprintf (stderr, "%s at or before line %d:\nreading \"%s\"\n", s, lineno, pcsvtext);
}

/*
int main()
{
    int res = yyparse();
    printf("%d\n", res);
    if (res == 0)
    {
        printf("%s\n", gResult.ToString().c_str());
    }

    return res;
}
*/

