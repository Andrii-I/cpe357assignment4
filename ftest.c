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

void findFilename(char* filename, bool sflag, int* kids)
{
    struct dirent* dent;
    struct stat st;
    DIR* dir;
    char mystring[500];
    char* mybuffer = mystring;
    int found = 0;
    
    char found_files[100][100] = {0};
    char dirs[100][100];
    int dirnum = 1;
    for (int i = 0; i < 100; i++)
    {
        found_files[i][0] = 0;
        dirs[i][0] = 0;
    } 
    //strcpy(dirs[0], ".");
    getcwd(dirs[0], 100);

    for (int i = 0; dirs[i] != 0; i++)
    {         
        dir = opendir(dirs[i]);
        //printf("%s\n", dirs[i]);
        for (dent = readdir(dir); dent != NULL; dent = readdir(dir)) 
        {
            //printf("%s\n", dent->d_name);
            stat(dent->d_name, &st);
            if(S_ISREG(st.st_mode) && 
                strcmp(((const char*)(&(dent->d_name))), filename) == 0  )
            {               
                char filepath[500];
                strcpy(filepath, dirs[i]);
                strcat(filepath, "/");
                strcat(filepath, filename);
                fflush(0);
                printf("%s\n", filepath);
                fflush(0);
                found = 1;                            
            }
            if (sflag == 0 && S_ISDIR(st.st_mode) == 1 && 
                strcmp(((const char*)(&(dent->d_name))), ".") != 0 &&
                strcmp(((const char*)(&(dent->d_name))), "..") != 0 &&
                (dent->d_name)[0] != '.'); //change sflag to 1
            {
                printf("folder: %s, ISDIR: %i\n", dent->d_name, (int)S_ISDIR(st.st_mode));
                char new_subfolder[500];

                strcpy(new_subfolder, dirs[i]);
                strcat(new_subfolder, "/");
                strcat(new_subfolder, dent->d_name);
                strcpy(dirs[dirnum], new_subfolder);
                dirnum++;
            }
        }
        closedir(dir);
    }
    closedir(dir);
        


    if (!found)
    {
        printf(">nothing found<\n");
        fflush(0);
    }        

    closedir(dir);
    return;
}

void main()
{
    int* kids = mmap(NULL, sizeof(int)*10, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    char* arg1 = "a.out"; 
    findFilename(arg1, 0, kids);
}