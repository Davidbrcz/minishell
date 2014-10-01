%{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
void yyerror(char *);
#include "gram.tab.h"
char *strdup(const char *s);
int fileno(FILE *stream);
%}
%option nounput
%option noinput
%%

[-a-zA-Z0-9_\./]+ { yylval.str = strdup(yytext); return WORD;}

\"([^\\\"]|\\.)*\" {  yylval.str = strdup(yytext); return QWORD;}

"|" {return PIPE;}
"<" {return IN;}
">" {return OUT;}

"\n" { return *yytext; } 
[ \t] ;
. yyerror("invalid character");
%%
int yywrap(void) {
return 1;
} 
