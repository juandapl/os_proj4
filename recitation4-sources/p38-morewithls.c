#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char *modes[]={"---","--x","-w-","-wx","r--","r-x","rw-","rwx"}; 
              /* eight distinct modes */

void list(char *);
void printout(char *);

int	main(int argc, char *argv[]){
struct stat mybuf;

if (argc<2) { list("."); exit(0);}

while(--argc){
  if (stat(*++argv, &mybuf) < 0) { 
	perror(*argv); continue;
	}

  if ((mybuf.st_mode & S_IFMT) == S_IFDIR )
    	list(*argv);      /* directory encountered */
  else 	printout(*argv);  /* file encountered      */
  }
}


void list(char *name){
DIR 	*dp;
struct 	dirent *dir;
char 	*newname;

	if ((dp=opendir(name))== NULL ) {
		perror("opendir"); return;
		}
	while ((dir = readdir(dp)) != NULL ) {
  		if (dir->d_ino == 0 ) continue;
  		newname=(char *)malloc(strlen(name)+strlen(dir->d_name)+2);
  		strcpy(newname,name);
  		strcat(newname,"/");
  		strcat(newname,dir->d_name);
  		printout(newname);
  		free(newname); newname=NULL;
  		}
	closedir(dp);
}

void printout(char *name){
struct stat 	mybuf;
char 		type, perms[10];
int 		i,j;

	stat(name, &mybuf);
	switch (mybuf.st_mode & S_IFMT){
  	case S_IFREG: type = '-'; break;
  	case S_IFDIR: type = 'd'; break;
  	default:      type = '?'; break;
  	}

	*perms='\0';

	for(i=2; i>=0; i--){
   		j = (mybuf.st_mode >> (i*3)) & 07;
   		strcat(perms,modes[j]); 
		}

	printf("%c%s%3d %5d/%-5d %7d %.12s %s \n", \
                type, perms, (int)mybuf.st_nlink, mybuf.st_uid, \
                mybuf.st_gid, (int)mybuf.st_size, \
                ctime(&mybuf.st_mtime)+4, name); /* try without 4 */
}
