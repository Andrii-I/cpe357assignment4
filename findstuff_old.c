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
#include <string.h>
#include <stdbool.h>

#define COLOR_BLUE "\x1b[34m"
#define COLOR_RESET "\x1b[0m"

int fd[2];

//////////////////////////////////////////////////////////////////////
// get argument function
//
// return true if ok and false if error or no further argument
// line .. the command line
// argn .. the to extracted argument beginning with 0
// result .. should be a static char array of 1000 bytes
//
// it respects quotation marks!!
// its also not much tested
// 
// 
//////////////////////////////////////////////////////////////////////
bool get_argument(char* line, int argn, char* result)
	{
  	//firstly: remove all spaces at the front
  	char temp[1000];
  	int start_space = 1;
  	for (int i = 0, u = 0; i <= strlen(line); i++)
  		if (line[i] == ' ' && start_space == 1) continue;
  		else 
  			{
  			temp[u++] = line[i]; 
  			start_space = 0;
  			}
  	//now remove an double or tripple spaces
  	char temp2[1000];
  	int space_on = 0;
  	for (int i = 0, u = 0; i <= strlen(temp); i++)
  		{
  		if (space_on == 1 && temp[i] == ' ') continue;
  		temp2[u++] = temp[i];
  		if (temp[i] == ' ') space_on = 1;
  		else space_on = 0;
  		}
  	//finally extract the arguments
  	int start, end;
  	start = end = 0;
  	int count = 0;
  	int quote = 0;
  	for (int i = 0; i <= strlen(temp2); i++)
  		if (temp2[i] == '\"') quote = !quote;
  		else if (quote == 0 &&(temp2[i] == ' ' || temp2[i] == 0))
  			{
  			end = i;
  			if (argn == count)
  				{
  				int length = end - start;
  				strncpy(result, temp2 + start, length);
  				result[length] = 0;
                  
  				return 1;
  				}
  			start = end + 1;
  			count++;
  			}
  	return 0;
	}

char usage[] = "\nusage:\nfind <filename> [-s]\nfind <\"text\"> [-f:<file_ending>] [-s]\nkill <-pid>\nlist\nquit (q)\n\n";

int overridemode=0;
void myfct(int y)
    {
    dup2(fd[0],STDIN_FILENO); //Overwrite userinput
    overridemode=1;
    }

/* int findFilename(char* filename, bool sflag, int* childpids, int parentPid)
{
    struct dirent* dent;
    struct stat st;
    DIR* dir;
    char mystring[500];
    char* mybuffer = mystring;
    getcwd(mybuffer, 500);
    dir = opendir(mybuffer);
    int found = 0;

    int kidnum = 0;

    char childreport[500];

 
    for (dent = readdir(dir); dent != NULL; dent = readdir(dir))
    {
        stat(dent->d_name, &st);
        if(S_ISREG(st.st_mode) && strcmp(((const char*)(&(dent->d_name))), filename) == 0  )
        {               
            sprintf(childreport,"\nkid %d is reporting!",kidnum);
            strcat(childreport,"\nfound stuff:\n");
            strncat(childreport, filename, sizeof(filename) - 1);
            strcat(childreport, " in ");
            strcat(childreport, mybuffer);
            strcat(childreport,"\n\n\0");
            found = 1;

            sleep(5);
            close(fd[0]);
            write(fd[1],childreport,strlen(childreport));
            close(fd[1]); 
            kill(parentPid,SIGUSR1);                       
        }
    }
    if (!found)
    {
        printf("kid %d is reporting!",kidnum);
        printf(">nothing found<\n");
        fflush(0);
    } 
    else
    {
        sleep(5);
        close(fd[0]);
        write(fd[1],childreport,strlen(childreport));
        close(fd[1]); 
        kill(parentPid,SIGUSR1);
    }       

    closedir(dir);
} */

