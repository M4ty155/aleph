#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alephv.h"
#include "tData.h"

struct ast *newast(int nodetype, struct ast *l, struct ast *r){
	struct ast *a = malloc(sizeof(struct ast));
	
	if(!a){
		printf("No hay memoria\n");
		exit(1);
	}
	
	a->nodetype = nodetype;
	a->l = l;
	a->r = r;
	return a;
}
	
struct ast *newCadena(char*cad){
	struct cadena*a = malloc(sizeof(struct cadena));
	
	if(!a){
		printf("No hay memoria\n");
		exit(1);
	}
	
	a->nodetype = ATM;
	a->cad=strdup(cad); 
	return(struct ast*)a;
}
		
tData eval(struct ast*a){
	tData arbol;
	tData aux;
	tData lista;
	struct ast * auxAr;
	struct ast* auxRecIdent;
	struct ast* auxRecExp;
	struct ast* der;
	struct symlist* izq;
	if(!a)
		return NULL;
	switch(a->nodetype){

		case OPE_IF:
			printf("\nif type: %d",((struct flow *)a)->cond->nodetype);
			printf("\nLo que tiene atom: %s",eval(((struct flow *)a)->cond)->atom);
            if(eval(((struct flow *)a)->cond)->atom == "true"){
                if(((struct flow *)a)->tl){
                    arbol = eval(((struct flow *)a)->tl);
                }else
                    arbol = NULL;        /* valor por defecto */
            }else{
                if(((struct flow *)a)->el){
                    arbol = eval(((struct flow *)a)->el);
                }else
                    arbol = NULL;
            }
            break;
			/* while - do */
        case OPE_WHILE:
            arbol = NULL;        /* valor por defecto */
            
            if(((struct flow *)a)->tl){
                while(eval(((struct flow *)a)->cond) != 0)
                    arbol = eval(((struct flow *)a)->tl);
            }
            break;
		case OPE_ASIGN:
			lista =newData("[]");
			der=a->r;
			while(der!=NULL){
				PUSH(lista,eval(der->l));
			der=der->r;
			}
			izq=(struct symlist*)a->l;
			while(izq!=NULL && !isEmpty(lista)){
				arbol=POP(&lista);//cp
				//printf("\n%d",izq->nodetype);
				izq->sym->value=arbol;//cp
				izq=izq->next;
			}
		break;
		case OPE_REF:
			arbol = ((struct symref*)a)->s->value;//cp
		break;

		case OPE_LIST_EXPR:
			arbol = eval(a->l);
		break;
		case SET: case ATM: case LIST:
			arbol = astLASTotData(a);
		break;
		case OPE_UNION:
			arbol = UNION(eval(a->l),eval(a->r));
		break;
		case OPE_INTER:
			arbol = INTER(eval(a->l),eval(a->r));
		break;
		case OPE_DIF:
			arbol = DIFF(eval(a->l),eval(a->r));		
		break;
		case OPE_PUSH:
			arbol=eval(a->l);
			PUSH(arbol,eval(a->r));
		break;
		case OPE_POP:
			//printf("\nTengo un: %d\n",a->l->nodetype);
			aux=eval(a->l);
			arbol=POP(&aux);
			((struct symref*)(a->l))->s->value=aux;
//			a->l=(struct ast *)aux;
		break;
		case OPE_EQUAL:
			if(isEqual(eval(a->l),eval(a->r))){ //retorna 1
				arbol=newData("true");
			}	
			else{
				arbol=newData("false");
			}
		break;
		case OPE_NOTEQUAL:
			if(!isEqual(eval(a->l),eval(a->r))){//retorna 0
				arbol=newData("true");
			}
			else{
				arbol=newData("false");
			}
		break;
		default:
			printf("Operacion no reconocida");
		break;
	}
	return arbol;
}
			
struct ast *newflow(int nodetype, struct ast *cond, struct ast *tl, struct ast *el){

    struct flow *a = malloc(sizeof(struct flow));
    
    if(!a){
        //yyerror("no hay espacio");
        exit(1);
    }
    
    a->nodetype = nodetype;
    a->cond = cond;
    a->tl = tl;
    a->el = el;
    return (struct ast *)a;
}

tData astLASTotData(struct ast *a){
	tData LAS=NULL;
	tData aux;
	struct ast* auxArbol;
	if(a==NULL){
		return LAS;
	}
	else{
		switch(a->nodetype){
		case SET:
			if(a->l!=NULL){
				LAS=newData("{}");
				auxArbol=a->l;
				while(auxArbol!=NULL){
					aux=newNestedData(astLASTotData(auxArbol->l),SET);
					LAS=UNION(LAS,aux);
					auxArbol=auxArbol->r;
				}
			}
			break;
			
			case LIST: 
				if(a->l!=NULL){
				LAS=newData("[]");
				auxArbol=a->l;
				while(auxArbol!=NULL){
					//aux=newNestedData(astLASTotData(auxArbol->l),SET);
					PUSH(LAS,astLASTotData(auxArbol->l));
					//LAS=UNION(LAS,aux);
					auxArbol=auxArbol->r;
				}
			}
			//printf("\nLo que tiene LAS: \n");								
			//printData(LAS);								
			break;
			case ATM:
			//printf("\ncaso ATM");
			LAS = newData(((struct cadena*)a)->cad);
			//printf("\nEl ATM que retorna: ");
			//printData(LAS);
			break;
			case OPE_LIST_EXPR:
			case OPE_LIST_IDENT:
				LAS=astLASTotData(a->l);
			break;
			case OPE_IF:
			case OPE_UNION:
			case OPE_DIF:
			case OPE_INTER:
			case OPE_PUSH:
			case OPE_POP:
			case OPE_EQUAL:
			case OPE_NOTEQUAL:
			//case OPE_LIST_EXP
			LAS = eval(a);
			break;
		}
	}
	return LAS;
}

static unsigned symhash(char *sym){
    unsigned int hash = 0;
    unsigned c;
    
    while(c = *sym++) hash = hash*9 ^ c;
    
    return hash;
}

struct symbol *lookup(char *sym){//mandas un token y lo busca en la tabla
    struct symbol *sp = &symtab[symhash(sym)%NHASH];
    int scount = NHASH;
    
    while(--scount >= 0){
        if(sp->name && !strcmp(sp->name, sym)) 
            return sp;
        
        if(!sp->name){
            sp->name = strdup(sym);
            sp->value = newData("");
            return sp;
        }
        
        if(++sp >= symtab+NHASH)    /* prueba con la siguiente entrada */
            sp = symtab;
    }
    //yyerror("la tabla de símbolos está agotada\n");
    abort();
}			

struct ast *newasgn(struct symlist *s, struct ast *v){
    struct symasgn *a = malloc(sizeof(struct symasgn));
    
    if(!a){
        //yyerror("no hay espacio");
        exit(0);
    }
    
    a->nodetype = OPE_ASIGN;
    a->s = s;//lista ident
    a->v = v;//lista expr
    return (struct ast *)a;
}

 struct symlist *newsymlist(struct symbol *sym, struct symlist *next){
    struct symlist *sl = malloc(sizeof(struct symlist));
    
    if(!sl){
        //yyerror("no hay espacio");
        exit(0);
    }
    
    sl->sym = sym;
    sl->next = next;
    return sl;
}


struct ast *newref(struct symbol *s){//rec list ident
    struct symref *a = malloc(sizeof(struct symref));
    
    if(!a){
        //yyerror("no hay espacio");
        exit(0);
    }
    
    a->nodetype = OPE_REF;//?
    a->s = s;//
    return (struct ast *)a;
}

