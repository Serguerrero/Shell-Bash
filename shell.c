//Sergio Guerrero 1001612151


#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size
#define MAX_NUM_ARGUMENTS 11     // Mav shell only supports five arguments
#define MAX_PIDS 15


void exitCheck(char quit[]);
void Parse(char** cmd_str, char** parsedArg);
void execCall(char** parsedcmd, int* listofpids, int count);
void printPids(int* listOfpids);
void cdcommand(char** parsed);


//Function to printPids when called.
void printPids(int* listOfpids)
{
 

	for (int j = 0; j < MAX_PIDS; j++)
	{
        /*
        if (listOfpids[j] == 0)
        {
            break;
        }
        */
		printf(" %d: %d\n",j, listOfpids[j]);
	}


}

void cdcommand(char** parsedLine)
{
	

    int check;

  
    //Code used if cd command line is used. 
	if (strcmp(parsedLine[0], "cd") == 0)
	{
        //Call to chdir to change the directory where parsed[1] is the directory
		check = chdir(parsedLine[1]);

        //Prints error incase chdir fails. 
        if (check == -1)
        {
           perror("cd failed: ");
        }

				
    }
			

}

//Function that takes the parsed command and calls the exec function. 
void execCall(char** parsedcmd, int* listofpids, int count) 
{ 
	int i;

    //return to main if either is entered to avoid "exec failed" bug
    if (strcmp(parsedcmd[0], "listpids") == 0 || strcmp(parsedcmd[0], "history") == 0)
    {
        return;
    }

    // Forking a child 
    pid_t pid = fork();  

    //Enter the child pid
    if (pid == 0)
    {
    	int ret = execvp(parsedcmd[0], parsedcmd);

        if(strcmp(parsedcmd[0], "cd") == 0)
        {
            ret = 0;
        }

    	if(ret == -1)
    	{
    		perror("exec failed: ");
    	}

    	exit(0);
    }

    //enter the parent pid
    else if (pid > 0)
    {	
        //Stores the parent pid of the current process into the listofpids.  
        listofpids[count - 1] = pid;


        int stat;

        //wait for child process to complete. 
    	wait(&stat);

    }
    else 
    {
    	printf("Failed to fork a child...\n");
    	
    }

} 

 

 void exitCheck(char quit[])
 {
    //shell can be exited using quit or exit.
    //check to see of either one was entred then exits program. 
 	if (strcmp(quit, "quit") == 0 || strcmp(quit, "exit") == 0)
  	{
  		printf("\n");
  		exit(0);
  	}

 }


//code from msh.c to parse the code and store into parsedArg. 
void Parse(char** cmd_str, char** parsedArg) 
{

	int i;

    /* Parse input */
    
    int token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;                                         
                                                           
    char *working_str  = strdup( *cmd_str );                

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input strings with whitespace used as the delimiter
    while (((argument_ptr = strsep(&working_str, WHITESPACE)) != NULL) && (token_count < MAX_NUM_ARGUMENTS))
    {
      parsedArg[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( parsedArg[token_count] ) == 0)
      {
        parsedArg[token_count] = NULL;
      }
        token_count++;
    }

    exitCheck(*parsedArg);

}


int main()
{

	//Initializations and allocations for vars involved in the cmd line
    //functionality.
	char *buffer = (char*) malloc(MAX_COMMAND_SIZE);
    char * parsedList[MAX_NUM_ARGUMENTS];


    //Initializations and alloactions for vars involved in the history
    //functionality.
    int HistIndex =0; 
    int histnum;
    char* history = (char*) malloc(MAX_PIDS);
    char *singleHistory = (char*) malloc(MAX_COMMAND_SIZE);
    char **arrHistory = malloc(sizeof(char*) * MAX_PIDS);
    char* hist =(char*) malloc(MAX_COMMAND_SIZE * sizeof(char));
    

    //Initializations and alloactions for vars involved in the listpids
    //functionality.
	int* listOfpids = (int*) malloc(MAX_PIDS);
    int count = 0;



	while(1)
	{	

		printf("\nmsh>");

        //count to keep track of how many commands the user has entered. 
        count++;
		
        //stores the line into buffer as long as it is != NULL
		if(fgets(buffer, MAX_COMMAND_SIZE, stdin) != NULL)
		{
		
            //Store the command line into the hitsory array. 
            if((singleHistory= strdup(buffer)) != NULL)
            {

                arrHistory[HistIndex] = singleHistory;
                HistIndex++;
            }       
            else
            {
                perror("cannot save into history array: \n");
            }
            
            //Limits the size of the History array to 15. 
            //and resets if more than 15 cmd have been entered. 
            if(HistIndex > MAX_PIDS)
            {
                HistIndex = 0;
            }


            //if new line was entered, then user is done with command
            //and the command line is ready to be parsed. 
			if (strcmp(buffer, "\n") == 0)
			{
				continue;
                
			}
            

			Parse(&buffer, parsedList);

            //Store the first cmd from the buffer into the hist array
            //to later be able to traverse through it char by char.
            hist = parsedList[0];
            
            //Listpids call check
			if (strcmp(parsedList[0], "listpids") == 0)
			{
				printPids(listOfpids);
			}
            //History call check
            else if (strcmp(parsedList[0], "history") == 0)
            {
                
                // for loop to print out the history array.
                for(int n = 0; n < MAX_PIDS ; n++) 
                {
                 if(arrHistory[n] != NULL)
                 {
                    printf("command %d: %s\n", n, arrHistory[n]);
                 }
                 
                }
                
    
            }
            //check to see if cd is the command being used.
            else if(strcmp(parsedList[0], "cd") == 0)
            {
                cdcommand(parsedList);
                
        
             }
            //Check is user is attempting to recall a command 
            //from thr history array. 
            else if (hist[0] == '!')
            {
                //histnum is the number from the array the user wants to re-run
                histnum = atoi(&hist[1]);

                //fail check if number entered is out of bounds. 
                if (histnum > count)
                {
                    printf("Command not in history...\n");
                    continue;
                }

                //If all passes, cmd gets parsed and ran as it normally would. 
                Parse(&arrHistory[histnum], parsedList);

            }
            


			execCall(parsedList, listOfpids, count);


		}
		
		
	}


//Free all allocated memory. 
free(listOfpids);
free(buffer);
free(history);
free(singleHistory);
free(arrHistory);
free(hist);

return 0;
}





