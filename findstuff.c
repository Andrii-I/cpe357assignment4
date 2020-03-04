
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h> 
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdbool.h>

int fd[2];
char usage[] = "\nusage:\nfind <filename> [-s]\nfind <\"text\"> [-f:<file_ending>] [-s]\nkill <-pid>\nlist\nquit (q)\n\n\0";

int get_argument(char* line, int argn, char* result)
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
	//secondly: remove all spaces at the back
	char temp2[1000];
	int end_space = 1;
	for (int i = strlen(temp) - 1; i >= 0; i--)
		if (temp[i] == ' ' && end_space == 1) continue;
		else
		{
			temp2[i] = temp[i];
			if (end_space == 1) temp2[i + 1] = 0;
			end_space = 0;
		}
	//remove new line character if its there!
	int j = strlen(temp2)-1;
	if (temp2[j] == '\n') temp2[j] = 0;
	//now remove an double or tripple spaces
	char temp3[1000];
	int space_on = 0;
	for (int i = 0, u = 0; i <= strlen(temp2); i++)
	{
		if (space_on == 1 && temp2[i] == ' ') continue;
		temp2[u++] = temp2[i];
		if (temp2[i] == ' ') space_on = 1;
		else space_on = 0;
	}
	//finally extract the arguments
	int start, end;
	start = end = 0;
	int count = 0;
	int quote = 0;
	for (int i = 0; i <= strlen(temp2); i++)
		if (temp2[i] == '\"') quote = !quote;
		else if (quote == 0 && (temp2[i] == ' ' || temp2[i] == 0))
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

void add_null_term(char *txt)
{
for(int i=0;i<100;i++)
    if(txt[i]=='\n') {txt[i+1]=0;break;}
}


int overridemode=0;
void myfct(int y)
    {
    dup2(fd[0],STDIN_FILENO); //Overwrite userinput
    overridemode=1;
    }


void findFilesRecursively(char *basePath, char* filename, char* reportWIP, char* found, int* first)
{
    char path[1000];
    struct dirent *dent;
    struct stat st;
    DIR *dir = opendir(basePath);

    // Unable to open directory stream
    if (!dir)
        return;

    while ((dent = readdir(dir)) != NULL)
    {
        if (strcmp(dent->d_name, ".") != 0 && strcmp(dent->d_name, "..") != 0)
        {
            stat(dent->d_name, &st);
            if ( S_ISREG(st.st_mode) && strcmp(((const char*)(&(dent->d_name))), filename) == 0 )
            {
                if (*first == 1)
                {
                    strncpy(reportWIP, filename, sizeof(filename) - 1);
                    *first = 0;
                }
                else
                {
                    strncat(reportWIP, filename, sizeof(filename) - 1);
                }
                
                strcat(reportWIP, " in ");
                strcat(reportWIP, basePath);
                strcat(reportWIP,"\n");
                *found = 1;
            }

            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dent->d_name);

            findFilesRecursively(path, filename, reportWIP, found, first);
        }
    }

    closedir(dir);
}

