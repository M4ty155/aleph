#ifndef ALEPHV_H
#define ALEPHV_H
#include "tData.h"

#define OPE_HIGHER 4
#define OPE_SMALLER 5
#define OPE_SMEQ 6
#define OPE_HGEQ 7
#define OPE_AND 8
#define OPE_OR 9
#define OPE_NOT 10
#define OPE_PUSH 11
#define OPE_POP 12
#define OPE_SORT 13
#define OPE_IF 14
#define OPE_FOREACH 15
#define OPE_WHILE 16
#define OPE_ELSE 17
#define OPE_UNION 18
#define OPE_DIF 19
#define OPE_INTER 20
#define OPE_TAM 21
#define OPE_PER 22
#define OPE_ASIGN 23
#define OPE_EQUAL 24
#define OPE_NOTEQUAL 25
#define OPE_REF 26
#define OPE_LIST_EXPR 29
#define OPE_LIST_IDENT 30
#define INT_IF 31
#define INT_WHILE 32

struct ast{
	int nodetype; 
	struct ast *l;
	struct ast *r;
};

struct cadena{
	int nodetype;
	char* cad; 
};

/* tabla de símbolos */
struct symbol{
    char *name;
    tData value;
};

struct symasgn{
    int nodetype;
    struct symlist *s;
    struct ast *v;
};

struct symlist{
    struct symbol *sym;
    struct symlist *next;
};


struct symref{
    int nodetype;
    struct symbol *s;
};

struct flow{
    int nodetype;
    struct ast *cond;   
    struct ast *tl;     
    struct ast *el;     
};

#define NHASH 9997
struct symbol symtab[NHASH];
struct symbol *lookup(char*);
static unsigned symhash(char *);
struct ast *newasgn(struct symlist *s, struct ast *v);
struct symlist *newsymlist(struct symbol *, struct symlist *);
struct ast *newast(int,struct ast*,struct ast*);
struct ast *newCadena(char*);
struct ast *newref(struct symbol *s);
tData eval(struct ast*);
tData astLASTotData(struct ast*);
struct ast *newflow(int, struct ast*, struct ast*, struct ast*);
void treefree(struct ast*);

#endif
