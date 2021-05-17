       
/*--------------------------------------------------------------------*/
/*Bethelhem Nigat Nibret
   20170884
   file name= dfa.h
   assignment #5  */                  
/*--------------------------------------------------------------------*/                                                

#ifndef DFA_INCLUDED
#define DFA_INCLUDED

/* A DynArray_T is an array whose length can expand dynamically. */
//typedef struct DynArray * DynArray_T;

enum TokenType {TOKEN_WORD, TOKEN_PIPE, TOKEN_INDIRECTION, TOKEN_OUTDIRECTION};

struct Token
{
   enum TokenType eType;
   /* The type of the token. */

   char *pcValue;
   /* The string which is the token's value. */
};
/* Free token pvItem.  pvExtra is unused. */
void freeToken(void *pvItem, void *pvExtra);

/* get the type of token pvItem.*/
int getTokenType(void *pvItem);

/*get the string value that pvItem->pcvalue is pointing to */
char * getTokenValue(void *pvItem);

/* Print token pvItem to stdout iff it is a word.  pvExtra is
   unused. */
void printWordToken(void *pvItem, void *pvExtra);

/* Lexically analyze string pcLine.  Populate oTokens with the
   tokens that pcLine contains.  Return 1 (TRUE) if successful, or
   0 (FALSE) otherwise.  In the latter case, oTokens may contain
   tokens that were discovered before the error. The caller owns the
   tokens placed in oTokens. */
int lexLine(const char *pcLine, DynArray_T oTokens);

#endif
