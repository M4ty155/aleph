%{
    #include "aleph-v5.tab.h"
    #include <string.h>
%}
%option noyywrap

%%
"+"     {return OP_UNION;}
"*"     {return OP_INTERSEC;}
"-"     {return OP_DIF;}
"="     {return ASIGN;}
"in"    {return OP_PER;}
"push"  {return OP_PUSH;}
"pop"   {return OP_POP;}
"["		{return CORA;}
"]" 	{return CORC;}
"{"		{return LLAVEA;}
"}"		{return LLAVEC;}
"("		{return PARA;}
")"		{return PARC;}
","		{return COMA;}
":"		{return DOSPTOS;}
";"		{return PTOYCOMA;}
"if"  		{return IF;}
"foreach"  	{return FOREACH;}
"while"  	{return WHILE;}
"else"		{return ELSE;}
"&&"		{return AND;}
"||"		{return OR;}
"!"	        {return NOT;}
"=="		{return EQUAL;}
"!="		{return NOTEQUAL;}
"<"		    {return SMALLER;}
">"	        {return HIGHER;}
"<="		{return SMEQ;}
">="		{return HGEQ;}

"_"[A-Za-z][a-zA-Z0-9]* {yylval.s = lookup(yytext);return IDENT;}
[A-Za-z0-9]+ {yylval.cad = strdup(yytext);return ELEM;}
\n          {return EOL; }

[ \t] { /* ignora espacios en blanco */ }
. {printf("caracter raro %c\n", *yytext);}
%%
