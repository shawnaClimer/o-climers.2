#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#define PERM (S_IRUSR | S_IWUSR)

//to use for control c termination signal handler
static volatile sig_atomic_t doneflag = 0;
static void setdoneflag(int signo){
	doneflag = 1;
}



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
		filename = "test.out";
	}
	//puts(filename);
	//number of slaves
	int numSlaves=5;
	if(sflag){//change numSlaves
		numSlaves = atoi(x);
	}
	//puts(x);
	//number of increments per slave
	char *numIncrements="3";
	if(iflag){//change numIncrements
		//numIncrements = atoi(y);
		numIncrements = y;
	}
	//puts(y);
	//time in seconds for master to terminate
	int endTime=20;
	if(tflag){//change endTime
		endTime = atoi(z);
	}
	//puts(z);
	
	//for control c termination signal handler
	struct sigaction act;
	act.sa_handler = setdoneflag;
	act.sa_flags = 0;
	if((sigemptyset(&act.sa_mask) == -1) || (sigaction(SIGINT, &act, NULL) == -1)){
		perror("Failed to set SIGINT handler");
		return 1;
	}
	
	//create start time
	struct timespec start, now;
	clockid_t clockid;//clockid for timer
	clockid = CLOCK_REALTIME;
	long starttime, nowtime;
	if(clock_gettime(clockid, &start) == 0){
		starttime = start.tv_sec;
	}
	
	//shared memory
	key_t key;
	int shmid;
	int *shared;
	void *shmaddr = NULL;
	int mode;
	/* if((key = ftok("master.c", 'R')) == -1){
		perror("key error");
		return 1;
	} */
	//get the shared memory
	if((shmid = shmget(IPC_PRIVATE, sizeof(int), PERM)) == -1){
		perror("failed to create shared memory");
		return 1;
	}
	//attach to shared memory
	if((shared = (int *)shmat(shmid, shmaddr, PERM)) == (void *)-1){
		perror("failed to attach");
		if(shmctl(shmid, IPC_RMID, NULL) == -1){
			perror("failed to remove memory seg");
		}
		return 1;
	}
	
	//for child process
	pid_t childpid;
	int i;//for iterating num of slaves
	char processnum[15];//for sending slave number
		
	for(i=0; i < numSlaves; i++){
		//get current time
		if(clock_gettime(clockid, &now) == 0){
			nowtime = now.tv_sec;
		}
		//check for signal and timer
		if(!doneflag && ((nowtime - starttime) < endTime)){
			childpid = fork();
			if(childpid == -1){
				perror("Failed to fork");
				return 1;
			}
			if(childpid == 0){
				processnum[0] = '\0';//clear out old data
				sprintf(processnum, "%d", (i + 1));
				execl("slave", "slave", filename, numIncrements, processnum, NULL);
				perror("Child failed to exec slave");
				return 1;
			}
			if(childpid != wait(NULL)){
				perror("Parent failed to wait due to signal or error");
				return 1;
			}
		}else{
			//TODO code for killing child processes
			
			if(doneflag){
				puts("control c termination");
			}else{
				puts("timer ran out");
			}
			break;
		}
	}
	
	//TODO code for freeing shared memory
	if(shmdt(shared) == -1){
		perror("failed to detach from shared memory");
		return 1;
	}
	if(shmctl(shmid, IPC_RMID, NULL) == -1){
		perror("failed to delete shared memory");
		return 1;
	}
	
	return 0;
}