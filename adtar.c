// adtar.c: abu dhabi / alex delis tar
// by Nicholas Raffone and Juan Piñeros

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

typedef struct INodeArrayElem{
    unsigned long inodeval;
    char filepath[1024];
} INodeArrayElem;

typedef struct INodeArray{
    INodeArrayElem** INodeList;
    int arrSize;
} INodeArray;

INodeArray* createINodeArray(){
    INodeArray* arr = (INodeArray*) malloc(sizeof(INodeArray));
    arr->arrSize = 0;
    arr->INodeList = NULL;
    return arr;
}

void insertINodeArrayElem(INodeArray* arr, unsigned long inodeval, char* filepath){
    if(arr->arrSize == 0){
        arr->INodeList = (INodeArrayElem**) malloc(sizeof(INodeArrayElem*));
    }else{
        arr->INodeList = realloc(arr->INodeList, (arr->arrSize+1)*sizeof(INodeArrayElem*));
    }
    arr->INodeList[arr->arrSize] = (INodeArrayElem*) malloc(sizeof(INodeArrayElem));
    arr->INodeList[arr->arrSize]->inodeval = inodeval;
    strcpy(arr->INodeList[arr->arrSize]->filepath, filepath);
    arr->arrSize++;
}

int findINodeArrayElem(INodeArray* arr, unsigned long inodeval){
    for(int i = 0; i<arr->arrSize; i++){
        if(arr->INodeList[i]->inodeval == inodeval){
            return i;
        }
    }
    return -1;
}

void destroyINodeArray(INodeArray* arr){
    for(int i = 0; i<arr->arrSize; i++){
        free(arr->INodeList[i]);
    }
    free(arr->INodeList);
    free(arr);
}

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

void writeHierarchyToFile(FILE* toWrite, char* fname, INodeArray* arr){
    specifyDirName(fname);
    DIR *dr = opendir(fname);
    if(dr == NULL){
        FILE* isFile = fopen(fname, "r");
        if(isFile == NULL){
            printf("WRITE: file not valid: %s\n",fname);
            return;
        }
        struct stat file_stat;
        lstat (fname, &file_stat);
        int linkTo = findINodeArrayElem(arr, file_stat.st_ino);
        if(linkTo > -1){
            // its a link, denote as so
            char metadata[256];
            sprintf(
                metadata,
                "TYPE: LINK\nNAME: %s\nOWNER ID: %d\nOWNER GROUP ID: %d\nRIGHTS: %o\n",
                result,
                file_stat.st_uid,
                file_stat.st_gid,
                file_stat.st_mode&(S_IRWXU | S_IRWXG | S_IRWXO)
            );
            fprintf(toWrite, "%lu\n", strlen(metadata));

            //print metadata contents
            fprintf(toWrite, "%s", metadata);

            char linkToStr[256];
            sprintf(
                linkToStr,
                "LINK TO: %d\n",
                linkTo
            );
            fprintf(toWrite, "%lu\n", strlen(linkToStr));
            fprintf(toWrite, "%s", linkToStr);
            return;
        }
        insertINodeArrayElem(arr, file_stat.st_ino, "");
        
        // get metadata
        //https://stackoverflow.com/questions/14325706/how-to-get-octal-chmod-format-from-stat-in-c
        char metadata[256];
        sprintf(
            metadata,
            "TYPE: FILE\nNAME: %s\nOWNER ID: %d\nOWNER GROUP ID: %d\nRIGHTS: %o\n",
            result,
            file_stat.st_uid,
            file_stat.st_gid,
            file_stat.st_mode&(S_IRWXU | S_IRWXG | S_IRWXO)
        );

        // get metadata size
        fprintf(toWrite, "%lu\n", strlen(metadata));

        //print metadata contents
        fprintf(toWrite, "%s", metadata);

        // open file and get size
        int fileSize = file_stat.st_size;

        // write filesize
        fprintf(toWrite, "%d\n", fileSize);

        char* fileData = malloc(fileSize);

        fread(fileData, fileSize, 1, isFile);

        fprintf(toWrite, "%s\n", fileData);

        free(fileData);

        fclose(isFile);
    }else{

        struct stat file_stat;
        lstat (fname, &file_stat);
        
        char metadata[256];
        sprintf(
            metadata,
            "TYPE: DIRECTORY\nNAME: %s\nOWNER ID: %d\nOWNER GROUP ID: %d\nRIGHTS: %o\n",
            result,
            file_stat.st_uid,
            file_stat.st_gid,
            file_stat.st_mode&(S_IRWXU | S_IRWXG | S_IRWXO)
        );
        fprintf(toWrite, "%lu\n", strlen(metadata));

        //print metadata contents
        fprintf(toWrite, "%s", metadata);

        // traversal process
        chdir(fname);
        struct dirent *de;

        //traverse and close
        while ((de = readdir(dr)) != NULL){
            if(strcmp(".", de->d_name) !=0 && strcmp("..", de->d_name)!=0 ){
                writeHierarchyToFile(toWrite, de->d_name, arr);
            }
        }
        closedir(dr);

        char* closeCommand = "CMD: CLOSE\n";
        fprintf(toWrite, "%lu\n", strlen(closeCommand));

        fprintf(toWrite, "%s", closeCommand);
        chdir("..");
    }
}

