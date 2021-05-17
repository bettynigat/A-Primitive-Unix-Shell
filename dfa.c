/*--------------------------------------------------------------------*/
/* dfa.c                                                              */
/* Original Author: Bob Dondero                                       */
/* Illustrate lexical analysis using a deterministic finite state     */
/* automaton (DFA)                                                    */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*Bethelhem Nigat Nibret
20170884
file name= dfa.c
assignment#5                    */
/*--------------------------------------------------------------------*/
#include "dynarray.h"
#include "dfa.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*--------------------------------------------------------------------*/

enum {MAX_LINE_SIZE = 1024};

enum {FALSE, TRUE};


/*--------------------------------------------------------------------*/

/* A Token is either a number or a word, expressed as a string. */

/*--------------------------------------------------------------------*/

void freeToken(void *pvItem, void *pvExtra)

/* Free token pvItem.  pvExtra is unused. */

{
   struct Token *psToken = (struct Token*)pvItem;
   free(psToken->pcValue);
   free(psToken);
}

/*--------------------------------------------------------------------*/


/*--------------------------------------------------------------------*/

void printWordToken(void *pvItem, void *pvExtra)

/* Print token pvItem to stdout iff it is a word.  pvExtra is
   unused. */

{
   struct Token *psToken = (struct Token*)pvItem;
   if ((psToken->eType == TOKEN_WORD) || (psToken->eType == TOKEN_INDIRECTION) ||(psToken->eType == TOKEN_OUTDIRECTION)
      || (psToken->eType == TOKEN_PIPE))
      printf("%s ", psToken->pcValue);
}

/*--------------------------------------------------------------------*/

static struct Token *makeToken(enum TokenType eTokenType,
   char *pcValue)

/* Create and return a Token structure whose type is eTokenType and whose
   value consists of string pcValue.  Return NULL if insufficient
   memory is available.  The caller owns the Token. */

{
   struct Token *psToken;

   psToken = (struct Token*)malloc(sizeof(struct Token));
   if (psToken == NULL)
      return NULL;

   psToken->eType = eTokenType;

   psToken->pcValue = (char*)malloc(strlen(pcValue) + 1);
   if (psToken->pcValue == NULL)
   {
      free(psToken);
      return NULL;
   }

   strcpy(psToken->pcValue, pcValue);

   return psToken;
}

/*--------------------------------------------------------------------*/

 int lexLine(const char *pcLine, DynArray_T oTokens)

/* Lexically analyze string pcLine.  Populate oTokens with the
   tokens that pcLine contains.  Return 1 (TRUE) if successful, or
   0 (FALSE) otherwise.  In the latter case, oTokens may contain
   tokens that were discovered before the error. The caller owns the
   tokens placed in oTokens. */

/* lexLine() uses a DFA approach.  It "reads" its characters from
   pcLine. */

