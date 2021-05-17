
/*-------------------------------------------------------------------*/
/*Bethelhem Nigat Nibret
   20170884
   file name= parse_command.h
   assignment #5        */            
/*--------------------------------------------------------------------*/ 

#ifndef PARSE_INCLUDED
#define PARSE_INCLUDED




struct Command
{

  /*This structure has count_argument that contains the number 
  of words in a single command excluding input or output 
  file names. its cmd points to an array of character pointers 
  which each save a word in a single command*/

	char** cmd;
	int count_argument;  
}; 


typedef struct Command * Command_T;

struct MainCommand 
{ 
  /*This stucture saves the parsed array ready for execution. it saves
   input or output redirection files. It also saves the total number of 
   pipes and the number of the set of arguments. CommandArray saves a 
   pointer to a Command stucture*/

   Command_T* CommandArray;
   char* input_file;
   char* output_file; 
   int num_commands;
   int count_pipe; 
};

typedef struct MainCommand * MainCommand_T;



void DynArray_to_Array(DynArray_T oDynArray, char **ppvArray, 
  int initial_index, int final_index);
  /*it copies contents from the dynamic array oDynArray to a new fixed
 size array ppvArray.it starts copying from the initial_index element 
 of oDynArray until the final_index of oDynArray. Then it sets the
 last element of ppvArray to NULL*/

Command_T makeCommand (void);
/*it makes a sturcture Command and returns a pointer to it*/

MainCommand_T makeMainCommand(Command_T command);
/*it makes a sturcture MainCommand and returns a pointer to it
  the created structure will have a pointer to the stucture command*/

MainCommand_T parse_command(DynArray_T oDynArray);
/*it takes a dynarray and parses it into a set of commands to be 
  executed. then it copies the parsed arguments into a MainCommnad
  sturcture.it returns a pointer to this*/

void freeMainCommand(MainCommand_T MainCommand_ptr);
/* freeMainCommand frees a structure of type MainCommand and its 
  and its associated memory.*/

int command_check( DynArray_T oDynArray); 
/**command_check checks performs syntactic anaylsis. if successful it
teturns 0 either wise it returns -1. it takes an a pointer to a 
structure of type DynArray_T*/

#endif
