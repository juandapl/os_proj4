#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#define BUFSIZE 16

int	main(int argc, char *argv[]){
	char buffer[BUFSIZE];
	// char *buffer=NULL;
	int  filedes;
	ssize_t nread;
	long total=0;

	if ((filedes=open(argv[1], O_RDONLY))== -1){
		printf("error in opening %s \n",argv[1]);
		exit(1);
		}
	
	while ( (nread=read(filedes,buffer,BUFSIZE)) > 0 )
		total += nread;
	
	printf("Total char in %s %ld \n",argv[1],total);
	exit(0);
}