{
   enum LexState {START, IN_WORD, IN_QUOTE_START,IN_QUOTE_END};

   enum LexState eState = START;

   int iLineIndex = 0;
   int iValueIndex = 0;
   char c;
   char acValue[MAX_LINE_SIZE];
   struct Token *psToken;

   assert(pcLine != NULL);
   assert(oTokens != NULL);

   for (;;)
   {
      /* "Read" the next character from pcLine. */
      c = pcLine[iLineIndex++]; //first character is read

      switch (eState)
      {
         case START:
            if ((c == '\n') || (c == '\0'))
               return TRUE;

            else if ((c == '"'))
               eState= IN_QUOTE_START; 

            else if ((c == '|') || (c == '<') || (c == '>'))
            {
               acValue[iValueIndex++] = c;
               acValue[iValueIndex] = '\0';

               if ((c == '|'))
                  {
                  psToken = makeToken(TOKEN_PIPE, acValue); //creates a structure 
                  iValueIndex = 0;
                  eState = START;
                  if (psToken == NULL)
                  {
                     fprintf(stderr, "Cannot allocate memory\n");
                     return FALSE;
                  }
                  if (! DynArray_add(oTokens, psToken))  
                  {
                     fprintf(stderr, "Cannot allocate memory\n");
                     return FALSE;
                  }
               }

               else if ((c == '<'))
               {
                  psToken = makeToken(TOKEN_INDIRECTION, acValue);
                  iValueIndex = 0;
                  eState = START;
                  if (psToken == NULL)
                  {
                     fprintf(stderr, "Cannot allocate memory\n");
                     return FALSE;
                  }
                  if (! DynArray_add(oTokens, psToken))
                  {
                     fprintf(stderr, "Cannot allocate memory\n");
                     return FALSE;
                  }
               }

               else if ((c == '>'))
               {
                  psToken = makeToken(TOKEN_OUTDIRECTION, acValue);
                  iValueIndex = 0;
                  eState = START;
                  if (psToken == NULL)
                  {
                     fprintf(stderr, "Cannot allocate memory\n");
                     return FALSE;
                  }
                  if (! DynArray_add(oTokens, psToken))
                  {
                     fprintf(stderr, "Cannot allocate memory\n");
                     return FALSE;
                  }
               }
               
            }

            else if (isspace(c))
               eState = START;

            else //it is a word 
            {
               acValue[iValueIndex++] = c; //first put it in the array 
               eState = IN_WORD;
            }
         
            break;

      
         case IN_WORD:
            if ((c == '\n') || (c == '\0'))
            {
               /* Create a WORD token. */
               acValue[iValueIndex] = '\0'; //terminate 
               psToken = makeToken(TOKEN_WORD, acValue);
               iValueIndex = 0;

               if (psToken == NULL)
                  
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               return TRUE;
            }
            
            else if (isspace(c))
            {
               /* Create a WORD token. */
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);

               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               iValueIndex = 0;
               eState = START;
            }

            else if ((c == '"'))
               eState= IN_QUOTE_START;


            else if ((c == '|') || (c == '<')|| (c == '>'))
            {
               acValue[iValueIndex] = '\0'; //terminate 
               psToken = makeToken(TOKEN_WORD, acValue);

               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               iValueIndex = 0; //save the word first 

                     //save the special character 
               acValue[iValueIndex++] = c;
               acValue[iValueIndex] = '\0';

               if ((c == '|'))
               {
                  psToken = makeToken(TOKEN_PIPE, acValue);
                  if (psToken == NULL)
                  {
                     fprintf(stderr, "Cannot allocate memory\n");
                     return FALSE;
                  }
                  if (! DynArray_add(oTokens, psToken))
                  {
                     fprintf(stderr, "Cannot allocate memory\n");
                     return FALSE;
                  }
               }

               else if ((c == '<'))
               {
                  psToken = makeToken(TOKEN_INDIRECTION, acValue);
                  if (psToken == NULL)
                  {
                     fprintf(stderr, "Cannot allocate memory\n");
                     return FALSE;
                  }
                  if (! DynArray_add(oTokens, psToken))
                  {
                     fprintf(stderr, "Cannot allocate memory\n");
                     return FALSE;
                  }
               }

               else if ((c == '>'))
               {
                  psToken = makeToken(TOKEN_OUTDIRECTION, acValue);
                  if (psToken == NULL)
                  {
                     fprintf(stderr, "Cannot allocate memory\n");
                     return FALSE;
                  }
                  if (! DynArray_add(oTokens, psToken))
                  {
                     fprintf(stderr, "Cannot allocate memory\n");
                     return FALSE;
                  }
               }
               iValueIndex = 0;
               eState = START;  
            }

            else //it is a word 
            {
               acValue[iValueIndex++] = c;
               eState = IN_WORD;
            }
            break;

         case IN_QUOTE_START: //from word
            if ((c == '\n') || (c == '\0')) //terminated while quote is opened 
            {
               fprintf(stderr, "./ish: Could not find quote pair\n");
               return FALSE;
            } 

            else if ((c == '"'))
               eState= IN_QUOTE_END;

            else //stay in loop 
            {
               acValue[iValueIndex++] = c; //first put it in the array 
               eState= IN_QUOTE_START;
            }

            break; 

         case IN_QUOTE_END:
            if ((c == '\n') || (c == '\0'))
            {
               /* Create a WORD token. */
               acValue[iValueIndex] = '\0'; //terminate 
               psToken = makeToken(TOKEN_WORD, acValue);

               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               iValueIndex = 0;
               return TRUE;
            }

            else if (isspace(c))
            {
               /* Create a WORD token. */
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);

               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }

               iValueIndex = 0;
               eState = START;
            }


            else if ((c == '"'))
               eState= IN_QUOTE_START;

            else if ((c == '|') || (c == '<')|| (c == '>'))
            {
               acValue[iValueIndex] = '\0'; //terminate 
               psToken = makeToken(TOKEN_WORD, acValue);

               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }

               iValueIndex = 0; //save the word first 

                     //save the special character 
               acValue[iValueIndex++] = c;
               acValue[iValueIndex] = '\0';

               
               if ((c == '|'))
               {
                  psToken = makeToken(TOKEN_PIPE, acValue);
                  if (psToken == NULL)
                  {
                     fprintf(stderr, "Cannot allocate memory\n");
                     return FALSE;
                  }
                  if (! DynArray_add(oTokens, psToken))
                  {
                     fprintf(stderr, "Cannot allocate memory\n");
                     return FALSE;
                  }
               }

               else if ((c == '<'))
               {
                  psToken = makeToken(TOKEN_INDIRECTION, acValue);
                  if (psToken == NULL)
                  {
                     fprintf(stderr, "Cannot allocate memory\n");
                     return FALSE;
                  }
                  if (! DynArray_add(oTokens, psToken))
                  {
                     fprintf(stderr, "Cannot allocate memory\n");
                     return FALSE;
                  }
               }

               else if ((c == '>'))
               {
                  psToken = makeToken(TOKEN_OUTDIRECTION, acValue);
                  if (psToken == NULL)
                  {
                     fprintf(stderr, "Cannot allocate memory\n");
                     return FALSE;
                  }
                  if (! DynArray_add(oTokens, psToken))
                  {
                     fprintf(stderr, "Cannot allocate memory\n");
                     return FALSE;
                  }
               }

               iValueIndex = 0;
               eState = START;  
            }

            else //it is a word 
            {
               acValue[iValueIndex++] = c;
               eState = IN_WORD;
            }

            break;
            
         default:
            assert(FALSE);
      }
   }
   return 1; 
}

/*--------------------------------------------------------------------*/

/*int main(void)*/

/* Read a line from stdin, and write to stdout each number and word
   that it contains.  Repeat until EOF.  Return 0 iff successful. */

/*{
   char acLine[MAX_LINE_SIZE];
   DynArray_T oTokens;
   int iSuccessful;

   printf("------------------------------------\n");
   while (fgets(acLine, MAX_LINE_SIZE, stdin) != NULL)
   {
      oTokens = DynArray_new(0);
      if (oTokens == NULL)
      {
         fprintf(stderr, "Cannot allocate memory\n");
         exit(EXIT_FAILURE);
      }

      iSuccessful = lexLine(acLine, oTokens);
      if (iSuccessful)
      {
          
          printf("Words:  ");
          DynArray_map(oTokens, printWordToken, NULL);
          printf("\n");
      }
      printf("------------------------------------\n");

      DynArray_map(oTokens, freeToken, NULL);
      DynArray_free(oTokens);
   }

   return 0;
}*/
