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


int main(int argc, char **argv){
	//sent from master
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
		
		//shared memory
		key_t key;
		int shmid;
		int *shared;
		int *here;
		void *shmaddr = NULL;
		
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
		int sharedNum;
		
		//allocate shared memory for bakery algorithm
		key_t bakerykey;
		int bakeid;
		int *bakeshare;
		int *bakenum;
		if((bakerykey = ftok("master.c", 8)) == -1){
			perror("bakery key error");
			return 1;
		}
		if((bakeid = shmget(bakerykey, (sizeof(int) * 21), IPC_CREAT | 0666)) == -1){
			perror("failed to create bakery shared memory");
			return 1;
		}
		if((bakeshare = (int *)shmat(bakeid, NULL, 0)) == (void *)-1){
			perror("failed to attach bakeshare");
			if(shmctl(bakeid, IPC_RMID, NULL) == -1){
				perror("failed to remove bake mem seg");
			}
			return 1;
		}
		bakenum = bakeshare;
		
		//code for entering critical section
		*bakenum++;
		while(*bakenum == 1){} //if someone is choosing we wait
		*bakenum = 1;//set first spot to flag process is choosing
		int k;
		int max = 0;
		int temp;
		for(k = 1; k < 20; k++){
			temp = *bakenum++;
			if(temp > max){
				max = temp;
			}
		}
		bakenum = bakeshare;//return to beginning
		max++;
		for(k = 0; k < i + 2; k++){// ith spot comes after flag
			*bakenum++;
		}
		*bakenum = max;
		bakenum = bakeshare;//return to beginning
		*bakenum++ = 0;//set flag back to zero
		
		//do{
			//int n = sizeof(number[]);
		/* choosing[i] = 1;
		int k = 0;
		int max = 0;
		while(number[k] != NULL){
			if(number[k] > max){
				max = number[k];
			}
			k++;
		}
		//number[i] = 1 + max(number[0], ..., number[n-1]);
		number[i] = 1 + max;
		choosing[i] = 0;
		int j;
		for(j = 0; j < k; j++){
			while(choosing[j]);
			while((number[j] !=0) && (number[j] < number[i] || (number[j] == number[i] && j < i)));
		} */
		//}
		puts("calling sleep");
		sleep(rand() % 3);//sleeps for 0-2 seconds
		*here = *here + 1;
		sharedNum = *here;
		
		//build message for output file		
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
		
		//update with shared memory
		strcat(message, " with sharedNum = ");
		sprintf(sharedstring, "%d", sharedNum);
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
		int c = 0;
		while(message[c] != '\0'){
			ch = message[c];
			c++;
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
		//number[i] = 0;
		
	}
}