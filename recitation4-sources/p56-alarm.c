#include <stdio.h>
#include <unistd.h>

void	main(){
	alarm(3); // schedule an alarm signal
	printf("Looping for good!\n");
	fflush(stdout);
	while (1) ;
	printf("This line should be never part of the output\n");
	fflush(stdout);
	}
