
/*
Bethelhem Nigat Nibret
file name ish.c
Assignment no=5 
student number 20170884 
*/
#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif

#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>

#include "dynarray.h"
#include "dfa.h"
#include "syntactic.h"
#include "parse_command.h"
enum {FALSE, TRUE};

void QUIT_HANDLER2 (int sig)
{
	/**executes exit system call*/
	printf("\n");
  	fflush(stdout);
  	exit(0);
	 
}

void QUIT_HANDLER (int sig)
{
	/*signal handler for Ctrl-\. if a user types Ctrl-\ again 
	withing 5 seconds QUIT_HANDLER2 is executed. */
    fflush(stdout);
    fprintf(stdout, "\nType Ctrl-\\ again within 5 seconds to exit\n");
    printf("%% ");
    fflush(NULL);
    alarm(5);

    if(signal(SIGQUIT, QUIT_HANDLER2)==SIG_ERR)
    {
        perror("./ish: SIGQUIThandler: ");
    }  
}

void ALRM_HANDLER (int sig)
{
	signal(SIGQUIT, QUIT_HANDLER); 
	
}
void executeBuiltin(MainCommand_T MCptr)
{
	/*executes the 4 builtin commnads. if unsuccessful it prints
	error messages. it takes a pointer to a struct MainCommand*/
	char* home;  
	Command_T Cptr= MCptr->CommandArray[0]; 

	if ((strcmp(Cptr->cmd[0],"cd"))==0)
	{
		if (MCptr->count_pipe) //morethan 1 command sets 
		{
			fprintf(stderr, "./ish: cd takes one parameter\n");
			return; 
		}
		//now we have one argument
		Command_T Cptr= MCptr->CommandArray[0];
		//case cd
		
		if ((Cptr->count_argument)>2)
		{
			fprintf(stderr, "./ish: cd takes one parameter\n");
			return; 
		}

		if ((Cptr->count_argument)==2)
		{
			if (chdir(Cptr->cmd[1])==-1)
			{
				perror("./ish: cd "); 
				return; 
			}
		}

		if ((Cptr->count_argument)==1)
		{
			home = getenv("HOME");
            if(home == NULL)
            {
            	//printf("home\n");
                fprintf(stderr,"./ish: cd HOME directory not set\n");
            }
            else
            {
                if(chdir(home) == -1)
                {
                	//printf("%s\n", home);
                    perror("./ish: cd ");
                }
            }
		}
	}

	else if ((strcmp(Cptr->cmd[0],"setenv"))==0)
	{
		if (MCptr->count_pipe) //morethan 1 command sets 
		{
			fprintf(stderr, "./ish: setenv takes one or two parameters\n");
			return; 
		}
		//now we have one argument
		//case cd
		
		if ((Cptr->count_argument)>3)
		{
			fprintf(stderr, "./ish: setenv takes one or two parameters\n");
			return; 
	
		}

		if ((Cptr->count_argument)==3)
		{
			//fprintf(stdout, "%s\n", Cptr->cmd[1] );
			if (setenv(Cptr->cmd[1],Cptr->cmd[2], TRUE)==-1)
			{
				perror("./ish: setenv "); 
				return; 
			}

		}

		if ((Cptr->count_argument)==2)
		{
			//fprintf(stdout, "%s here\n", Cptr->cmd[1] );
			if (setenv(Cptr->cmd[1],"\0", TRUE)==-1)
			{

				perror("./ish: setenv "); 
				return; 
			}
		}

		if ((Cptr->count_argument)==1)
		{
			fprintf(stderr, "./ish: setenv takes one or two parameters\n");
			return;  
		}

	}

	else if ((strcmp(Cptr->cmd[0],"unsetenv"))==0)
	{
		if (MCptr->count_pipe) //morethan 1 command sets 
		{
			fprintf(stderr, "./ish: setenv takes one or two parameters\n");
			return; 

		}
		//now we have one argument
		//case cd
	
		if ((Cptr->count_argument)>2)
		{
			fprintf(stderr, "./ish: unsetenv takes one parameter\n");
			return; 
		}

		if ((Cptr->count_argument)==2)
		{
			if (unsetenv(Cptr->cmd[1])==-1)
			{
				perror("./ish: unsetenv ");
				return;  
			}
		}
		if ((Cptr->count_argument)==1)
		{
			fprintf(stderr, "./ish: unsetenv takes one parameter\n" ); 
			return; 
		}
	}

	else if ((strcmp(Cptr->cmd[0],"exit"))==0)
	{
		if (MCptr->count_pipe) //morethan 1 command sets 
		{
			fprintf(stderr, "./ish: exit does not take any parameters\n");
			return; 
		}
		//now we have one argument
		//case cd
		
		if ((Cptr->count_argument)>1)
		{
			fprintf(stderr, "./ish: exit does not take any parameters\n");
			return; 
		}

		exit(0);
	}
	return; 
}

