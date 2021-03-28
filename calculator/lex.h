#pragma once

#define MAXTOKENLEN 40
/* BUFLEN = length of the input buffer for
   source code lines */
#define BUFLEN 256
#define MAXRESERVED 3

extern char tokenString[MAXTOKENLEN + 1];

TokenType getToken(void);