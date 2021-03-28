#include "globals.h"
#include "lex.h"
#include "util.h"
#include "parse.h"

static TokenType token; //holds current token
int isError = 0;

static void syntaxError(char* message)
{
	fprintf(fpOut, "Error(line %d): %s\n", lineno, message);
	//exit(EXIT_FAILURE);
	//Error = TRUE;
	isError = 1;
}

static void match(TokenType expectedtk)
{
	if (token == expectedtk)
		token = getToken();
	else
	{
		syntaxError("unexpected token -> ");
		printToken(token, tokenString);
	}
}

TreeNode* newNode(NodeKind kind)
{
	TreeNode* t = (TreeNode*)calloc(1, sizeof(TreeNode));
	int i;
	if (t == NULL)
	{
		fprintf(fpOut, "Error(line %d):Out of memory.\n", lineno);
		exit(EXIT_FAILURE);
	}
	else {
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;
		t->sibling = NULL;
		t->nodekind = kind;
		//t->kind.nodek = kind;
		t->lineno = lineno;
	}
	return t;
}


// program 		-> stmt_seq ¡®.¡¯ ENDFILE
static TreeNode* program(void)
{
	TreeNode* t = newNode(ProK);

	t->child[0] = stmt_seq();
	match(DOT);
	if (token == ENDFILE)
		match(ENDFILE);
	else
	{
		syntaxError("Program should end with a '.'");
	}

	return t;//error checking comes later
}

//Stmt_seq 	-> stmt stmt_seq_tail
static TreeNode* stmt_seq(void)
{
	TreeNode* t = newNode(StmtseqK);

	t->child[0] = stmt();
	t->child[1] = stmt_seq_tail();


	return t;
}

//Stmt_seq_tail	-> ; stmt_seq | ¦Å
static TreeNode* stmt_seq_tail(void)
{
	TreeNode* t = newNode(StmtseqtailK);
	if (token == SEMI)
	{
		match(SEMI);
		t->child[0] = stmt_seq();
	}
	else if (token == DOT)
	{
		// epsilon ¦Å  do nothing
	}
	else
	{
		syntaxError("Syntax Error: missing semicolon ';'before");
	}

	return t;
}

//Stmt			-> declaration_stmt|assign_stmt|write_stmt	
static TreeNode* stmt(void)
{
	TreeNode* t = newNode(StmtK);

	switch (token)
	{
	case INT:
	case FLOAT:
		t->child[0] = decal_stmt();
		break;
	case ID:
		t->child[0] = assign_stmt();
		break;
	case WRITE:
		t->child[0] = write_stmt();
		break;
	default:
		syntaxError("Unexpected Token:");
		printToken(token, tokenString);
		token = getToken();
		break;
	}
	return t;
}

//declaration_stmt->float | int id
static TreeNode* decal_stmt(void)
{
	TreeNode* t = newNode(DecalK);
	SymbolTableItem* p = NULL;

	switch (token)
	{
	case FLOAT:
	{
		match(FLOAT);
		if (token == ID)
		{
			p = lookupSymbolTable(tokenString);
			if (p != NULL) //found
			{
				syntaxError("Identifier defined twice!");
				printToken(token, tokenString);
			}
			else// not found
			{
				insertSymbolTable(tokenString, ExpFloatT);
			}
		}
		match(ID);
		break;
	}
	case INT:
	{
		match(INT);
		if (token == ID)
		{
			p = lookupSymbolTable(tokenString);
			if (p != NULL) //found
			{
				syntaxError("Identifier defined twice!");
				printToken(token, tokenString);
			}
			else// not found
			{
				insertSymbolTable(tokenString, ExpIntT);
			}
		}
		match(ID);
		break;
	}

	default:
		//error
		syntaxError("Syntax Error in Declaration!");
		break;
	}

	return t;
}

//assign_stmt->id = expr
static TreeNode* assign_stmt(void)
{
	TreeNode* t = newNode(AssignK);
	SymbolTableItem* p = NULL;

	if (token == ID)
	{
		p = lookupSymbolTable(tokenString);
		if (p == NULL)
		{
			syntaxError("Identifier Undefined!");
			printToken(token, tokenString);
			exit(EXIT_FAILURE);
		}
		else
		{
			match(ID);
			match(ASSIGN);
			t->child[0] = expr();
			//p->val = ==
			p->val = t->child[0]->syn;
			p->initial = 1;
			if (p->type == ExpIntT && t->child[0]->type == ExpFloatT)
			{
				syntaxError("Illegal Conversion:Float -> Int");
			}
		}

	}
	else
	{
		syntaxError("Syntax Error in collapsing assign_stmt!");
	}
	//typechecking
	return t;
}

//write_stmt	-> write(expr)
static TreeNode* write_stmt(void)
{
	TreeNode* t = newNode(WriteK);

	match(WRITE);
	match(LPAREN);
	t->child[0] = expr();
	match(RPAREN);

	//printf value
	if (isError == 0)
	{
		if (t->child[0]->type == ExpIntT)
		{
			fprintf(fpOut, "%d\n", (int)t->child[0]->syn);
		}
		else 
		{
			fprintf(fpOut, "%g\n", t->child[0]->syn);
		}
	}


	return t;
}

