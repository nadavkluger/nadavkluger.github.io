#include "yoctofs.h"

fsObj g_root = {"/", true, NULL, NULL, NULL};
fsObj *g_pCurr = &g_root;

bool findDirInDir(fsObj **dir, char *name);
void parsePathStrToArr(char *path, char ***arr, int *length);
void removeDir(char *path, fsObj *pLocation);

void printPath()
{
    char *path = malloc(sizeof(char));
    char *lastPath = NULL;
    int size = 0;
    fsObj *tmp = g_pCurr;

    path[0] = '\0';

    while (tmp->parent)
    {
        size += strlen(tmp->name);
        path = realloc(path, (size + 1) * sizeof(char));
        lastPath = realloc(lastPath, (strlen(path) + 1) * sizeof(char));
        strcpy(lastPath, path);
        strcpy(path, tmp->name);
        strcat(path, "/");
        strcat(path, lastPath);
        tmp = tmp->parent;
    }

    printf("%s", g_root.name);

    if (size > 0)
    {
        printf("%s", path);
    }

    free(path);
    free(lastPath);
}

void listDir(char *path)
{
    fsObj *tmp;
    if (findFsObj(path, &tmp) && tmp->isDir)
    {
        tmp = (fsObj *)tmp->content;

        if (tmp)
        {
            while (tmp)
            {
                printf("%s", tmp->name);
                if (tmp->isDir)
                {
                    printf("/");
                }
                printf("\n");
                tmp = tmp->next;
            }
        }
        else
        {
            printf("Empty directory\n");
        }
    }
    else if (!tmp)
    {
        printf("Path does not exist\n");
    }
    else if (!(tmp->isDir))
    {
        printf("Path is a file, not a directory\n");
    }
}

void removeDir(char *path, fsObj *pLocation)
{
    char *concatedPath = NULL;
    int pathLen = strlen(path);
    fsObj *tmp;

    tmp = ((fsObj *)(pLocation->content));

    while (tmp)
    {
        concatedPath = realloc(concatedPath, (pathLen + strlen(tmp->name) + 2) * sizeof(char));
        strcpy(concatedPath, path);
        strcat(concatedPath, "/");
        strcat(concatedPath, tmp->name);
        tmp = tmp->next;
        removePath(concatedPath);
    }

    free(concatedPath);
}
void removePath(char *path)
{
    fsObj *pLocation;
    fsObj *tmp;
    int pathLen = strlen(path) - 1;

    // Could be either bla/ and bla
    if (path[0] == '/' && strstr(path, g_pCurr->name) >= (path + pathLen - 1 - strlen(g_pCurr->name)))
    {
        printf("Cannot delete an open directory\n");
        return;
    }
    if (findFsObj(path, &pLocation) && pLocation->isDir)
    {
        if (strstr(path, "..") != 0)
        {
            removeDir(path, pLocation);
        }
    }
    else if (!pLocation)
    {
        printf("Path '%s' does not exist\n", path);
        return;
    }

    tmp = (fsObj *)((pLocation->parent)->content);

    if (tmp == pLocation)
    {
        ((pLocation->parent)->content) = pLocation->next;
    }
    else
    {
        while (tmp->next != pLocation)
        {
            tmp = tmp->next;
        }

        tmp->next = pLocation->next;
    }

    free(pLocation->content);
    free(pLocation);
}

void makeFsObj(char *path, bool isDir)
{
    int size;
    int pathLen = strlen(path);
    int i;
    char **pathArr;
    char *realPath = malloc(pathLen);
    int objNameLen;
    fsObj *tmp;
    fsObj *parent;
    fsObj tmp2copy;

    parsePathStrToArr(path, &pathArr, &size);

    if (size == 0)
    {
        printf("Invalid path\n");
        return;
    }

    objNameLen = strlen(pathArr[size - 1]) + (path[pathLen - 1] == '/');
    strncpy(realPath, path, pathLen - objNameLen);
    realPath[pathLen - objNameLen] = '\0';

    if (findFsObj(realPath, &parent) && parent->isDir)
    {
        tmp = ((fsObj *)(parent->content));

        if (tmp)
        {
            while (tmp->next)
            {
                tmp = tmp->next;
            }

            tmp->next = malloc(sizeof(fsObj));
            tmp = tmp->next;
        }
        else
        {
            parent->content = malloc(sizeof(fsObj));
            tmp = ((fsObj *)(parent->content));
        }
        tmp2copy = (fsObj){.name = "", .isDir = isDir, .parent = parent, .content = NULL, .next = NULL};
        memcpy(tmp, &tmp2copy, sizeof(fsObj));
        strcpy(tmp->name, pathArr[size - 1]);

        if (isDir)
        {
            tmp->content = (fsObj *)malloc(sizeof(fsObj));
            tmp2copy = (fsObj){.name = "..", .isDir = false, .parent = tmp, .content = NULL, .next = NULL};
            memcpy(tmp->content, &tmp2copy, sizeof(fsObj));
        }
    }
    else
    {
        if (!parent)
        {
            printf("Path does not exist\n");
        }
        else if (!(parent->isDir))
        {
            printf("Path is a file, not a directory\n");
        }
        else
        {
            printf("Oh snap!");
        }
    }

    for (i = 0; i < size; i++)
    {
        free(pathArr[i]);
    }

    free(realPath);
    free(pathArr);
}

