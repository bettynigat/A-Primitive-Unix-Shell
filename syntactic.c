

/*Bethelhem Nigat Nibret
   20170884
   file name= syntactic.c
   assignment #5     */               
/*--------------------------------------------------------------------*/                                                   
#include "dynarray.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "dfa.h"
#include "syntactic.h"


enum {MAX_LINE_LENGTH= 1024};
enum {FALSE, TRUE};


int command_check( DynArray_T oDynArray)
{
  assert(oDynArray != NULL);
/**command_check checks performs syntactic anaylsis. if successful it
teturns 0 either wise it returns -1. it takes an a pointer to a 
structure of type DynArray_T*/


  enum syntaxState {START, PIPE, INDIRECTION, OUTDIRECTION};
  enum syntaxState eState= START; 


  struct Token *previousToken;
  struct Token *psToken; 
  struct Token *nextToken; 
  int i; 
  int length; 
  enum TokenType tokenType; 
  //enum TokenType tokenTypeNext; 
  int count_indirection= 0; 
  int count_outdirection= 0; 
  int count_pipe=0; 

  assert(oDynArray!=NULL);

  length= DynArray_getLength(oDynArray); //length of dyn arrya 
  if (length==0)
  {
  	return 0; 
  }
  psToken= (struct Token*) (DynArray_get(oDynArray,0)); 
  tokenType= psToken->eType;
  if (tokenType!= TOKEN_WORD)
  {
  	fprintf(stderr, "./ish: Missing command name\n");
  	return -1; 
  }

  if (length > 1)
  {
  	for (i=1; i<length; i++)
  	{

  		psToken= (struct Token*) (DynArray_get(oDynArray,i)); 
  		tokenType= psToken->eType; 

  		switch(eState)
  		{
  		case START:
  		  if (tokenType==TOKEN_PIPE)
            eState = PIPE;
          else if(tokenType==TOKEN_INDIRECTION) 
            eState= INDIRECTION;
          else if(tokenType==TOKEN_OUTDIRECTION) 
            eState= OUTDIRECTION;  
          else i+=1;
          i-=1; 
          break; 

        case PIPE:
          count_pipe=1; 
          if (i==length-1)// pipe at the end
          { 
            fprintf(stderr, "./ish: Pipe or redirection destination not specified\n");
            return -1;
          }


          previousToken= (struct Token*) (DynArray_get(oDynArray,i-1));
          if (previousToken->eType !=TOKEN_WORD) //<|, ||
          {
            if (previousToken->eType == TOKEN_OUTDIRECTION)
            {
              fprintf(stderr, "./ish: Pipe or redirection destination not specified\n");
              return -1;
            }

            if (previousToken->eType== TOKEN_INDIRECTION)
            {
              fprintf(stderr, "./ish: Pipe or redirection destination not specified\n");
              return -1;
            }
            else // ||
            {
            fprintf(stderr, "./ish: Missing command name\n");
            return -1; 
            }
          }

          if (count_outdirection) // abc > fd |
          {
            fprintf(stderr,  "./ish: Multiple redirection of standard in/out\n");
            return -1;
          }

          nextToken= (struct Token*) (DynArray_get(oDynArray,i+1));

          if (nextToken->eType !=TOKEN_WORD) //|>, |<, ||
          {
            fprintf(stderr, "./ish: Missin command name\n"); 
            return -1; 
          }

          eState= START;
          break; 

          case INDIRECTION:

            if (i==length-1)// < at the end
            { 
            fprintf(stderr, "./ish: Pipe or redirection destination not specifiedfasdf}\n");
            return -1;
            }
            if (count_pipe)
            {
              fprintf(stderr, "./ish: Multiple redirection of standard in/out\n");
              return -1;
            }
            if (count_indirection)
            {
              fprintf(stderr, "./ish: Multiple redirection of standard in/out\n");
              return -1;
            }

            count_indirection=1; 

            nextToken= (struct Token*) (DynArray_get(oDynArray,i+1));

            if (nextToken->eType !=TOKEN_WORD) //|>, |<, || must be checke?????????
            {
              fprintf(stderr, "./ish: Pipe or redirection destination not specified\n"); 
              return -1; 
            } 

            previousToken= (struct Token*) (DynArray_get(oDynArray,i-1));
            if (previousToken->eType !=TOKEN_WORD) 
            {
              fprintf(stderr, "./ish: Missing command name\n"); 
              return -1; 
            } 
            eState= START;
            break; 

        case OUTDIRECTION:
          if (i==length-1)// < at the end
          { 
            fprintf(stderr, "./ish: Pipe or redirection destination not specified\n");
            return -1;
          }
          if (count_outdirection)
          {
            fprintf(stderr, "./ish: Multiple redirection of standard in/out\n");
            return -1;
          }
          count_outdirection=1; 
          eState= START;
          break; 
            
      }
    }
  }
     return 0;
}
/*
int main(void){
  char acLine[1024];
   DynArray_T oTokens;
   int iSuccessful;
  // int check; 

   while (fgets(acLine, 1024, stdin) != NULL)
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
        if (command_check(oTokens)==0)
        {
          printf("Words:  ");
          DynArray_map(oTokens, printWordToken, NULL);
          printf("\n");
        }

      }

      DynArray_map(oTokens, freeToken, NULL);
      DynArray_free(oTokens);
   }

   return 0;
}
*/