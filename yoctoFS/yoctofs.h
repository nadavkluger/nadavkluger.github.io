#ifndef YOCTOFS_H_
#define YOCTOFS_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LEN 20

typedef struct FileSystemObject {
    char name[MAX_NAME_LEN];
    bool isDir;
    struct FileSystemObject* next;
    struct FileSystemObject* parent;
    void* content;
} fsObj;

void printPath();
bool findFsObj(char *path, fsObj **out);
void listDir(char *path);
void makeFsObj(char *path, bool isDir);
void removePath(char *path);

#endif