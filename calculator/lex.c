#include "globals.h"
#include "lex.h"
#include "util.h"


/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN + 1];


static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0; /* current position in LineBuf */
static int bufsize = 0; /* current size of buffer string */
static int EOF_flag = 0; /* corrects ungetNextChar behavior on EOF */

/* lookup table of reserved words */
static struct
{
	char* str;
	TokenType tk;
} reservedWords[MAXRESERVED]
= { {"int",INT},{"float",FLOAT},{"write",WRITE} };

/*get Next char*/
static int getNextChar(void)
{
	if (!(linepos < bufsize))
	{

		if (fgets(lineBuf, BUFLEN - 1, fpSrc))
		{
			lineno++;
			bufsize = strlen(lineBuf);
			linepos = 0;
			return lineBuf[linepos++];
		}
		else
		{
			EOF_flag = 1;
			return EOF;
		}
	}
	else return lineBuf[linepos++];
}

/*go back*/
static void ungetNextChar(void)
{
	if (!EOF_flag) linepos--;
}

/*look for reserved words*/
static TokenType reservedLookup(char* s)
{
	int i;
	for (i = 0; i < MAXRESERVED; i++)
		if (!strcmp(s, reservedWords[i].str))
			return reservedWords[i].tk;
	return ID;
}



TokenType getToken(void)
{
	int tkStrIndex = 0;
	TokenType currentToken;
	StateType state = START;

	bool save = true;
	/*state machine*/
	while (state != DONE)
	{
		int c = getNextChar();
		bool save = true;

		switch (state)
		{
		case START:
		{
			if ((c == ' ') || (c == '\t') || (c == '\n'))
				save = false;
			else if (isalpha(c))
				state = INID;
			else if (isdigit(c))
				state = INNUM;
			else
			{
				state = DONE;
				switch (c)
				{
				case EOF:
					save = false;
					currentToken = ENDFILE;
					break;
				case '=':
					currentToken = ASSIGN;
					break;
				case '+':
					currentToken = PLUS;
					break;
				case '-':
					currentToken = MINUS;
					break;
				case '*':
					currentToken = TIMES;
					break;
				case '/':
					currentToken = OVER;
					break;
				case '(':
					currentToken = LPAREN;
					break;
				case ')':
					currentToken = RPAREN;
					break;
				case ';':
					currentToken = SEMI;
					break;
				case '.':
					currentToken = DOT;
					break;
				default:
					currentToken = ERROR;
					break;
				}
			}
			break;
		}
		case INID:
		{
			if (!isalpha(c) && !isdigit(c))
			{
				ungetNextChar();
				save = false;
				state = DONE;
				currentToken = ID;
			}
			break;
		}
		case INNUM:
		{
			if (!isdigit(c))
			{
				if (c == '.')	//float
				{
					state = INNUM2;
				}
				else
				{
					save = false;
					state = DONE;
					currentToken = INTNUM;
					ungetNextChar();
				}
			}
			break;
		}
		case INNUM2:
		{
			if (isdigit(c))
			{
				state = INNUM3;
			}
			else
			{
				save = false;
				state = DONE;
				currentToken = INTNUM;
				if (c == EOF)
				{
					EOF_flag = 0;
					ungetNextChar();
				}
				else
				{
					ungetNextChar();
					ungetNextChar();
				}
				//twice to the .
				tkStrIndex--;	//remove . from tkString
			}
			break;
		}
		case INNUM3:
		{
			if (!isdigit(c))
			{
				ungetNextChar();
				save = false;
				state = DONE;
				currentToken = FLOATNUM;
			}
			break;
		}
		case DONE:
		default:
		{
			fprintf(stderr, "Scanner Bug: state= %d\n", state);
			state = DONE;
			currentToken = ERROR;
			break;
		}

		}

		if ((save) && (tkStrIndex <= MAXTOKENLEN))
			tokenString[tkStrIndex++] = (char)c;
		if (state == DONE)
		{
			tokenString[tkStrIndex] = '\0';
			if (currentToken == ID)
			{
				currentToken = reservedLookup(tokenString);
			}
		}
	}

	if (LexTrace)
	{
		fprintf(stdout, "\t%d: ", lineno);
		printToken(currentToken, tokenString);
	}


	return currentToken;
}