void main()
{   
    int *childpids = mmap(0,sizeof(int)*10,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    for(int i=0;i<10;i++) 
    {
        childpids[i]=0;
    }
    
    signal(SIGUSR1,myfct);
    //char str_in[1000];
    int parentPid = getpid();
    pipe(fd);
    int save_stdin = dup(STDIN_FILENO);

    while(1)
    {
        //printf("my prog$");
        fflush(0);
        dup2(save_stdin,STDIN_FILENO);
        //read(STDIN_FILENO, str_in, 100);

        char* str_in = "find a.out\n";

        //printf("my prog$");
        fflush(0);
        dup2(save_stdin,STDIN_FILENO);

        char t[100];            
        if ( get_argument(str_in, 0, t) == 1 && 
             get_argument(str_in, 1, t) == 0)
        {           
            char arg0[100];
            get_argument(str_in, 0, arg0);
            if ( (int)(arg0[strlen(arg0) - 1]) == 10 )
            {
                arg0[strlen(arg0) - 1] = '\0';
            }
            

            if (strcmp(arg0, "quit") == 0 || strcmp(arg0, "q") == 0)  
            {
                printf("%s\n", "REMINDER: ADD QUIT SEQUENCE");
                exit(0);
            }
            else if (strcmp(arg0, "list") == 0)
            {
                printf("LISTING\n"); //lists all running child processes and what they try to do. Also displays their serial number.
            }
            else 
            {
                printf("%s", usage);
            } 
        }
        else if (get_argument(str_in, 0, t) == 1 && 
                 strcmp(t, "kill") == 0 &&
                 get_argument(str_in, 1, t) == 1 &&
                 get_argument(str_in, 2, t) == 0 )
        {
            char arg1[100];
            get_argument(str_in, 1, arg1);
            if ( (int)(arg1[strlen(arg1) - 1]) == 10 )
            {
                arg1[strlen(arg1) - 1] = '\0';
            }

            if (arg1[0] == '-') 
            {
                printf("KILL THE CHILD\n"); //kill <num> kills a child process, and so ends its finding attemps
            }
            else
            {
                printf("%s", usage);
            }           
        }
        else if (get_argument(str_in, 0, t) == 1 && 
                 strcmp(t, "find") == 0 &&
                 get_argument(str_in, 1, t) == 1)
        {
            char arg1[100];
            if (get_argument(str_in, 1, arg1) == 1)
            {
                if ( (int)(arg1[strlen(arg1) - 1]) == 10 )
                {
                    arg1[strlen(arg1) - 1] = '\0';
                }
                
                if (arg1[0] == '\"' && arg1[strlen(arg1) - 1] == '\"')
                {
                    char arg2[100];
                    if (get_argument(str_in, 2, arg2) == 1)
                    {
                        if ( (int)(arg2[strlen(arg2) - 1]) == 10 )
                        {
                            arg2[strlen(arg2) - 1] = '\0';
                        }

                        if (strncmp(arg2, "-f:", 3) == 0)
                        {
                            char arg3[100];
                            if (get_argument(str_in, 3, arg3) == 1 &&
                                strncmp(arg3, "-s\n", 2) == 0)
                            {
                                printf("find \"text\" w -f:xyz w -s\n");
                            }
                            else if (get_argument(str_in, 3, arg3) == 0)
                            {
                                printf("find \"text\" w -f:xyz w/o -s\n");
                            }
                        }
                        else if (strncmp(arg2, "-s\n", 3) == 0 && 
                                 get_argument(str_in, 3, t) == 0)
                        {
                            printf("find \"text\" w -s\n");
                        }
                        else
                        {
                            printf("%s", usage);
                        }
                        
                    }
                    else if (get_argument(str_in, 2, t) == 0)
                    {
                        printf("find \"text\" w/o -f:xyz or -s\n");
                    }
                }
                else 
                {
                    char arg2[100];
                    if (get_argument(str_in, 2, arg2) == 0)
                    {  
                        printf("find filename w/o -s\n");
                    
                        struct dirent* dent;
                        struct stat st;
                        DIR* dir;
                        char mystring[500];
                        char* mybuffer = mystring;
                        getcwd(mybuffer, 500);
                        dir = opendir(mybuffer);
                        int found = 0;
                    
                        int kidnum = 0;
                    
                        char childreport[500];
                    
                     
                        for (dent = readdir(dir); dent != NULL; dent = readdir(dir))
                        {
                            stat(dent->d_name, &st);
                            if(S_ISREG(st.st_mode) && strcmp(((const char*)(&(dent->d_name))), arg1) == 0  )
                            {               
                                sprintf(childreport,"\nkid %d is reporting!",kidnum);
                                strcat(childreport,"\nfound stuff:\n");
                                strncat(childreport, arg1, sizeof(arg1) - 1);
                                strcat(childreport, " in ");
                                strcat(childreport, mybuffer);
                                strcat(childreport,"\n\n\0");
                                found = 1;                      
                            }
                        }
                        if (!found)
                        {
                            printf("kid %d is reporting!",kidnum);
                            printf(">nothing found<\n");
                            fflush(0);
                        } 
                        else
                        {
                            sleep(5);
                            close(fd[0]);
                            write(fd[1],childreport,strlen(childreport));
                            close(fd[1]); 
                            kill(parentPid,SIGUSR1);
                        }    


                    }
                    else if (strncmp(arg2, "-s\n", 3) == 0)
                    {
                        printf("find filename w -s\n");
                        //findFilename(arg1, 1, childpids, parentPid);
                    }
                    else
                    {
                        printf("%s", usage);
                    }
                }
            }
            else
            {
                printf("%s", usage);
            }
        }
        else 
        {
            printf("%s", usage);
        } 
        break;
    }     
}