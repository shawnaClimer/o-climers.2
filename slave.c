#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h> //used for sleep
#include <stdlib.h>
#include <errno.h>
#define MILLION 1000000L

int main(int argc, char **argv){
	//sent from master
	//char *filename;
	int numiterations;
	
	puts(argv[0]);
	puts(argv[1]);
	puts(argv[2]);
	
	if(argc < 4){
		perror("Not enough arguments on command line");
		return 1;
	}
	//filename = argv[1];
	char *filename = argv[1];
	numiterations = atoi(argv[2]);
	char *processnum = argv[3];
	
	puts(filename);
	
	//message to be written to test.out
	char *message = (char *)malloc(100);
	
	struct timespec tpstamp;
	clockid_t clockid;//clockid for timestamp
	clockid = CLOCK_REALTIME;
	long timess, timens;
	//int tsize = sizeof(time);
	//tsize++;
	//string for timestamp
	puts("allocating char * for timestamp");
	char *tmstring = (char *)malloc(sizeof(long) + 1);
	puts("timestring allocated");
	puts("allocating char * for sharedNum");
	//string for sharedNum
	char *shared = (char *)malloc(sizeof(int) + 1);
	puts("shared allocated");
	
	int i;
	for(i = 0; i < numiterations; i++){
		puts("this is slave");
		
		//TODO: code for entering critical section
		puts("calling sleep");
		sleep(rand() % 3);//sleeps for 0-2 seconds
		
		message[0] = '\0';//clear previous message
		strcat(message, "File modified by process number ");
		strcat(message, processnum);
		strcat(message, " at time ");
		
		//get time and put in string
		tmstring[0] = '\0';//clear previous time
		if(clock_gettime(clockid, &tpstamp) == 0){
			timess = tpstamp.tv_sec;
			timens = tpstamp.tv_nsec;//time in ssnn
		}
		sprintf(tmstring, "%d", timess);//convert time to string
		strcat(message, tmstring);
		sprintf(tmstring, "%d", timens);
		strcat(message, tmstring);
		strcat(message, " with sharedNum = ");
		sprintf(shared, "%d", i);
		strcat(message, shared);
		strcat(message, "\n");
		
		//put in test.out
		FILE *logfile;
		logfile = fopen(filename, "a");
		if(logfile == NULL){
			perror("Log file failed to open");
			return -1;
		}
		char ch;
		int j = 0;
		while(message[j] != '\0'){
			ch = message[j];
			j++;
			if(fputc(ch, logfile) != ch){//error writing to file
				fclose(logfile);
				return -1;
			}
		}
		fclose(logfile);
		
		sleep(rand() % 3);
		//TODO: code for exiting critical section
		
	}
}