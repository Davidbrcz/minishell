%token WORD PIPE IN OUT QWORD
%type<str> cmd 
%type<str> WORD
%type<str> QWORD
%type<str> args
%type<str> arg_word
%type<str> cmd_out
%type<str> cmd_in
%type<str> cmd_begin_pipe
%type<str> cmd_end_pipe
%start prog

%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bison.h"
%}
%union {
  char* str;
}
%%
prog: line;
line : cmd_solo | pipe_line |;

cmd_solo: cmd | cmd_in | cmd_out | cmd_inout;

cmd_begin_pipe : cmd | cmd_in;
cmd_end_pipe : cmd | cmd_out;

cmd_in : cmd IN WORD //{printf("in file = %s \n",$<str>3);}
;

cmd_inout : cmd OUT WORD IN WORD //{printf("in=%s\nout=%s\n",$<str>5,$<str>3);}
          | cmd IN WORD OUT WORD //{printf("in=%s\nout=%s\n",$<str>3,$<str>5);}
;

cmd_out : cmd OUT WORD //{printf("out file = %s \n",$<str>3);}
;

cmd : WORD args //{printf("cmd(%d) = %s\n",j++,$$);}
;

arg_word : WORD | QWORD
;

args: args arg_word  //{printf("arg(%d) = %s\n",++i,$<str>2);} 
        | {$$="";}
;

pipe_line : cmd_begin_pipe pipe_cmd  ;
pipe_cmd : PIPE cmd pipe_cmd  | PIPE cmd_end_pipe ;

%%

void yyerror(char *s) {
    fprintf(stderr, "The input is not valid. Error = %s\n",s);
}