//expr			-> term expr_tail
static TreeNode* expr(void)
{
	TreeNode* t = newNode(ExprK);

	t->child[0] = term();
	t->child[1] = expr_tail(t->child[0]->syn);	//expr_tail.inh = term.syn
	t->syn = t->child[1]->syn;

	if (t->child[0]->type == ExpFloatT || t->child[1]->type == ExpFloatT)
	{
		t->type = ExpFloatT;
	}
	else
	{
		t->type = ExpIntT;
	}

	//printf("exp type:%d,line:%d\n", t->type,lineno);

	return t;
}

//expr_tail		-> +term expr_tail | -term expr_tail |¦Å
static TreeNode* expr_tail(float exprTailInh)
{
	TreeNode* t = newNode(ExprtailK);
	t->inh = exprTailInh;

	if (token == PLUS)
	{
		match(PLUS);
		t->child[0] = term();
		t->child[1] = expr_tail(t->inh + t->child[0]->syn);
		t->syn = t->child[1]->syn;
		if (t->child[0]->type == ExpFloatT || t->child[1]->type == ExpFloatT)
		{
			t->type = ExpFloatT;
		}
		else
		{
			t->type = ExpIntT;
		}
	}
	else if (token == MINUS)
	{
		match(MINUS);
		t->child[0] = term();
		t->child[1] = expr_tail(t->inh - t->child[0]->syn);
		t->syn = t->child[1]->syn;
		if (t->child[0]->type == ExpFloatT || t->child[1]->type == ExpFloatT)
		{
			t->type = ExpFloatT;
		}
		else
		{
			t->type = ExpIntT;
		}
	}
	else if (token == SEMI || token == DOT || token == RPAREN)
	{
		// epsilon do nothing
		t->syn = t->inh;
	}
	else
	{
		syntaxError("Syntax Error in collapsing expr_tail!");
	}

	return t;
}

//term			-> factor term_tail
static TreeNode* term(void)
{
	TreeNode* t = newNode(TermK);

	t->child[0] = factor();
	t->child[1] = term_tail(t->child[0]->syn);
	t->syn = t->child[1]->syn;
	if (t->child[0]->type == ExpFloatT || t->child[1]->type == ExpFloatT)
	{
		t->type = ExpFloatT;
	}
	else
	{
		t->type = ExpIntT;
	}

	return t;
}

//term_tail		-> *factor term_tail | /factor term_tail | ¦Å
static TreeNode* term_tail(float termTailInh)
{
	TreeNode* t = newNode(TermtailK);
	t->inh = termTailInh;

	if (token == TIMES)
	{
		match(TIMES);
		t->child[0] = factor();
		t->child[1] = term_tail(t->inh * t->child[0]->syn);
		t->syn = t->child[1]->syn;
		if (t->child[0]->type == ExpFloatT || t->child[1]->type == ExpFloatT)
		{
			t->type = ExpFloatT;
		}
		else
		{
			t->type = ExpIntT;
		}
	}
	else if (token == OVER)
	{
		match(OVER);
		t->child[0] = factor();
		if (t->child[0]->syn == 0)
			syntaxError("div 0 error!!!");
		t->child[1] = term_tail(t->inh / t->child[0]->syn);
		t->syn = t->child[1]->syn;
		if (t->child[0]->type == ExpFloatT || t->child[1]->type == ExpFloatT)
		{
			t->type = ExpFloatT;
		}
		else
		{
			t->type = ExpIntT;
		}
	}
	else if (token == PLUS || token == MINUS || token == SEMI || token == DOT || token == RPAREN)
	{
		// epsilon do nothing
		t->syn = t->inh;
	}
	else
	{
		syntaxError("Syntax Error in collapsing term_tail!");
	}

	return t;
}

//factor		-> -number | number
static TreeNode* factor(void)
{
	TreeNode* t = newNode(FactorK);

	if (token == MINUS)
	{
		match(MINUS);
		t->child[0] = number();
		t->syn = 0 - t->child[0]->syn;
		t->type = t->child[0]->type;
	}
	else if (token == FLOATNUM || token == INTNUM || token == ID || token == LPAREN)
	{
		t->child[0] = number();
		t->syn = t->child[0]->syn;
		t->type = t->child[0]->type;
	}
	else
	{
		syntaxError("Syntax Error in collapsing factor!");
	}

	return t;
}

//number		-> (expr)|id | NUM
static TreeNode* number(void)
{
	TreeNode* t = newNode(NumberK);
	SymbolTableItem* p = NULL;

	if (token == LPAREN)
	{
		match(LPAREN);
		t->child[0] = expr();
		match(RPAREN);
		t->syn = t->child[0]->syn;
	}
	else if (token == ID)
	{
		//do sth.
		p = lookupSymbolTable(tokenString);
		if (p != NULL)
		{
			if (p->initial == 0)
			{
				syntaxError("Identifier Uninitilized!");
				printToken(token, tokenString);
			}
			t->syn = p->val;
			t->type = p->type;
		}
		else
		{
			syntaxError("Identifier Undefined!");
			printToken(token, tokenString);
		}
		match(ID);
	}
	else if (token == FLOATNUM)
	{
		t->type = ExpFloatT;
		t->syn = atof(tokenString);
		match(FLOATNUM);
	}
	else if (token == INTNUM)
	{
		t->type = ExpIntT;
		t->syn = atof(tokenString);
		match(INTNUM);
	}
	else
	{
		syntaxError("Syntax Error in collapsing number!");
	}

	return t;
}

TreeNode* parse(void)
{
	TreeNode* t;
	token = getToken();
	t = program();
	if (token != ENDFILE)
		syntaxError("Code ends before file");
	return t;
}