void parsePathStrToArr(char *path, char ***arr, int *length)
{
    char *pSep;
    char *pLastMatch = path - 1;
    char subPath[MAX_NAME_LEN];
    int pathLen = strlen(path);

    (*arr) = NULL;
    (*length) = 0;

    if ((pathLen == 0) || strchr(path, ' ') || strstr(path, "//"))
    {
        return;
    }

    if (path[0] == '/')
    {
        *arr = realloc(*arr, sizeof(char *));
        (*arr)[0] = malloc((2) * sizeof(char));
        strcpy((*arr)[(*length)++], "/");
        pSep = strchr((++pLastMatch) + 1, '/');
    }
    else
    {
        pSep = strchr(path, '/');
    }

    while (pSep)
    {
        strncpy(subPath, pLastMatch + 1, pSep - pLastMatch - 1);
        subPath[pSep - pLastMatch - 1] = '\0';

        *arr = realloc(*arr, (*length) * sizeof(char *));
        (*arr)[(*length)] = malloc((1 + strlen(subPath)) * sizeof(char));
        strcpy((*arr)[(*length)], subPath);
        (*length)++;

        pLastMatch = pSep;
        pSep = strchr(pSep + 1, '/');
    }

    if (pLastMatch < path || pLastMatch != path + pathLen - 1)
    {
        strcpy(subPath, pLastMatch + 1);
        *arr = realloc(*arr, (*length) * sizeof(char *));
        (*arr)[(*length)] = malloc((1 + strlen(subPath)) * sizeof(char));

        strcpy((*arr)[(*length)], subPath);

        (*length)++;
    }
}

bool findObjInDir(fsObj **dir, char *name)
{
    fsObj *tmp = (fsObj *)((*dir)->content);

    if (strcmp(name, "..") == 0)
    {
        (*dir) = (*dir)->parent;
        return (*dir);
    }

    while (tmp)
    {
        if (strcmp(tmp->name, name) == 0)
        {
            break;
        }

        tmp = tmp->next;
    }

    if (tmp)
    {
        *dir = tmp;
        return true;
    }
    else
    {
        *dir = NULL;
        return false;
    }
}
bool findFsObj(char *path, fsObj **dir)
{
    char subPath[MAX_NAME_LEN];
    char *pSep;
    char *pLastMatch = path - 1;
    int pathLen = strlen(path);

    if ((strchr(path, ' ')) || strstr(path, "//"))
    {

        *dir = NULL;
        return false;
    }
    if (pathLen == 0 || (pathLen == 1 && path[0] == '.'))
    {
        *dir = g_pCurr;
        return true;
    }

    if (path[0] == '/')
    {
        *dir = &g_root;

        if (pathLen == 1)
        {
            return true;
        }
        pLastMatch++;
        pSep = strchr(path + 1, '/');
    }
    else
    {
        *dir = g_pCurr;
        pSep = strchr(path, '/');
    }

    while (pSep)
    {
        strncpy(subPath, pLastMatch + 1, pSep - pLastMatch - 1);
        subPath[pSep - pLastMatch - 1] = '\0';

        if (!findObjInDir(dir, subPath))
        {
            return false;
        }
        if (!(*dir)->isDir)
        {
            return true;
        }
        pLastMatch = pSep;
        pSep = strchr(pSep + 1, '/');
    }

    if (pLastMatch < path || *pLastMatch != path[pathLen - 1])
    {
        strcpy(subPath, pLastMatch + 1);
        return findObjInDir(dir, subPath);
    }

    return true;
}