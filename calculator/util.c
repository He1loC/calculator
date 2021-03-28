#include "globals.h"
#include "lex.h"
#include "parse.h"
#include "util.h"

void printToken(TokenType token, const char* tokenString)
{
	switch (token)
	{
	case INT:
		fprintf(fpOut,
			"reserved word: %s\n", tokenString);
		break;
	case FLOAT:
		fprintf(fpOut,
			"reserved word: %s\n", tokenString);
		break;
	case WRITE:
		fprintf(fpOut,
			"reserved word: %s\n", tokenString);
		break;
	case ASSIGN: fprintf(fpOut, "=\n"); break;
	case LPAREN: fprintf(fpOut, "(\n"); break;
	case RPAREN: fprintf(fpOut, ")\n"); break;
	case SEMI: fprintf(fpOut, ";\n"); break;
	case PLUS: fprintf(fpOut, "+\n"); break;
	case MINUS: fprintf(fpOut, "-\n"); break;
	case TIMES: fprintf(fpOut, "*\n"); break;
	case OVER: fprintf(fpOut, "/\n"); break;
	case ENDFILE: fprintf(fpOut, "EOF\n"); break;
	case INTNUM:
		fprintf(fpOut,
			"INTNUM, val= %s\n", tokenString);
		break;
	case FLOATNUM:
		fprintf(fpOut,
			"FLOATNUM, val= %s\n", tokenString);
		break;
	case ID:
		fprintf(fpOut,
			"ID, name= %s\n", tokenString);
		break;
	case ERROR:
		fprintf(fpOut,
			"ERROR: %s\n", tokenString);
		break;
	case DOT:
		fprintf(fpOut, ".\n"); break;
	default: /* should never happen */
		fprintf(fpOut, "Unknown token: %d\n", token);
	}
}

SymbolTable initSymbolTable(void)
{
	SymbolTable p;
	//initial head node
	p = (SymbolTableItem*)calloc(1, sizeof(SymbolTableItem));

	if (p != NULL)
	{
		p->next = NULL;
	}
	else
	{
		fprintf(stderr, "initSymbolTable failure!\n");
		exit(EXIT_FAILURE);
	}

	return p;
}

void insertSymbolTable(char* name, ExpType type)
{
	SymbolTableItem* p = pSymbolTable, * pt;

	while (p->next != NULL)
	{
		p = p->next;
	}

	pt = (SymbolTableItem*)calloc(1, sizeof(SymbolTableItem));
	if (pt!=NULL)
	{
		strcpy_s(pt->name, NAMEMAXLEN, name);
		pt->type = type;
		pt->initial = 0;
		pt->next = NULL;
		p->next = pt;
	}
	else
	{
		fprintf(stderr, "insertSymbolTable failure!\n");
		exit(EXIT_FAILURE);
	}

	return;
}

SymbolTableItem* lookupSymbolTable(char* name)
{
	SymbolTableItem* p = pSymbolTable->next;

	while (p != NULL)
	{
		if (strcmp(p->name, name) == 0)
		{
			return p;
		}
		else
		{
			p = p->next;
		}
	}

	if (p == NULL)
	{
		return NULL;
	}

}
