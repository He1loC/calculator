#include "globals.h"
#include "lex.h"
#include "parse.h"
#include "util.h"

/* allocate global variables */
int lineno = 0;
FILE* fpSrc;
FILE* fpOut;
const int LexTrace = 0;
SymbolTable pSymbolTable=NULL;

int main(int argc, char* argv[])
{
	errno_t err;
	TreeNode* t;

	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s <inputFileName> <outputFileName>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	err = fopen_s(&fpSrc, argv[1], "r");
	if (err || fpSrc == NULL)
	{
		fprintf(stderr, "Can't open %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	err = fopen_s(&fpOut, argv[2], "w");
	if (err || fpOut == NULL)
	{
		fprintf(stderr, "Can't open %s\n", argv[2]);
		exit(EXIT_FAILURE);
	}
	
	if (LexTrace)
	{
		TokenType test;
		test = getToken();
		while (test != ENDFILE)
			test = getToken();
	}

	pSymbolTable = initSymbolTable();
	t = parse();


	fclose(fpSrc);
	fclose(fpOut);

	
	system(argv[2]);

	return 0;
}

