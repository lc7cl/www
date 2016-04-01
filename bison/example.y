%{ 
#include <stdio.h>

extern int yylex();
void yyerror(const char* msg);
int num_lines = 0, num_chars = 0;
%}

%token WWW1
%token WWW2

%%

SP: SP '' | '';

rules: rule | rules rule | rules SP;

rule: WWW1 | WWW2 {printf("hello\n");};

%%

void yyerror(const char* msg)
{
    printf("%s", msg);
}

int main()
{

yyparse();


return 0;

}
