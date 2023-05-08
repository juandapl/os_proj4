#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
	struct stat statbuf;

	if (stat(argv[1], &statbuf) == -1)
		perror("Failed to get file status");
	else {
		printf("Time/Date  : %s",ctime(&statbuf.st_atime));
		printf("---------------------------------\n");	
		printf("entity name: %s\n",argv[1]);
		printf("accessed   : %s", ctime(&statbuf.st_atime));
	  	printf("modified   : %s", ctime(&statbuf.st_mtime));
		}

	return(1);
}