void unpackFile(char* fileToUnpack){
    INodeArray* arr = createINodeArray();
    FILE* zippedFile = fopen(fileToUnpack, "r");
    unsigned long sizeToRead;
    while(
        fscanf(zippedFile, "%lu\n", &sizeToRead) == 1
    ){
        char* metadata = (char*) malloc(sizeToRead);

        fread(metadata, sizeToRead, 1, zippedFile);

        if(strncmp(metadata, "CMD:", 4) == 0){
            // command
            char command[256];
            sscanf(metadata, "CMD: %s\n", command);
            if(strncmp(command, "CLOSE", 5) == 0){
                chdir("..");
            }
        }else{
            // metadata
            char type[256];
            char name[256];
            int owner_id;
            int owner_gid;
            char perm_str[256];
            sscanf(
                metadata,
                "TYPE: %s\nNAME: %s\nOWNER ID: %d\nOWNER GROUP ID: %d\nRIGHTS: %s\n",
                type,
                name,
                &owner_id,
                &owner_gid,
                perm_str
            );
            int perm_int = strtol(perm_str, 0, 8);

            if(strncmp(type, "FILE", 4) == 0){
                char curr_dir[1024];
                getcwd(curr_dir, sizeof(curr_dir));
                strcat(curr_dir, "/");
                strcat(curr_dir, name);
                insertINodeArrayElem(arr, 0, curr_dir);
                // add to file
                FILE* newFile = fopen(name, "w");
                unsigned long fileSize;
                fscanf(zippedFile, "%lu\n", &fileSize);
                char* fileData = (char*) malloc(fileSize);
                fread(fileData, fileSize, 1, zippedFile);
                fwrite(fileData, 1, fileSize, newFile);
                free(fileData);
                fclose(newFile);
            }else if(strncmp(type, "DIRECTORY", 9) == 0){
                mkdir(name, perm_int);
                chdir(name);
            }else if(strncmp(type, "LINK", 4) == 0){
                int linkCmdSize;
                fscanf(zippedFile, "%d\n", &linkCmdSize);

                char linkToCommand[1024];
                fread(linkToCommand, linkCmdSize, 1, zippedFile);
                int linkTo;
                sscanf(linkToCommand, "LINK TO: %d\n", &linkTo);
                link(arr->INodeList[linkTo]->filepath, name);
            }
        }
        
        free(metadata);
    }
    fclose(zippedFile);
    destroyINodeArray(arr);
}

void printZipFileHierarchy(char* fileToUnpack){
    FILE* zippedFile = fopen(fileToUnpack, "r");
    unsigned long sizeToRead;
    int spacing = 0;
    while(
        fscanf(zippedFile, "%lu\n", &sizeToRead) == 1
    ){
        char* metadata = (char*) malloc(sizeToRead);

        fread(metadata, sizeToRead, 1, zippedFile);

        if(strncmp(metadata, "CMD:", 4) == 0){
            // command
            char command[256];
            sscanf(metadata, "CMD: %s\n", command);
            if(strncmp(command, "CLOSE", 5) == 0){
                spacing -= 4;
            }
        }else{
            // metadata
            char type[256];
            char name[256];
            int owner_id;
            int owner_gid;
            char perm_str[256];
            sscanf(
                metadata,
                "TYPE: %s\nNAME: %s\nOWNER ID: %d\nOWNER GROUP ID: %d\nRIGHTS: %s\n",
                type,
                name,
                &owner_id,
                &owner_gid,
                perm_str
            );
            int perm_int = strtol(perm_str, 0, 8);
            if(strncmp(type, "FILE", 4) == 0){
                unsigned long fileSize;
                fscanf(zippedFile, "%lu\n", &fileSize);
                char* fileData = (char*) malloc(fileSize);
                fread(fileData, fileSize, 1, zippedFile);
                free(fileData);
                for(int i = 0; i<spacing; i++){
                    printf(" ");
                }
                printf("%s\n", name);
            }else if(strncmp(type, "DIRECTORY", 9) == 0){
                for(int i = 0; i<spacing; i++){
                    printf(" ");
                }
                printf("%s/\n", name);
                spacing += 4;
            }
        }
        
        free(metadata);
    }
    fclose(zippedFile);
}