int executeNoPipe(MainCommand_T MCptr)
{

	/*executes commands containing with no pipe. if unsuccessful 
	it prints error messages and returns -1. it takes a pointer
	to a struct MainCommand*/
	char* output_file=MCptr->output_file;
	char* input_file= MCptr->input_file;
	
		pid_t iPid; 
		
			Command_T Cptr= MCptr->CommandArray[0];
			fflush(NULL); 
			iPid=fork();
			
			if (iPid==-1)
			{
				perror("./ish: ");
		        return -1;
			}
			if (iPid==0)
			{	

				if ((signal(SIGINT, SIG_DFL))== SIG_ERR)
            	{
	                perror("./ish: ");
	                return -1;
	            }

	            if ((signal(SIGQUIT, SIG_DFL))== SIG_ERR)
            	{
	                perror("./ish: ");
	                return -1;
	            }

	            if ((signal(SIGALRM, SIG_DFL))== SIG_ERR)
            	{
	                perror("./ish: ");
	                return -1;
	            }

				if (output_file)
				{
					int iFd;
					int iRet; 
					if((iFd = open(output_file,O_WRONLY | O_CREAT | O_TRUNC,0600)) < 0)
					{					
						return -1; 
					}
					
					if ((iRet= dup2(iFd, 1))==-1)
					{
						perror("./ish: ");
						return -1; 
					}
					if ((iRet=close(iFd))==-1)
					{
						perror("./ish: ");
						return -1;
					}
				}

				if (input_file)
				{
					int iFd;
					int iRet; 
					if((iFd = open(input_file,O_RDWR,0600)) < 0)
					{	
						perror("./ish: ");
						return -1;
					}

					
					if ((iRet= dup2(iFd, 0))==-1)
					{
						perror("./ish: ");
						return -1; 
					}
					if ((iRet=close(iFd))==-1)
					{
						perror("./ish: ");
						return -1;
					}
				}

				printf("%s" , Cptr->cmd[0]);
				execvp(Cptr->cmd[0],Cptr->cmd);


	            perror(Cptr->cmd[0]);  
	        	_exit(0);
			}
		wait(NULL); 
		return 0; 
}
		
		
int executePipe(MainCommand_T MCptr)
{
	/*executes commands containing with pipe. if unsuccessful 
	it prints error messages and returns -1. it takes a pointer
	to a struct MainCommand*/
	char* output_file=MCptr->output_file;
	char* input_file= MCptr->input_file;
	int Fdin;
	int Fdout;
	int command_no= MCptr->num_commands; 
	pid_t iPid;
	int stdin=dup(0);
	int stdout=dup(1);

	
	
	for (int i=0; i<command_no; i++)
	{
		Command_T Cptr= MCptr->CommandArray[i];
		//if the first command 
		if (i==0)
		{
			if (input_file)
			{
				if((Fdin = open(input_file,O_RDWR,0600)) < 0)
				{					
					perror("./ish: ");
					return -1; 
				}
			}
			else
			{
				if ((Fdin= dup(stdin))==-1)
				{
					perror("./ish: ");
					return -1; 
				}
			} 
		}

		if (dup2(Fdin,0)==-1)
		{
			perror("./ish: ");
			return -1; 
		}
		if (close(Fdin)==-1)
			{
				perror("./ish: ");
				return -1; 
			}

		//if last command 
		if (i==command_no-1)
		{
			if(output_file)
			{
				if((Fdout = open(output_file,O_WRONLY | O_CREAT | O_TRUNC,0600)) < 0)
					{					
						perror("./ish:  ");
						return -1; 
					}
			}
			else 
			{
				if ((Fdout=dup(stdout))==-1)
					{
						perror("./ish: ");
						return -1; 
					}
			}

		}

		else //in the middle 
		{
			int p[2];
			if (pipe(p)==-1)
			{
				perror("./ish: ");
				return -1; 
			}
			Fdin=p[0];
			Fdout=p[1]; 	
		}

		//redirect the output either to file or to pipe 

		if (dup2(Fdout,1)==-1)
			{
				perror("./ish: ");
				return -1; 
			}
		if (close(Fdout)==-1)
			{
				perror("./ish: ");
				return -1; 
			}

		fflush(NULL);
		iPid=fork();

		if (iPid==-1)
			{
				perror("./ish: ");
		        return -1;
			}
		if (iPid==0)
		{	

			if ((signal(SIGINT, SIG_DFL))== SIG_ERR)
        	{
                perror("./ish: ");
                return -1;
            }

            if ((signal(SIGQUIT, SIG_DFL))== SIG_ERR)
        	{
                perror("./ish: ");
                return -1;
            }

            if ((signal(SIGALRM, SIG_DFL))== SIG_ERR)
        	{
                perror("./ish: ");
                return -1;
            }

			execvp(Cptr->cmd[0],Cptr->cmd);
			perror(Cptr->cmd[0]); 
	        _exit(0);
	    }
	    wait(NULL);
	}

	if (dup2(stdin,0)==-1)
	{
		perror("./ish: ");
		return -1; 
	}
	if (close(stdin)==-1)
	{
		perror("./ish: ");
		return -1; 
	}
	if (dup2(stdout,1)==-1)
	{
		perror("./ish: ");
		return -1; 
	}	

	if (close(stdout)==-1)
	{
		perror("./ish: ");
		return -1; 
	}

	wait(NULL); 
	return 0; 
}

