#include <stdio.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define SHMKEY 5217

union semun{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

int empty1,full1;//sem for in-ds
int empty2,full2;//sem for out-ds
int sem;//sem for pmc/pidmatch

typedef struct{
        pid_t pid;
        char text[120];
}message;

typedef struct{
	message in_ds;
	message out_ds;
	int pmc;//producer's message counter
	int PidMatch;
}shared_data;

int main(int argc,char* argv[]){
	if(argc != 3){
		printf("Wrong arguments!\n");
		return -1;
	}
	int N=atoi(argv[1]);//first argument for N processes
	int K=atoi(argv[2]);//second for K repetitions
	shared_data *shmem;
	int mes_counter=0,rem_P=N;
	int pid_match=0,match;
	int shmid;
	int i,j,pid;
	int rnum;
	char textline[160];
	union semun arg;
	message m;
	struct sembuf down={0,-1,0};
	struct sembuf up={0,1,0};
	FILE *fp;
	int txtlen=0;
//**************************************SHARED MEMORY******************************************
	shmid=shmget(SHMKEY,(sizeof(shared_data)),IPC_CREAT|0666);
	if(shmid==-1){perror("SHMGET ERROR:");return -1;}//ERROR
	shmem=(shared_data *)shmat(shmid,(char*)0,0);
	if ((int)shmem == -1){perror("SHMAT ERROR");return -1;}//ERROR
	shmem->pmc=0;
	shmem->PidMatch=0;
//**************************************SEMAPHORES*********************************************
	empty1=semget(1796,1,IPC_CREAT|0660);
	if(empty1==-1){perror("SEMGET:");return -1;}//ERROR
	arg.val=1;
	semctl(empty1,0,SETVAL,arg);//initialize with 1/in-ds now is empty
	full1=semget(1797,1,IPC_CREAT|0660);
        if(full1==-1){perror("SEMGET:");return -1;}//ERROR
        arg.val=0;
        semctl(full1,0,SETVAL,arg);//initialize with 0/in-ds isn't full
	full2=semget(1798,1,IPC_CREAT|0660);
        if(full2==-1){perror("SEMGET:");return -1;}//ERROR
        arg.val=0;
        semctl(full2,0,SETVAL,arg);//initialize with 0/out-ds isn't full
	empty2=semget(1799,1,IPC_CREAT|0660);
        if(empty2==-1){perror("SEMGET:");return -1;}//ERROR
	arg.val=1;
	semctl(empty2,0,SETVAL,arg);//initialize with 1/out-ds is empty
	sem=semget(1800,1,IPC_CREAT|0660);
        if(sem==-1){perror("SEMGET:");return -1;}//ERROR
        arg.val=1;
        semctl(sem,0,SETVAL,arg);//initialize mut5 with 1

//**********************************************************************************************
	for(i=0;i<N;i++){//creating the N producers...
		pid=fork();
		if(pid == -1){perror("FORK ERROR:");return -1;}//ERROR
		else if(pid==0){
        		fp=fopen("randomtext.txt","r");
			if(fp==NULL){perror("FOPEN ERROR:");return -1;}//ERROR
		        fseek(fp,0,SEEK_END);
		        txtlen=ftell(fp);
			srand(time(NULL));
			break ;//child is ready,break the loop...
		}
	}
//here starts the C-P communication
	while(rem_P != 0){//and stops when all producers exit
		switch(pid){
			case 0://Producer's code here...
				//****making the message******
				rnum=rand()%txtlen;//take a random number in range [0,txtlen-1]
				if(txtlen>120)rnum=(rnum*getpid())%(txtlen-120);//if we are sure there are more than 1 line get a r_num sure different from other p
				rnum=abs(rnum);
				if(txtlen>120){//if text has more than 120 chars skip current line
					fseek(fp,rnum,SEEK_SET);
					while(fgetc(fp)!='\n') continue;//skip line
				}
				j=0;
				while((textline[j++]=fgetc(fp))!='\n') continue;
				textline[--j]='\0';//change '\n' with '\0'
				fseek(fp,0,SEEK_SET);
				m.pid=getpid();
				strcpy(m.text,textline);
				//*******message ready*********
				semop(empty1,&down,1);//down empty1---when in-ds is empty put the message
				shmem->in_ds=m;//in-ds now has the message
				semop(full1,&up,1);//up full1---now c can take the message from in-ds

				semop(full2,&down,1);//down full2---if out-ds has a message get it when you can
				m=shmem->out_ds;
				semop(empty2,&up,1);//up empty2---out-ds is empty again so c can put here the message

				semop(sem,&down,1);
				if(m.pid==-5){//EXIT CODE
					semop(sem,&up,1);//we dont need the shm UP the sem
					shmdt(shmem);//detach
					printf("P %d exits with %d pid_match.\n",getpid(),pid_match);
					fclose(fp);//close text
					exit(0);
				}
				else if(m.pid==getpid()){
					shmem->pmc++;//we have a message
					if(shmem->pmc > K){//dont print it if u have print K messages
						semop(sem,&up,1);
						semop(full2,&up,1);
						break;
					}
					shmem->PidMatch++;
					pid_match++;
					printf("P:%d ,text: %s\n",m.pid,m.text);
				}
				else{
					shmem->pmc++;
					if(shmem->pmc > K){
						semop(sem,&up,1);
						semop(full2,&up,1);
						break;
					}
					printf("P:%d , %d ,text: %s\n",getpid(),m.pid,m.text);
				}
				semop(sem,&up,1);
				break;

			default://Consumer's code here...
				if(mes_counter != K){//inside this make K repetitions
					semop(full1,&down,1);//down full1---when in-ds has a messsage take it
					m=shmem->in_ds;
					semop(empty1,&up,1);//up empty1---in-ds is empty,p can put a messege there
					//change the message
					int ccounter=0;
					while(m.text[ccounter]!='\0'){
						if(m.text[ccounter]>='a' && m.text[ccounter]<='z')
							m.text[ccounter]-='a'-'A';
						ccounter++;
					}
					semop(empty2,&down,1);//down empty2---if out-ds is empty put message
					shmem->out_ds=m;
					semop(full2,&up,1);//up full2---out-ds has a message now,so p can get it
					mes_counter++;
				}
				else{//after these repetitions send exit messages to children
//we have stop taking messages from in-ds but we have to 'UP' the empty1 semaphore everytime and let
//the stuck process pass that point to take the exit message.
					semop(empty1,&up,1);
//then we have to create that exit message
					m.pid=-5;//exit value
					strcpy(m.text,"Goodbye!");//just a message
//now we send the message to out-ds but we have to wait for access
					semop(empty2,&down,1);
					shmem->out_ds=m;
					semop(full2,&up,1);
//wait for child to exit
					wait(&match);
					rem_P--;//one process terminated
				}
		}
	}
//all children processes terminated...
//let's print the results!
	printf("\nProcess number:%d ( %d producers , 1 consumer )\n",N+1,N);
	printf("Steps number/Repetitions : %d\n",K);
	printf("Total pid_match:%d\n",shmem->PidMatch);
	shmdt(shmem);//detach...
//delete shm and sems...
	shmctl(shmid,IPC_RMID,(struct shmid_ds *)0);
	semctl(empty1,0,IPC_RMID,0);
        semctl(full1,0,IPC_RMID,0);
        semctl(full2,0,IPC_RMID,0);
        semctl(empty2,0,IPC_RMID,0);
	semctl(sem,0,IPC_RMID,0);
	return 0;
}
