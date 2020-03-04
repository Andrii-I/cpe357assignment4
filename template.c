
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
int fd[2];

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
        printf("my prog$");
        fflush(0);
        dup2(save_stdin,STDIN_FILENO);
        overridemode=0;
        read(STDIN_FILENO,input,1000);
        if(overridemode==0)
            add_null_term(input);//to get a NULL at the end of the string in case of a user input

        if (strncmp(input,"find",4) == 0)
            {
            int sleepcount = input[5]-48; //ASCII conversion
            if (fork() == 0) //child process
                {
                char childreport[10000];
                //search for an empty spot in the child list
                int kidnum=0;
                for(int i=0;i<10;i++) if(childpids[i]==0) {childpids[i]=getpid();kidnum=i;break;}
                //printf("kid %d sleeps for %d seconds to indicate a search\n",kidnum,sleepcount);
                sleep(sleepcount);
                //finding stuff here...
                //finding done.                       
                close(fd[0]); //close read            
                sprintf(childreport,"kid %d is reporting!",kidnum);
                strcat(childreport,"\nfound stuff:\n");
                strcat(childreport,"text.txt in usr/base/blabla\n");
                strcat(childreport,"text.txt in usr/folder2/sub\n");
                strcat(childreport,"\0");//null terminator is important, because pipe!
               //  printf("%d\n",strlen(childreport));
                write(fd[1],childreport,strlen(childreport));
                close(fd[1]); //close write  
                kill(parentPid,SIGUSR1);
               
                return 0;
                }
           
            }
        //killing the kid for good:
        for(int i=0;i<10;i++) if(childpids[i]!=0)       waitpid(childpids[i],0,WNOHANG);

        printf("%s\n",input);
       
        }
    return 0;
    }
