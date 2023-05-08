#include <stdio.h>
#include <signal.h>
#include <unistd.h>


void  wakeup(int);

void main(){
  	printf("about to sleep for 5 seconds \n");
  	signal(SIGALRM, wakeup);

  	alarm(5);
  	pause();  /* pauses the process until a sig arrives */
  	printf("Hola Amigo! Un abrazo!\n");
}

void wakeup(int signum){
  	signal(SIGALRM, wakeup);
  	printf("Alarm received from kernel\n");
}
