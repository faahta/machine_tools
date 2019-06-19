#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<time.h>
#include<errno.h>
#include<signal.h>
#include<sys/wait.h> 


int k;
int count=0, tot_duration=0;
float average;
int fd[2];
char start[10], stop[10];

typedef struct machines{
	int pid;
	int duration;
}machines_t;
machines_t *machines;


void init(){
	machines = (machines_t *)malloc(k * sizeof(machines_t));
	pipe(fd);
	sprintf(start, "%s", "start");
	sprintf(stop, "%s", "stop");
}

void handler(int signo){
	signal(SIGUSR1, handler);
	//printf("my DADDY is tryna kill me, but i am survivor\n");
	return;
}

int 
main(int argc, char *argv[]){

	if(argc!=2){
		printf("usage: %s k\n",argv[0]);
		exit(1);
	}
	k = atoi(argv[1]);
	machines_t m;
	int i;
	int mtp, mp;
	mp = fork();
	if(mp == 0){
		while(1){
			char c;
			signal(SIGUSR1, handler);
			pause();
			sleep(3);
			if(errno == EINTR){
				printf("read interrupted \n");
				close(fd[1]);
				read(fd[0], &m, sizeof(m));
				close(fd[0]);
				
				printf("Do you want to see average duration:"); fflush(stdin);
				c = getchar();
				if(c == 'y'){
					printf("calculating...%d\n",m.duration);
					count ++;
					tot_duration += m.duration;
					average = tot_duration / count;
					printf("average since previous request = %f\n", average);
				} else{
					//printf("invalid character\n");
					//break;	
				}	
			}
			
			//execl(argv[0], argv[0], start, NULL);
		}
	} else{
		for(i = 0;i < k; i++){
			mtp = fork();
			if(mtp != 0){
				/*Controller process*/
				close(fd[0]);
				write(fd[1], start, sizeof(start));
				close(fd[1]);
				
				wait(NULL);
				read(fd[0], &m, sizeof(m));
				close(fd[0]);
				sleep(15);
				
				close(fd[0]);
				write(fd[1], stop, sizeof(stop));
				close(fd[1]);
							
				kill(mp, SIGUSR1);
				
				//printf("LOOP %d Controller process sent signal to Monitor process\n", i);
				close(fd[0]);
				write(fd[1], &m, sizeof(m));
				close(fd[1]);
				
				//sleep(5);
				
				
			} else{
				/*Machine tool processes*/
				char command[10];
				
				close(fd[1]);
				read(fd[0], command, sizeof(command));
				//printf("Machine tool process %d: received start command\n", i);
				close(fd[0]);
				
				int r = ((rand() % 4));
				machines[i].pid = getpid();
				machines[i].duration = r;
				
				close(fd[0]);
				write(fd[1], &machines[i], sizeof(machines[i]));
				
				close(fd[1]);
				read(fd[0], &command, sizeof(stop));
				close(fd[0]);
			}
			//printf("\n");
		}	
	}
	
	
	return 0;
	
}


