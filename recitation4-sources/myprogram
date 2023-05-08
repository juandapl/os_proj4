#include  <stdio.h>
#include  <stdlib.h> 
#include  <fcntl.h>
#include  <unistd.h>
#include  <sys/stat.h>

int main(){
  int fd, bytes, bytes1, bytes2;
  char buf[50];

  mode_t fdmode = S_IRUSR|S_IWUSR;

  if ( ( fd=open("t", O_WRONLY | O_CREAT, fdmode ) ) == -1 ){
	perror("open");
	exit(1);
  	}

  bytes1 = write(fd, "First write. ", 13);
  printf("%d bytes were written. \n", bytes1);
  close(fd); 

  if ( (fd=open("t", O_WRONLY | O_APPEND)) == -1 ){
	 perror("open");
        exit(1);
  	}

  bytes2 = write(fd, "Second Write. \n", 14);
  printf("%d bytes were written. \n", bytes2);
  close(fd);

  if ( (fd=open("t", O_RDONLY)) == -1 ){
	perror("open");
        exit(1);
	}

  bytes=read(fd, buf, bytes1+bytes2);
  printf("%d bytes were read \n",bytes);
  close(fd);

  buf[bytes]='\0';
  printf("%s\n",buf);
  return(1);
}
