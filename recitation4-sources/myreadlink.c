#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


main(int argc,char *argv[]){
struct stat mybuf;


char buf[1024];
ssize_t len;

if ((len = readlink("/home/ad/Desktop/SysPro11/Set004/src-set004/myprogram", buf, sizeof(buf)-1)) != -1)
    buf[len] = '\0';

	printf("%s\n", buf);

}


