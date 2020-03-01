#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#define COLOR_BLUE "\x1b[34m"
#define COLOR_RESET "\x1b[0m"

//int extractArguments(char* argvptr); // returns num of args

void main()
{

    while(1)
    {
        //*** GET INPUT AND SPLIT IT INTO 2D ARRAY ***
        char str_in[100];
        char argv[10][10]; //can store 10 words of 10 characters
        int i,j,argc;
        i = j = argc = 0;

        read(STDIN_FILENO, str_in, sizeof(str_in));
     
        for(; i <= (strlen(str_in)); i++)
        {
            if(str_in[i] == ' ')
            {
                argv[argc][j] = '\0';
                argc++;
                j=0;
            }
            else if (str_in[i] == '\0')
            {
                argv[argc][j - 1] = '\0';
                argc++;
            }
            else
            {
                argv[argc][j] = str_in[i];
                j++;
            }
        }
        //***



        if (argc == 1)
        {           
            if (strcmp(argv[0], "quit") == 0 || strcmp(argv[0], "q") == 0)  
            {
                printf("%s\n", "REMINDER: ADD QUIT SEQUENCE");
                exit(0);
            }
            else if (strcmp(argv[0], "list") == 0)
            {
                printf("LISTING\n"); //lists all running child processes and what they try to do. Also displays their serial number.
            }
        }
        else if (argc == 2 && strcmp(argv[0], "kill") == 0)
        {
            printf("KILL THE CHILD\n"); //kill <num> kills a child process, and so ends its finding attemps
        }
        else //if (argc < 2 || argc > 4)
        {
            printf("usage: command [SEARCHTERM] [-f:FILEEXTENSION] [-s]\n");
        }



    }    
}