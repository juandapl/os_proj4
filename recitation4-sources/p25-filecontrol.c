#include  <stdio.h>
#include  <stdlib.h> 
#include  <fcntl.h>
#include  <unistd.h>
#include  <stdlib.h>
#include  <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>

int filestatus(int filedes){
	int myfileflags;

	if ( (myfileflags = fcntl(filedes,F_GETFL)) == -1){
		printf("file status failure\n");
		return(-1);
		}

	printf("file descriptor: %d ",filedes);

	switch ( myfileflags & O_ACCMODE ){ //test against the open file flags
	case O_WRONLY:
		printf("write-only"); break;
	case O_RDWR:
		printf("read-write"); break;
	case O_RDONLY:
		printf("read-only"); break;
	default:
		printf("no such mode");
	}

	if ( myfileflags & O_APPEND) printf(" - append flag set");
	printf("\n"); 
	return(0);
}


int main(){
  int fd1, fd2, myflags;

  mode_t fdmode = S_IRUSR|S_IWUSR;

  if ( ( fd1=open("aa", O_RDONLY, fdmode ) ) == -1 ){
	perror("open");
	exit(1);
  	}
  else printf("Opened file \"aa\" for reading with fd %d\n",fd1); 

  if ( (fd2=open("bb", O_WRONLY )) == -1 ){
	 perror("open");
         exit(1);
  	 }
  else printf("Opened file \"bb\" for writing with fd %d\n",fd2);

  filestatus(fd1);	
  filestatus(fd2);	

  myflags = fcntl(fd2, F_SETFL, O_APPEND); 
  filestatus(fd2);

	
  close(fd1); 
  close(fd2);
  return(1);
}

