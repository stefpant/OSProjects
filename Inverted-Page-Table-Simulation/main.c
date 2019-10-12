#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "entry.h"
#include "inverted_pt.h"


int main(int argc,char* argv[]){
	int k,q,max;
	if(argc==3 || argc==4){
		k=atoi(argv[1]);
		q=atoi(argv[2]);
		printf("Starting simulation with:\n\tk=%d\n\tq=%d\n",k,q);
	}
	else{
		printf("Wrong arguments.Simulation ended!\n");
		return -1;
	}
	max=1000000;
	if(argc==4){
		max=atoi(argv[3]);
		printf("\tmax references=%d\n",max);
		if(max>1000000) max=1000000;
	}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	FILE *fp[2];
	fp[0]=fopen("trace/bzip.trace","r");
	fp[1]=fopen("trace/gcc.trace","r");
	int ProcessID[2]={1234,4321};
	int maxPF[2]={0},maxWB[2]={0};
	int ReadCounter=0,WriteCounter=0;
	int CurrPF[2]={0};
	int CurrRefs[2]={0};
	char line[10];
	int flag[2]={0};
	char pline[6];
	char action;
	int maxFlushes[2]={0};
	int size=2*k;
	iptPtr ipt;
	ipt = ConstructIPT(size);
	init_ipt(ipt,size);

	int i,j,w;
	while(1){
		for(i=0;i<2;i++){
			for(j=0;j<q;j++){
				CurrRefs[i]++;
				if(CurrRefs[i]>max){
					flag[i]=1;
					break;
				}
				w=0;
				while((line[w++]=fgetc(fp[i]))!='\n') continue;//get a line from text
				line[--w]='\0';
				strncpy(pline,line,5);
				pline[5]='\0';
				action=line[9];
				if(action=='W') WriteCounter++;//for write on this page increase counter
				else ReadCounter++;//same for read

				if(!search_elem(ipt,size,ProcessID[i],pline)){//page not in IPT
					CurrPF[i]++;
					maxPF[i]++;
					if(CurrPF[i]>k){//for k+1 PF
						maxFlushes[i]++;
						maxWB[i]+=FlushWhenFull(ipt,size,ProcessID[i]);//WBs<--from FWF
						CurrPF[i]=1;
					}

					insert_elem(ipt,size,ProcessID[i],pline,action);//insert the page from HD
				}
				else{//page found in IPT
					check_if_modified(ipt,size,ProcessID[i],pline,action);
				}
			}
		}
		if(flag[0]&&flag[1]) break;//we reached the max references
	}
//print the results now
	printf("Results...\n");
	printf("Read References=%d\nWrite references=%d\n",ReadCounter,WriteCounter);
	printf("\tFor process 1 we have:\n");
	printf("\t\tTotal Page Faults=%d\n",maxPF[0]);
	printf("\t\tTotal Write Backs=%d\n",maxWB[0]);
	printf("\t\tTotal Flushes=%d\n",maxFlushes[0]);
	printf("\tFor process 2 we have:\n");
	printf("\t\tTotal Page Faults=%d\n",maxPF[1]);
	printf("\t\tTotal Write Backs=%d\n",maxWB[1]);
	printf("\t\tTotal Flushes=%d\n",maxFlushes[1]);
//```````````````````````````````````````````````

//``````````````````````````````````````````````````
	DestructIPT(ipt);
	fclose(fp[0]);
	fclose(fp[1]);
	return 0;
}
