#pragma once

void printToken(TokenType token, const char* tokenString);

SymbolTable initSymbolTable(void);
void insertSymbolTable(char* name, ExpType type);
SymbolTableItem* lookupSymbolTable(char* name);