int main()
    {  
    int *childpids = mmap(0,sizeof(int)*10,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    for(int i=0;i<10;i++) childpids[i]=0;

    signal(SIGUSR1,myfct);
    char input[1000];
    int parentPid = getpid();
    pipe(fd);
    int save_stdin = dup(STDIN_FILENO);
    while (1)
        {
        int* input_rec = mmap(0,sizeof(int)*10,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
        *input_rec = 0;
        printf("my prog$");
        fflush(0);
        dup2(save_stdin,STDIN_FILENO);
        overridemode=0;
        read(STDIN_FILENO,input,1000);
        if(overridemode==0)
            add_null_term(input);//to get a NULL at the end of the string in case of a user input

        char arg0[1000];
        char arg1[1000];
        char arg2[1000];
        char arg3[1000];

        // FINDFILE SIMPLE
        if ( get_argument(input, 0, arg0) == 1 && 
             get_argument(input, 1, arg1) == 1 &&
             get_argument(input, 2, arg2) == 0 &&
             strcmp(arg0,"find") == 0)
        //if (strncmp(input,"find",4) == 0)
            {
            //int sleepcount = input[5]-48; //ASCII conversion
            *input_rec = 1;
            if (fork() == 0) //child process
                {
                char childreport[10000];
                char reportWIP[10000];
                //search for an empty spot in the child list
                int kidnum=0;
                for(int i=0;i<10;i++) if(childpids[i]==0) {childpids[i]=getpid();kidnum=i;break;}
                //printf("kid %d sleeps for %d seconds to indicate a search\n",kidnum,sleepcount);
                //sleep(sleepcount);
                //finding stuff here...

                struct dirent* dent;
                struct stat st;
                DIR* dir;
                char mystring[10000];
                char* mybuffer = mystring;
                getcwd(mybuffer, 10000);
                dir = opendir(mybuffer);
                int found = 0;

                char* filename = arg1;

                for (dent = readdir(dir); dent != NULL; dent = readdir(dir))
                {
                    stat(dent->d_name, &st);
                    if(S_ISREG(st.st_mode) && strcmp(((const char*)(&(dent->d_name))), filename) == 0  )
                    {               
                        sprintf(reportWIP,"\n\nkid %d is reporting!\n",kidnum);
                        strcat(reportWIP,"found stuff:\n");
                        strcat(reportWIP, filename);
                        strcat(reportWIP, " in ");
                        strcat(reportWIP, mybuffer);
                        strcat(reportWIP,"\n\n\0");
                        found = 1;              
                    }
                }
                if (found == 0)
                {
                    sprintf(reportWIP,"\n\nkid %d is reporting!\n",kidnum);
                    strcat(reportWIP, ">File not found<\n\0");
                }

                
                //finding done.                       
                close(fd[0]); //close read    
                strcpy(childreport, reportWIP);        
                write(fd[1],childreport,strlen(childreport));
                close(fd[1]); //close write  
                kill(parentPid,SIGUSR1);
               
                return 0;
                }
           
            }
        else if (strncmp(input,"finr",4) == 0)
        {
            
            *input_rec = 1;
            //int sleepcount = input[5]-48; //ASCII conversion
            if (fork() == 0) //child process
                {
                char reportWIP[10000];
                char reportWIP2[10000];
                char childreport[10000];
                char mystring[10000];
                char* mybuffer = mystring;
                getcwd(mybuffer, 10000);
                int found = 0; 
                int first = 1; 

                char* filename = "a.out"; //TO DELETE

                //search for an empty spot in the child list
                int kidnum=0;
                for(int i=0;i<10;i++) if(childpids[i]==0) {childpids[i]=getpid();kidnum=i;break;}
                //printf("kid %d sleeps for %d seconds to indicate a search\n",kidnum,sleepcount);
                //sleep(sleepcount);
                
                //finding stuff here...
                findFilesRecursively(mybuffer, filename, reportWIP, ((char *)(&found)), ((int *)(&first))); //change "aba.out"
                
                //finding done.
                if (found == 1)
                {
                    sprintf(reportWIP2,"\n\nkid %d is reporting!\n",kidnum);
                    strcat(reportWIP2,"found stuff:\n");
                    strcat(reportWIP2, reportWIP);
                    strcat(reportWIP2,"\n\0");
                }
                else
                {
                    sprintf(reportWIP2,"\n\nkid %d is reporting!\n",kidnum);
                    strcat(reportWIP2, ">File not found<\n\n\0");
                }
            
                close(fd[0]); //close read    
                strcpy(childreport, reportWIP2);        
                write(fd[1],childreport,strlen(childreport));
                close(fd[1]); //close write  
                kill(parentPid,SIGUSR1);
               
                return 0;
                }
        
        }
        else if ()
        {
            
        }


/*         else if (*input_rec == 0)
        {
        
            //int sleepcount = input[5]-48; //ASCII conversion
            if (fork() == 0) //child process
                {
                char reportWIP[10000];
                char reportWIP2[10000];
                char childreport[10000];
                char mystring[10000];
                char* mybuffer = mystring;
                getcwd(mybuffer, 10000);
                int found = 0; 
                int first = 1; 

                //search for an empty spot in the child list
                int kidnum=0;
                for(int i=0;i<10;i++) if(childpids[i]==0) {childpids[i]=getpid();kidnum=i;break;}
                //printf("kid %d sleeps for %d seconds to indicate a search\n",kidnum,sleepcount);

                strcpy(reportWIP2, usage);
            
                close(fd[0]); //close read    
                strcpy(childreport, reportWIP2);        
                write(fd[1],childreport,strlen(childreport));
                close(fd[1]); //close write  
                kill(parentPid,SIGUSR1);
               
                return 0;
                }
        
        }  */
        

        

        
        
        //killing the kid for good:
        for(int i=0;i<10;i++) if(childpids[i]!=0)       waitpid(childpids[i],0,WNOHANG);

        printf("%s\n", input);

        if (*input_rec == 0)
        {
            printf("\n%s\n", usage);
        }

        }
    return 0;
    }