void printZipFileMetadata(char* fileToUnpack){
    INodeArray* arr = createINodeArray();
    FILE* zippedFile = fopen(fileToUnpack, "r");
    unsigned long sizeToRead;
    int spacing = 0;
    while(
        fscanf(zippedFile, "%lu\n", &sizeToRead) == 1
    ){
        char* metadata = (char*) malloc(sizeToRead);

        fread(metadata, sizeToRead, 1, zippedFile);

        if(strncmp(metadata, "CMD:", 4) == 0){
            // command
            char command[256];
            sscanf(metadata, "CMD: %s\n", command);
            if(strncmp(command, "CLOSE", 5) == 0){
                spacing -= 4;
            }
        }else{
            // metadata
            char type[256];
            char name[256];
            int owner_id;
            int owner_gid;
            char perm_str[256];
            sscanf(
                metadata,
                "TYPE: %s\nNAME: %s\nOWNER ID: %d\nOWNER GROUP ID: %d\nRIGHTS: %s\n",
                type,
                name,
                &owner_id,
                &owner_gid,
                perm_str
            );
            if(strncmp(type, "FILE", 4) == 0){
                unsigned long fileSize;
                fscanf(zippedFile, "%lu\n", &fileSize);
                char* fileData = (char*) malloc(fileSize);
                fread(fileData, fileSize, 1, zippedFile);
                free(fileData);
                for(int i = 0; i<spacing; i++){
                    printf(" ");
                }
                printf("%s (owner: %d gid: %d perms: %s)\n", name, owner_id, owner_gid, perm_str);
                insertINodeArrayElem(arr, 0, name);
            }else if(strncmp(type, "DIRECTORY", 9) == 0){
                for(int i = 0; i<spacing; i++){
                    printf(" ");
                }
                printf("%s/ (owner: %d gid: %d perms: %s)\n", name, owner_id, owner_gid, perm_str);
                spacing += 4;
            }else if(strncmp(type, "LINK", 4) == 0){
                int linkCmdSize;
                fscanf(zippedFile, "%d\n", &linkCmdSize);

                char linkToCommand[1024];
                fread(linkToCommand, linkCmdSize, 1, zippedFile);
                int linkTo;
                sscanf(linkToCommand, "LINK TO: %d\n", &linkTo);

                for(int i = 0; i<spacing; i++){
                    printf(" ");
                }
                printf(
                    "%s (owner: %d gid: %d perms: %s, linked to: %s)\n",
                    name,
                    owner_id,
                    owner_gid,
                    perm_str,
                    arr->INodeList[linkTo]->filepath
                );
            }
        }
        
        free(metadata);
    }
    fclose(zippedFile);
    destroyINodeArray(arr);
}

int main(int argc, char** argv){

    if(argc < 2){
        printf("Not enough arguments\n");
        exit(1);
    }

    char base_path[256];
    getcwd(base_path, 256);

    if(strncmp(argv[1], "-c", 2) == 0){
        // store
        if(argc < 3){
            printf("Not enough arguments\n");
            exit(1);
        }
        INodeArray* arr = createINodeArray();
        FILE* toWrite = fopen(argv[2], "w"); // if w, is write mode, if a then its append mode
        for(int i = 3; i<argc; i++){
            chdir(base_path);
            writeHierarchyToFile(toWrite, argv[i], arr);
        }
        fclose(toWrite);
        destroyINodeArray(arr);
    }else if(strncmp(argv[1], "-a", 2) == 0){
        // append
         if(argc < 3){
            printf("Not enough arguments\n");
            exit(1);
        }
        INodeArray* arr = createINodeArray();
        FILE* toWrite = fopen(argv[2], "a"); // if w, is write mode, if a then its append mode
        for(int i = 3; i<argc; i++){
            chdir(base_path);
            writeHierarchyToFile(toWrite, argv[i], arr);
        }
        destroyINodeArray(arr);
        fclose(toWrite);

    }else if(strncmp(argv[1], "-x", 2)==0){
        // extract
        unpackFile(argv[2]);

    }else if(strncmp(argv[1], "-m", 2)==0){
        // print metadata
        printZipFileMetadata(argv[2]);

    }else if(strncmp(argv[1], "-p", 2)==0){
        // print file structure
        printZipFileHierarchy(argv[2]);

    }else{
        printf("Invalid flag, use -c|-a|-x|-m|-p\n");
        exit(1);
    }
    return 0;

}
