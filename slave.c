#include <stdio.h>

int main(int argc, char **argv){
	//sent from master
	//char *filename;
	int numiterations;
	
	puts(argv[0]);
	puts(argv[1]);
	puts(argv[2]);
	
	if(argc < 3){
		perror("Not enough arguments on command line");
		return 1;
	}
	//filename = argv[1];
	char *filename = argv[1];
	numiterations = atoi(argv[2]);
	
	puts(filename);
	
	int i;
	for(i = 0; i < numiterations; i++){
		puts("this is slave");
		//TODO: code for entering critical section
	}
}