int execute(MainCommand_T MCptr)
{
	/*it calls the execute functions defined above depending on the 
	type of command to be executed.if unsuccessful 
	it prints error messages and returns -1. it takes a pointer
	to a struct MainCommand*/
	if ( (strcmp(MCptr->CommandArray[0]->cmd[0],"cd")==0) || (strcmp(MCptr->CommandArray[0]->cmd[0],"setenv")==0) ||
		(strcmp(MCptr->CommandArray[0]->cmd[0],"unsetenv")==0) || (strcmp(MCptr->CommandArray[0]->cmd[0],"exit")==0))
		executeBuiltin(MCptr); 
	else if (MCptr->count_pipe==0)
		executeNoPipe(MCptr);
	else
		executePipe(MCptr);
	return 0; 

}

int main(void)
{

   /* Read a line from ./ishrc write to stdout and execute. it also 
   ready from stdin and repeat until EOF. then after parsing what 
   it read, it calls execute functions. it also calls signal handler
   for signals.  Return 0 iff successful. */
   sigset_t set; 
   if(sigemptyset(&set)==-1)
   {
   	perror("./ish: ");
   	return -1; 
   }

   if(sigaddset(&set, SIGALRM)==-1)
   {
   	perror("./ish: ");
   	return -1; 
   }

   if(sigaddset(&set, SIGINT)==-1)
   {
   	perror("./ish: ");
   	return -1; 
   }

   if(sigaddset(&set, SIGQUIT)==-1)
   {
   	perror("./ish: ");
   	return -1; 
   }

   if(sigprocmask(SIG_UNBLOCK, &set, NULL)==-1)
   {
   	perror("./ish: ");
   	return -1; 
   }

   if (signal(SIGINT, SIG_IGN) == SIG_ERR)
    {
        perror("./ish: ");
        return -1;
    }
	if (signal(SIGQUIT, QUIT_HANDLER) == SIG_ERR)
    {
        perror("./ish: ");
        return -1;
    }
    
    if (signal(SIGALRM, ALRM_HANDLER) == SIG_ERR)
    {
        perror("./ish: ");
        return -1;
    }
  
   char buf[1024];
   char acLine[1024];
   DynArray_T oTokens;
   int iSuccessful;
   MainCommand_T MainCommand_ptr;
   FILE* fp; 

   char*p= malloc(100*sizeof(char));
   if (p==NULL)
   {
   		return -1; 
   }
   strcpy(p,getenv("HOME")); 

   char *get_file = strcat(p, "/.ishrc");
   fp=fopen(get_file,"r");
   free(p); 

   	if (fp)
   	{
	  	while (fgets(buf, 1024, fp) != NULL) 
	  	{

	  		printf("%% %s", buf);
	  		fflush(NULL);
	  		strtok(buf, "\r");

	  		
	  		oTokens = DynArray_new(0);
		     if (oTokens == NULL)
			 {
			    fprintf(stderr, "Cannot allocate memory\n");
			    exit(EXIT_FAILURE);
		     }
	  		iSuccessful = lexLine(buf, oTokens);
	  		if(iSuccessful&& DynArray_getLength(oTokens)>0)
	  			{	
	  				if(command_check(oTokens)==0)
	  				{
	          			MainCommand_ptr= parse_command(oTokens);
	          			if (MainCommand_ptr!=NULL){
	          				execute(MainCommand_ptr);
	          				freeMainCommand(MainCommand_ptr);
	          			} 
	          			
					}

	     	}
	     	if(DynArray_getLength(oTokens)>0)			
		      	DynArray_map(oTokens, freeToken, NULL);
		      DynArray_free(oTokens); 
	  	}
	 }

  	if(fp != NULL)
        fclose(fp);

    fflush(stdout); 
    fprintf(stdout, "%% ");
    fflush(stdout);
  	while(1)
  	{
  		
  		if (fgets(acLine, 1024, stdin) == NULL)
		{
  			printf("\n");
  			fflush(stdout);
  			exit(0);
		} 
      	oTokens = DynArray_new(0);
	    if (oTokens == NULL)
	    {
         fprintf(stderr, "Cannot allocate memory\n");
         exit(EXIT_FAILURE);
      	}

	    iSuccessful = lexLine(acLine, oTokens);
	    if (iSuccessful&& DynArray_getLength(oTokens)>0)
        {
       		if (command_check(oTokens)==0)
       		{	
	        	MainCommand_ptr= parse_command(oTokens); 
		        if (MainCommand_ptr!=NULL)
		        {
		        	execute(MainCommand_ptr);
		        	freeMainCommand(MainCommand_ptr);
		        }
			}
			
     	}
     	fprintf(stdout,"%% ");
		fflush(stdout);
     	if(DynArray_getLength(oTokens)>0)			
		      	DynArray_map(oTokens, freeToken, NULL);
	    DynArray_free(oTokens);

     
  }
  printf("\n");
  fflush(stdout);
   return 0;
}
