#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h> 
#include  <fcntl.h>
#include  <unistd.h>
#include  <sys/stat.h>
#define BUFFSIZE 1024

int main(int argc, char *argv[]){
  int n, from, to;
  char buf[BUFFSIZE];

  mode_t fdmode = S_IRUSR|S_IWUSR|S_IRGRP| S_IROTH;
 
  if (argc!=3) {
	write(2,"Usage: ", 7);
	write(2, argv[0], strlen(argv[0]));
	write(2," from-file to-file\n", 19);
	exit(1);
	}

  if ( ( from=open(argv[1], O_RDONLY)) < 0 ){
	perror("open");
	exit(1);
  }

  if ( (to=open(argv[2], O_WRONLY|O_CREAT|O_APPEND, fdmode)) < 0 ){
	perror("open");
        exit(1);
  }

  while ( (n=read(from, buf, sizeof(buf))) > 0 )
	write(to,buf,n);
  close(from);
  close(to);
  return 1;
}
