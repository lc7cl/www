%{
#include <stdio.h>   
%}
%%

(?=ab) printf("%s\n", yytext);

%%

int yywrap()
{
    return 1;
}

int main(int argc, char **argv)
{
    yyin = fopen(argv[1], "r");
    if (yyin == NULL)
        return -1;
    yylex();
    return 0;
}
