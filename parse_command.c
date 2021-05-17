/*Bethelhem Nigat Nibret
   20170884
   file name= parse_command.c
   assignment #5 */                   
/*--------------------------------------------------------------------*/ 

#include "dynarray.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "dfa.h"
#include "syntactic.h"
#include "parse_command.h"


void freeMainCommand(MainCommand_T MainCommand_ptr)
{
	/* freeMainCommand frees a structure of type MainCommand and its 
	and its associated memory.*/
	MainCommand_T MCptr=MainCommand_ptr;

	for(int i=0; i<MCptr->num_commands; i++)
	{
		for(int j=0; j<(MCptr->CommandArray[i]->count_argument)+1;j++)
		{
			if (MCptr->CommandArray[i]->cmd[j])
				free(MCptr->CommandArray[i]->cmd[j]);
		}
		free(MCptr->CommandArray[i]->cmd);
		free(MCptr->CommandArray[i]);
	}


	if(MCptr->CommandArray)
		free(MCptr->CommandArray); 
	if(MCptr->input_file)
		free(MCptr->input_file);
	if(MCptr->output_file)
		free(MCptr->output_file);
	free(MCptr);

}


void DynArray_to_Array(DynArray_T oDynArray, char **ppvArray, 
	int initial_index, int final_index)
/*it copies contents from the dynamic array oDynArray to a new fixed
 size array ppvArray.it starts copying from the initial_index element 
 of oDynArray until the final_index of oDynArray. Then it sets the
 last element of ppvArray to NULL*/
{		
	assert(oDynArray != NULL);
	int initial= initial_index; 
	int final= final_index;
    struct Token *psToken;

	int k = 0;

	for (int i = initial; i <=final; k++, i++)
    {
        psToken = (struct Token*)(DynArray_get(oDynArray, i));
        ppvArray[k] = (char *)malloc(strlen(psToken->pcValue)+1);
        if (ppvArray[k]==NULL)
        {
        	fprintf(stderr, "./ish: can't allocate memory space");
        	return;
        }
        strcpy(ppvArray[k],psToken->pcValue);
    }
    ppvArray[k] = NULL; 
}


Command_T makeCommand (void)
{
	/*it makes a sturcture Command and returns a pointer to it*/
	Command_T command; 
	command= (Command_T)calloc(1, sizeof(struct Command));
	if (command== NULL)
    {
        fprintf(stderr, "./ish: can't allocate memory space");
        return NULL;
    }

    command->cmd=NULL; //initially no array of command arguments
    command->count_argument=0; 
    return command; //returns array of commands 
} 

MainCommand_T makeMainCommand(Command_T command) 
{

	
	
	/*it makes a sturcture MainCommand and returns a pointer to it
	the created structure will have a pointer to the stucture command*/
	
	MainCommand_T MainCommand; 
	MainCommand= (MainCommand_T)calloc(1, sizeof(struct MainCommand));
	if (MainCommand == NULL)
    {
        fprintf(stderr, "./ish: can't allocate memory space");
        return NULL;
    }

	MainCommand->CommandArray= (Command_T*)(calloc(1024, sizeof(Command_T))); 
	 if (MainCommand->CommandArray == NULL)
    {
        fprintf(stderr, "./ish: can't allocate memory space");
        free(MainCommand); 
        return NULL;
    }

    MainCommand->input_file=NULL;
    MainCommand->output_file=NULL;
    MainCommand->num_commands=0;
    MainCommand->count_pipe=0;
    return MainCommand; 
    
}

