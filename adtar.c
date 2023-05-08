// adtar.c: abu dhabi / alex delis tar
// by Nicholas Raffone and Juan Piñeros

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

char result[1024];

void specifyDirName(const char* toSanitize){
    int cutOff = 0;
    for(int i = 0; i<strlen(toSanitize); i++){
        if(toSanitize[i]=='/'){
            cutOff = i+1;
        }
    }
    if(cutOff == 0){
        strcpy(result, toSanitize);
        return;
    }
    strncpy(result, toSanitize + cutOff, strlen(toSanitize));
}

void printHierarchy(char* fname, char* curr_path){
    specifyDirName(fname);
    DIR *dr = opendir(fname);
    if(dr == NULL){
        FILE* isFile = fopen(fname, "r");
        if(isFile == NULL){
            printf("file not valid: %s/%s\n", curr_path,fname);
            return;
        }
        struct stat file_stat;  
        lstat (fname, &file_stat);
        // printf("inode: %llu\n", file_stat.st_ino);
        printf("%s ", result);
    }else{
        printf("(D:%s ", result);
        chdir(fname);
        char new_path[1024];
        strcpy(new_path, curr_path);
        strcat(new_path, "/");
        strcat(new_path, fname);
        struct dirent *de;
        //traverse and close
        while ((de = readdir(dr)) != NULL){
            if(strcmp(".", de->d_name) !=0 && strcmp("..", de->d_name)!=0 ){
                printHierarchy(de->d_name, new_path);
            }
        }
        closedir(dr);
        printf("end dir) ");
        chdir("..");
    }
}

int main(int argc, char** argv){
    // dir root( dir 1, 2, 3 dir, 5, 6, end dir,  4, end dir, 5) end dit
    printf("(D:root ");
    for(int i = 1; i<argc; i++){
        printHierarchy(argv[i], ".");
    }
    printf("end dir)\n");
}
