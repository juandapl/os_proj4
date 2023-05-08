#include <stdio.h>	// to have access to printf()
#include <stdlib.h>	// to enable exit calls
#include <fcntl.h>	// to have access to flags def
#define PERMS 0644	// access permission for the 3 groups of users

char *workfile="mytest";

int 	main(){
	int filedes;
	
	if ( (filedes=open(workfile, O_CREAT|O_RDWR, PERMS)) == -1){
		perror("creating");
		exit(1);
		}
	else 	{ 
		printf("Managed to get to the file successfully\n"); 
		}

	exit(0);
}
