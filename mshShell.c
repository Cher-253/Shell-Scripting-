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

#define MAX_NUM_ARGUMENTS 10     // Mav shell only supports five arguments
//quit,exit,history and cd and !will not be in child, rest in child

int history_filled = 0;//flag to check if the array is filled or not

int main()
{

    char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
    int pidIterator=0;
    int pidBox [15];    
    char * history[15];
    int history_index = 0;
    int histIterator=0;
    for( histIterator=0;histIterator<15;histIterator++)
    {
        history[histIterator] = (char* ) malloc (sizeof(char) * 255);
    }
    int numCommand=0;
    while( 1 )
    {
        // Print out the msh prompt
        printf ("msh> ");

        // Read the command from the commandline.  The
        // maximum command that will be read is MAX_COMMAND_SIZE
        // This while command will wait here until the user
        // inputs something since fgets returns NULL when there
        // is no input
        while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

        if(cmd_str[0]=='!')//using ' ' because it is a character
        {
            //in order to change the number from its ASCII value to the number 
            //subtract ASCII value of 0 from ASCII value of n , //[1] - '0';  
            int newNum = atoi(&cmd_str[1]);
            cmd_str = history[newNum]; //storing it into cmd str 
        }

        //storing the user command into history array
        strncpy(history[history_index++], cmd_str, 255);
        //if the array gets filled with more than 15 elements then move the index to the 
        //0th position and also make the flag true because it is full. 
        if(history_index>14)
        {
            history_index = 0;
            history_filled = 1;
        }

        /* Parse input */
        char *token[MAX_NUM_ARGUMENTS];

        int token_count = 0;                                 
                                                            
        // Pointer to point to the token
        // parsed by strsep
        char *argument_ptr;                                         

        char *newCommandString;

        char *working_str  = strdup( cmd_str );                

        // we are going to move the working_str pointer so
        // keep track of its original value so we can deallocate
        // the correct amount at the end
        char *working_root = working_str;

        // Tokenize the input strings with whitespace used as the delimiter
        while ( ( (argument_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
                (token_count<MAX_NUM_ARGUMENTS))
        {
            token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
            if( strlen( token[token_count] ) == 0 )
            {
                token[token_count] = NULL;
            }
            token_count++;
        }

        if(( cmd_str[0]==' ')|| (cmd_str[0] == '\n') ) continue;
        // Now print the tokenized input as a debug check
        int ret;

        //if command is cd 
        if(strcmp(token[0],"cd")==0)
        {
            ret = chdir( token[1] );  
        }

        //if command is quit 
        else if(strcmp(token[0],"quit")==0)
        {
            exit(0); 
        }

        //if command is exit 
        else if(strcmp(token[0],"exit")==0)
        {
            exit(0);
        }

        //if command is showpids 
        else if(strcmp(token[0],"showpids")==0)
        {
            //showpids returns pids of children processes
            //parents get the child pids only from fork 
            //so collects them and stores them after child processes
            int j=0;
            while(pidBox[j]!=0)
            {
                //printf("The pid values are: %d\n\n",pidBox[j]);
                printf("%d: %d\n",j,pidBox[j]);
                j++;
            }
        }

        //if command is history 
        else if(strcmp(token[0],"history")==0)
        {  
            int f=0;
            int histIndex = history_index;
            //if i replace f with histIndex, then it will print all
            //values till histIndex inside the history array.
            //but once histIndex exceeds 15-> line-198,
            //it will loop back to 0 and the 16th command would be 
            //the first command. so how will i print the last 15 commands in this case?

            //if commands entered are lesser than or equal to 15
            if(history_filled == 0)
            {
            //f loops till histIndex as that is the total number of elements 
            //in the array at that point
                for(f=0;f<histIndex;f++)
                {
                    printf("%d: %s",f,history[f]);    
                }
            }

            //if commands entered are greater than 15
            else
            { 
                for(f=0;f<15;f++)
                {
                    printf("%d: %s",f,history[f]);                              
                }
            }
        } 

        else
        {
            int token_index  = 0;
            pid_t pid = fork( );  
            //this is child process
            if( pid == 0 )
            {
                char *arguments[4];
                arguments[0] = ( char * ) malloc( strlen( token[token_index] ) );
                arguments[1] = ( char * ) malloc( strlen( "-l" ) );
                strncpy( arguments[0], token[token_index], strlen( token[token_index] ) );
                strncpy( arguments[1], "-1", strlen( "-1" ) );
                arguments[2] = NULL;

                int ret2 = execvp( token[0], &token[0] ); 
                if( ret2 == -1 )
                {
                    printf("%s: Command not found.\n", token[token_index]);
                    exit(0);
                }
                //this is where child ends
                if( ret == -1)
                {
                    printf("there is no directory\n");
                }
            }

            //parent continues here,parent process receives the child's pid value
            else 
            {
                int status;
                wait( & status );
                if( pidIterator>14) pidIterator = 0;
                pidBox[pidIterator] = pid;
                pidIterator++;
            } 
        } 
        free( working_root );
    }
    return 0;
}

//Commands to run
//  gcc msh.c -o msh -std=c99 
//  ./msh
