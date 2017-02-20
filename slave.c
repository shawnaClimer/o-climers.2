#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h> //used for sleep
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#define PERM (S_IRUSR | S_IWUSR)

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
	
	//string for timestamp
	puts("allocating char * for timestamp");
	char *tmstring = (char *)malloc(sizeof(long) + 1);
	puts("timestring allocated");
	puts("allocating char * for sharedNum");
	//string for sharedNum
	char *sharedstring = (char *)malloc(sizeof(int) + 1);
	puts("shared allocated");
	
	int i;
	for(i = 0; i < numiterations; i++){
		puts("this is slave");
		
		//TODO: code for entering critical section
		puts("calling sleep");
		sleep(rand() % 3);//sleeps for 0-2 seconds
		
		//shared memory
		key_t key;
		int shmid;
		int *shared;
		int *here;
		void *shmaddr = NULL;
		int mode;
		if((key = ftok("master.c", 7)) == -1){
			perror("key error");
			return 1;
		} 
		//get the shared memory
		if((shmid = shmget(key, sizeof(int), IPC_CREAT | 0666)) == -1){
			perror("failed to create shared memory");
			return 1;
		}
		//attach to shared memory
		if((shared = (int *)shmat(shmid, shmaddr, 0)) == (void *)-1){
			perror("failed to attach");
			if(shmctl(shmid, IPC_RMID, NULL) == -1){
				perror("failed to remove memory seg");
			}
			return 1;
		}
		here = shared;
		//*here = 0;
		if(*here > 4){
			puts("found shared memory");
		}
		
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
		sprintf(sharedstring, "%d", *here);
		strcat(message, sharedstring);
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
		//detach from shared memory
		if(shmdt(shared) == -1){
			perror("failed to detach from shared memory");
			return 1;
		}
		sleep(rand() % 3);
		//TODO: code for exiting critical section
		
	}
}