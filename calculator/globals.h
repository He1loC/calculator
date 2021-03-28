#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/* tokens */
typedef enum
{
	ENDFILE, ERROR,
	/* reserved words */
	INT, FLOAT, WRITE,
	/* multicharacter tokens */
	ID, INTNUM, FLOATNUM,
	/* special symbols */
	ASSIGN, PLUS, MINUS, TIMES, OVER, LPAREN, RPAREN, SEMI, DOT
} TokenType;


/* states in scanner DFA */
typedef enum
{
	START, INID, INNUM, INNUM2, INNUM3, DONE
}StateType;

/* Node kind in parse */
typedef enum
{
	ProK, StmtseqK, StmtseqtailK, StmtK,
	DecalK, //attr type
	AssignK,	//attr type value
	WriteK,		//attr name
	ExprK, ExprtailK, TermK, TermtailK, FactorK, NumberK	//attr inh syn val
} NodeKind;
//typedef enum { DecalK, AssignK, FuncK } StmtKind;
//typedef enum { OpK, ConstK, IdK } ExpKind;
//typedef enum { IntK, FloatK}DecKind;

/* ExpType is used for type checking */
typedef enum { VoidT, ExpIntT, ExpFloatT } ExpType;

#define MAXCHILDREN 4

typedef struct treeNode
{
	struct treeNode* child[MAXCHILDREN];
	struct treeNode* sibling;
	int lineno;
	NodeKind nodekind;

	float inh;
	float syn;

	ExpType type; /* for type checking of exps */
} TreeNode;

#define NAMEMAXLEN 100

typedef struct symbolTableItem
{
	char name[NAMEMAXLEN+1];
	ExpType type;
	float val;
	int initial;
	struct symbolTableItem* next;
}SymbolTableItem, *SymbolTable;

extern int lineno;
extern FILE* fpSrc;
extern FILE* fpOut;

extern const int LexTrace;
extern SymbolTable pSymbolTable;