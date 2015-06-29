/* Reverse polish notation calculator */
%{
#define YYSTYPE double
#include <stdio.h>
#include <math.h>
%}

%token NUM

%% /* Grammar rules and actions follow */

input:   /* empty */
    | input line
;

line:    '\n'
    | exp '\n' { printf("%t%.10g\n", $1); }
;

exp:    NUM {$$=$1;}
    | exp exp '+'   {$$=$1+$2;}
    | exp exp '-'   {$$=$1-$2;}
    | exp exp '*'   {$$=$1*$2;}
    | exp exp '/'   {$$=$1/$2;}
    | exp exp '^'   {$$=power($1, $2);}
    | exp 'n'       {$$=-$1;}
;

%%
