#ifndef BISON_H 
#define BISON_H 

typedef struct yy_buffer_state* YY_BUFFER_STATE;
extern int yyparse();
extern YY_BUFFER_STATE yy_scan_string(char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
void yyerror(char *);
int yylex(void);
extern int yylineno;
extern char* yytext;

#endif
