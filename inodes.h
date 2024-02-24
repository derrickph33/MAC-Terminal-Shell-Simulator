#ifndef INODES_H
#define INODES_H
#include <stdint.h>

typedef struct{         //inode struct setup
        uint32_t node;
        uint32_t parentInode;
        char type;
        char name[32];
} Inode;

//variables
Inode inodeList[1024]; //array of inodes (each inode represents a file or directory)
size_t inodeCount = 0; //keep count of # of inodes
uint32_t currentInode = 0; //keep track of current inode


//function declarations here
void loadINodeList(const char *path);
void saveINodeList(const char *path);
void changeDirectory(const char *name);
void listContents();
void createDirectory(const char *name);
void createFile(const char *name);

#endif    
