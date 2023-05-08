#include  <stdio.h>
#include  <stdlib.h>
#include  <fcntl.h>
#include  <unistd.h>
#include  <sys/stat.h>

int main(){
  int fd1, fd2, fd3;
  mode_t fdmode = S_IRUSR|S_IWUSR|S_IRGRP| S_IROTH;

  if ( ( fd1=open("dupdup2file", O_WRONLY | O_CREAT | O_TRUNC, fdmode ) ) == -1 ){
	perror("open");
	exit(1);
  }
  printf("fd1 = %d\n", fd1);
  write(fd1, "What ", 5);
  fd2=dup(fd1);
  printf("fd2 = %d\n", fd2);
  write(fd2, "time", 4);
  close(0);

  fd3=dup(fd1);
  printf("fd3 = %d\n", fd3);
  write(fd3, " is it", 6);
  close(2);

  dup2(fd2, 2);
  write(2,"?\n",2);
  close(fd1); 
  close(fd2);
  close(fd3);
  return 1;
}
