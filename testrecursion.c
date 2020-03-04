/**
 * C program to list contents of a directory recursively.
 */

#include <stdio.h>
#include <string.h>

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

/**
 * Lists all files and sub-directories recursively 
 * considering path as base path.
 */
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
    // Directory path to list files
    char path[100];
    char reportWIP[10000];
    char reportWIP2[10000];
    char childreport[10000];
    int found = 0; 
    int kidnum = 0;
    int first = 1; 

    
    // Input path from user
    printf("Enter path to list files: ");
    scanf("%s", path);


    findFilesRecursively(path, "a.out", reportWIP, ((char *)(&found)), ((int *)(&first)));

    if (found == 1)
    {
        sprintf(reportWIP2,"\nkid %d is reporting!\n",kidnum);
        strcat(reportWIP2,"found stuff:\n");
        strcat(reportWIP2, reportWIP);
        strcat(reportWIP2,"\n\0");

    }
    else
    {
        sprintf(reportWIP2,"\nkid %d is reporting!\n",kidnum);
        strcat(reportWIP2, ">File not found<\n\n\0");
    }
    

    strcpy(childreport, reportWIP2); 

    printf("%s", childreport);

    return 0;
}


