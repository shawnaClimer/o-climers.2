#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

int main(int argc, char **argv){
	
	
	//getopt
	extern char *optarg;
	extern int optind;
	int c, err = 0;
	int hflag=0, sflag=0, lflag=0, iflag=0,tflag=0;
	static char usage[] = "usage: %s -h \n-s x \n-l filename \n-i y \n-t z\n";
	
	char *filename, *x, *y, *z;
	
	while((c = getopt(argc, argv, "hs:l:i:t:")) != -1)
		switch (c) {
			case 'h':
				hflag = 1;
				break;
			case 's':
				sflag = 1;
				x = optarg;//max number of slave processes
				break;
			case 'l':
				lflag = 1;
				filename = optarg;//log file 
				break;
			case 'i':
				iflag = 1;
				y = optarg;//number of increments per slave
				break;
			case 't':
				tflag = 1;
				z = optarg;//time until master terminates
				break;
			case '?':
				err = 1;
				break;
		}
		
	if(err){
		fprintf(stderr, usage, argv[0]);
		exit(1);
	}
	//help
	if(hflag){
		puts("-h for help\n-l to name log file\n-s for number of slaves\n-i for number of increments per slave\n-t time for master termination\n");
	}
	//set default filename for log
	if(lflag == 0){
		filename = "logfile.txt";
	}
	//puts(filename);
	//number of slaves
	int numSlaves=5;
	if(sflag){//change numSlaves
		numSlaves = atoi(x);
	}
	//puts(x);
	//number of increments per slave
	int numIncrements=3;
	if(iflag){//change numIncrements
		numIncrements = atoi(y);
	}
	//puts(y);
	//time in seconds for master to terminate
	int endTime=20;
	if(tflag){//change endTime
		endTime = atoi(z);
	}
	//puts(z);
	
	
}