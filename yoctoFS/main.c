#include <stdio.h>
#include "yoctofs.h"

#define INPUT_LEN 50
#define IS_EQUALS(x, y) (strcmp(x, y) == 0)

/*
TODO:
# Add size to ls
# Add date last change
# Add command history with arrows
*/

extern fsObj *g_pCurr;
void removeSpaces(char *s);

int main()
{
    char input[INPUT_LEN];
    char command[INPUT_LEN];
    char path[INPUT_LEN];
    char text[INPUT_LEN];
    char *pSep;
    char *pPipe;
    fsObj *tmp;

    printf("mkdir/touch/rm/ls/cd/pwd/echo/cat/help/quit\n");
    printPath();
    printf("$ ");
    fgets(input, INPUT_LEN, stdin);

    while (strcmp(input, "quit\n") != 0)
    {
        pSep = strchr(input, ' ');

        if (pSep)
        {
            strncpy(command, input, pSep - input);
            command[pSep - input] = '\0';
            strcpy(path, pSep + 1);

            /* Override newline*/
            path[strlen(path) - 1] = '\0';

            if (IS_EQUALS(command, "ls"))
            {
                listDir(path);
            }
            else if (IS_EQUALS(command, "cd"))
            {
                tmp = g_pCurr;

                if (findFsObj(path, &tmp) && tmp->isDir)
                {
                    g_pCurr = tmp;
                }
                else if (!tmp)
                {
                    printf("Path does not exist\n");
                }
                else if (!(tmp->isDir))
                {
                    printf("Path is a file not a directory\n");
                }
            }
            else if (IS_EQUALS(command, "mkdir"))
            {
                makeFsObj(path, true);
            }
            else if (IS_EQUALS(command, "touch"))
            {
                makeFsObj(path, false);
            }
            else if (IS_EQUALS(command, "echo"))
            {
                strcpy(text, path);
                pPipe = strchr(text, '>');
                text[pPipe - text] = '\0';
                strcpy(path, pPipe + 1);

                removeSpaces(path);

                if (findFsObj(path, &tmp))
                {
                    if (tmp->isDir)
                    {
                        printf("Path is a directory not a file\n");
                    }
                    else
                    {
                        if (tmp->content != NULL)
                        {
                            free(tmp->content);
                        }
                        tmp->content = malloc(strlen(text));
                        memcpy(tmp->content, text, strlen(text) + 1);
                    }
                }
                else
                {
                    printf("Path does not exist\n");
                }
            }
            else if (IS_EQUALS(command, "cat"))
            {
                if (findFsObj(path, &tmp))
                {
                    if (tmp->isDir)
                    {
                        printf("Path is a directory not a file\n");
                    }
                    else
                    {
                        printf("%s\n", tmp->content ? (char *)tmp->content: "Empty file");
                    }
                }
                else
                {
                    printf("Path does not exist\n");
                }
            }
            else if (IS_EQUALS(command, "rm"))
            {
                removePath(path);
            }
            else
            {
                printf("Invalid command\n");
            }
        }
        else if (IS_EQUALS(input, "ls\n"))
        {
            listDir("");
        }
        else if (IS_EQUALS(input, "pwd\n"))
        {
            printPath();
            printf("\n");
        }
        else if (IS_EQUALS(input, "help\n"))
        {
            printf("mkdir/touch/rm/ls/cd/quit/pwd/help\n");
        }
        else
        {
            printf("Argument must follow command\n");
        }

        printPath();
        printf("$ ");
        fgets(input, INPUT_LEN, stdin);
    }
    return 0;
}

void removeSpaces(char *s)
{
    char *d = s;
    do
    {
        while (*d == ' ')
        {
            ++d;
        }
    } while ((*s++ = *d++));
}