%{
	#include <stdio.h>
    #include "alephv.h"
   	#include "lex.yy.c"
    int yyerror(char*s);
%}

%union{
    struct ast*a;
    char* cad;
    struct symbol *s;
    struct symlist *sl;
}

%token OP_UNION OP_INTERSEC OP_DIF OP_PER ASIGN
%token CORA CORC LLAVEA LLAVEC PARA PARC COMA PTOYCOMA DOSPTOS
%token IF FOREACH WHILE ELSE
%token AND OR NOT EQUAL NOTEQUAL HIGHER SMALLER SMEQ HGEQ
%token OP_PUSH OP_POP 
%token <cad>  ELEM
%token <s>  IDENT
%token EOL

%left OP_UNION OP_INTERSEC OP_DIF
%left EQUAL NOTEQUAL SMALLER HIGHER SMEQ HGEQ
%left OR AND
%right NOT

%type <a>  asignacion exprA exprConj exprList exprLog exprRel set list sentencia lista_expr aleph alternativa bloque lista_sent bucle while_sent
%type <sl> lista_id
%%


aleph: 
    |aleph sentencia EOL {printData(eval($2));printf("\n->")}
    ;

sentencia: exprA {$$=$1;}
    |alternativa 
    |bucle
    |asignacion {$$=$1;}
    ;

asignacion: lista_id ASIGN lista_expr {$$ = newasgn($1,$3);}
		;

lista_id:IDENT  {$$ = newsymlist($1,NULL);}
        |IDENT COMA lista_id {$$ = newsymlist($1,$3);}
        ;

lista_expr:exprA {$$ = newast(OPE_LIST_EXPR,$1,NULL);}
	    |exprA COMA lista_expr {$$ = newast(OPE_LIST_EXPR,$1,$3);}
	    ;
		
alternativa : IF PARA  exprLog PARC bloque				{$$ = newflow(OPE_IF, $3, $5, NULL);}
		|IF PARA exprLog PARC bloque ELSE bloque		{$$ = newflow(OPE_IF, $3, $5, $7);}
		;

bloque:                                 {$$ = NULL}
        |LLAVEA lista_sent LLAVEC       {$$ = $2;}
        ;

lista_sent: lista_sent PTOYCOMA sentencia {if($3 == NULL)
                                            $$ = $1;
                                           else
                                            $$ = newast(LIST, $1, $3);
                                           }
           |sentencia                      {$$ = $1;}
           ;
	
bucle: while_sent
      |for_each_sent
      ;

while_sent: WHILE PARA exprLog PARC bloque {$$ = newflow(OPE_WHILE, $3, $5, NULL);}
        ; 
			
for_each_sent: FOREACH PARA IDENT DOSPTOS exprA PARC bloque
            ;

exprA: exprList                             {$$ = $1;}
    |exprConj                               {$$ = $1;}
    |list                                   {$$ = $1;}
    |set                                    {$$ = $1;}
    |ELEM                                   {$$ = newCadena($1);}
    |IDENT                                  {$$ = newref($1);}
    ;

exprList: OP_PUSH PARA exprA COMA exprA PARC   {$$ = newast(OPE_PUSH,$3,$5);}
        |OP_POP PARA exprA PARC                {$$ = newast(OPE_POP,$3,NULL);}
        ;    

exprConj: exprA OP_UNION exprA              {$$ = newast(OPE_UNION,$1,$3);}
        |exprA OP_DIF exprA                 {$$ = newast(OPE_DIF,$1,$3);}
        |exprA OP_INTERSEC exprA            {$$ = newast(OPE_INTER,$1,$3);}
        ;

exprRel: exprA EQUAL exprA                   {$$ = newast(OPE_EQUAL,$1,$3);}
        |exprA NOTEQUAL exprA                {$$ = newast(OPE_NOTEQUAL,$1,$3);}
        |exprA SMALLER exprA                 {$$ = newast(OPE_SMALLER,$1,$3);}
        |exprA HIGHER exprA                  {$$ = newast(OPE_HIGHER,$1,$3);}
        |exprA OP_PER exprA                  {$$ = newast(OPE_PER,$1,$3);}
        |exprA SMEQ exprA                    {$$ = newast(OPE_SMEQ,$1,$3);}
        |exprA HGEQ exprA                    {$$ = newast(OPE_HGEQ,$1,$3);}
        ;

exprLog: NOT exprLog            
        |exprLog OR exprLog     
        |exprLog AND exprLog   
        |exprRel               
        ;

set : LLAVEA LLAVEC                       {$$=newast(SET,NULL,NULL);}
	|LLAVEA lista_expr LLAVEC             {$$=newast(SET,$2,NULL);}
	;

list: CORA CORC                           {$$=newast(LIST,NULL,NULL);}
	|CORA lista_expr CORC                 {$$=newast(LIST,$2,NULL);}
    ;
%%

int main(){
    yyparse();
    return 0;
}

int yyerror(char*s){
    fprintf(stderr, "error: %s\n",s);
    return 0;
}