MainCommand_T parse_command(DynArray_T oDynArray)
{
	/*it takes a dynarray and parses it into a set of commands to be 
	executed. then it copies the parsed arguments into a MainCommnad
	sturcture.it returns a pointer to this*/

	assert(oDynArray != NULL);
	int length; 
	int count_commandArray=0;
	 
	int initial_index;
	long int final_index;
	int count_command=0; 

	
  	struct Token *psToken; 
  	struct Token *nextToken;
  	
  	char *input_file; 
  	char *output_file; 
	length=DynArray_getLength(oDynArray);
	if (length == 0){
        return NULL;
	}

    enum State {COMMAND, INDIRECTION,
                       OUTDIRECTION, PIPE};
    enum State eState = COMMAND;   

    Command_T command=NULL; 
    MainCommand_T MainCommand_ptr;
	MainCommand_ptr= makeMainCommand(command); 

    if (length)
    {
    		for (long int i=0; i<length;i++ )
    		{

    			psToken= (struct Token*) (DynArray_get(oDynArray,i)); 
  				switch(eState)
  				{
					case(COMMAND):
						count_command++;
						if (i<length-1) //it is not the last command 
						{
							nextToken= (struct Token*) (DynArray_get(oDynArray,i+1));
							//printf("next token value %s\n", nextToken->pcValue); 
							if (nextToken->eType ==TOKEN_PIPE)
								eState=PIPE;
							else if (nextToken->eType ==TOKEN_INDIRECTION)
								eState=INDIRECTION;
							else if (nextToken->eType ==TOKEN_OUTDIRECTION)
								eState=OUTDIRECTION;
							else eState=COMMAND; //else it is a command 
						}
						else //if it is the last command
						{
							initial_index= i-count_command+1;
							final_index=i; 
							Command_T Command_ptr; 
							Command_ptr= makeCommand();

							Command_ptr->count_argument= count_command; 
							count_commandArray++; 
							Command_ptr->cmd= (char**)calloc(count_command+1, sizeof(char*)); 
							if (Command_ptr->cmd==NULL)
					        {
					        	fprintf(stderr, "./ish: can't allocate memory space");
					        	return NULL;
					        } 
							
							DynArray_to_Array(oDynArray, Command_ptr->cmd,initial_index, final_index); 
							MainCommand_ptr->CommandArray[count_commandArray-1]=Command_ptr;
						}
						break; 

  					case(INDIRECTION):
  							//means the next token is input_file; 
  							nextToken= (struct Token*) (DynArray_get(oDynArray,i+1));
  							input_file=(char *)malloc(strlen(nextToken->pcValue)+1);
  							if (input_file==NULL)
					        {
					        	fprintf(stderr, "./ish: can't allocate memory space");
					        	return NULL;
					        } 
  							strcpy(input_file,nextToken->pcValue); //save input file 
  							//delete both <input_file; 
  							DynArray_removeAt(oDynArray, i);// remove < 
  							DynArray_removeAt(oDynArray, i);// remove input_file 
  							length=length-2; 
  							MainCommand_ptr->input_file= input_file; 
  							//now i points to the next token 

						if (i<length) //it is not the last command 
						{
							
							psToken= (struct Token*) (DynArray_get(oDynArray,i));
							
							if (psToken->eType ==TOKEN_PIPE){
								
								eState=PIPE;
							}
							else if (psToken->eType ==TOKEN_INDIRECTION){
								
								eState=INDIRECTION;
							}
							else if (psToken->eType ==TOKEN_OUTDIRECTION){
								
								eState=OUTDIRECTION;
							}
							else if (psToken->eType==TOKEN_WORD) {
								
								eState=COMMAND; 
							}
						}
						else //if it is the last command
						{
							
							initial_index= i-count_command;
							
							final_index=i-1;
							
							
							Command_T Command_ptr; 
							Command_ptr= makeCommand();
							Command_ptr->count_argument= final_index-initial_index +1;
							count_commandArray++; 
							Command_ptr->cmd= (char**)calloc(count_command+1, sizeof(char*));
							if (Command_ptr->cmd==NULL)
					        {
					        	fprintf(stderr, "./ish: can't allocate memory space");
					        	return NULL;
					        } 

							
							DynArray_to_Array(oDynArray, Command_ptr->cmd,initial_index, final_index);
							
							MainCommand_ptr->CommandArray[count_commandArray-1]=Command_ptr; 
							

						}

						i--; //so that we will be in i when we come back
						break; 
					case(OUTDIRECTION):
							 //means the next token is output_file; 
  							nextToken= (struct Token*) (DynArray_get(oDynArray,i+1));
  							output_file=(char *)malloc(strlen(nextToken->pcValue)+1);
  							if (input_file==NULL)
					        {
					        	fprintf(stderr, "./ish: can't allocate memory space");
					        	return NULL;
					        }  
  							strcpy(output_file,nextToken->pcValue); //save ouput file 
  							//delete both >output_file; 
  							DynArray_removeAt(oDynArray, i);// remove > 
  							DynArray_removeAt(oDynArray, i);// remove output_file 
  							length=length-2; 
  							MainCommand_ptr->output_file= output_file; 
  							//now i points to the next token 
  							

						if (i<length) //it is not the last command 
						{
							psToken= (struct Token*) (DynArray_get(oDynArray,i));
							if (psToken->eType ==TOKEN_PIPE)
								eState=PIPE;
							else if (psToken->eType ==TOKEN_INDIRECTION)
								eState=INDIRECTION;
							else if (psToken->eType ==TOKEN_OUTDIRECTION)
								eState=OUTDIRECTION;
							else if (psToken->eType==TOKEN_WORD) 
								eState=COMMAND; 
						}
						else //if it is the last command
						{
							initial_index= i-count_command;
							final_index=i-1; 
							Command_T Command_ptr; 
							Command_ptr= makeCommand();
							Command_ptr->count_argument= final_index-initial_index +1;
							count_commandArray++; 
							Command_ptr->cmd= (char**)calloc(count_command+1, sizeof(char*));
							if (Command_ptr->cmd== NULL)
							{
						        fprintf(stderr, "./ish: can't allocate memory space");
						        return NULL;
    						}
							 
							DynArray_to_Array(oDynArray, Command_ptr->cmd,initial_index, final_index);
							
							MainCommand_ptr->CommandArray[count_commandArray-1]=Command_ptr; 
							

						}

						i--; //so that we will be in i when we come back
						break; 
					case(PIPE):
							MainCommand_ptr->count_pipe++; 
							
							initial_index= i-count_command;
							final_index= i-1; 
							
							Command_T Command_ptr; 
							Command_ptr= makeCommand();
							Command_ptr->count_argument= final_index-initial_index +1;
							count_commandArray++; 
							Command_ptr->cmd= (char**)calloc(count_command+1, sizeof(char*)); 
							if (Command_ptr->cmd== NULL)
							{
						        fprintf(stderr, "./ish: can't allocate memory space");
						        return NULL;
    						}
							
							DynArray_to_Array(oDynArray, Command_ptr->cmd,initial_index, final_index); 
							
							MainCommand_ptr->CommandArray[count_commandArray-1]=Command_ptr; 


							//start counting another command set
							count_command=0; 
							//always after pipeline is word
							eState=COMMAND; 
							break; 

  				}

    		} 
 	}
 	
 	MainCommand_ptr->num_commands = count_commandArray; 
 	return MainCommand_ptr;

 }

/*int main(void){
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
          MainCommand_T MainCommand_ptr= parse_command(oTokens); 
          if (MainCommand_ptr==NULL) return 1; 
          //Command_T* Command_ptr; 

         
          printf("inputfile: %s\n", MainCommand_ptr->input_file);
          printf("outputfile: %s\n", MainCommand_ptr->output_file);
          printf("pipe no %d\n", MainCommand_ptr->count_pipe);

          	
        for (int i=0; i<MainCommand_ptr->num_commands; i++)
          	{
          		printf("parased: \n");
          		fflush(NULL);
          		Command_T Command_ptr= MainCommand_ptr->CommandArray[i]; 
          		///printf("ere: %d\n", Command_ptr!=NULL);
          		fflush(NULL);
          		for (int index=0; Command_ptr->cmd[index]!=NULL; index++)
          		{
          			//printf("some\n");
          			fflush(NULL);
          			printf("%s ", Command_ptr->cmd[index]);
          			fflush(NULL);
          		}
          		printf("\n");
          		printf("argument no: %d\n", Command_ptr->count_argument); 
          	}
          }
      }
	      DynArray_map(oTokens, freeToken, NULL);
	      DynArray_free(oTokens); 
     }
     printf("\n");

   return 0;
}*/









