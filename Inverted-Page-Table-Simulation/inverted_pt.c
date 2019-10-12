#include <stdlib.h>
#include "inverted_pt.h"
#include "entry.h"

struct invertedpt{
	entryPtr elem;
};


iptPtr ConstructIPT(int size){
	iptPtr ipt;
	ipt = malloc(sizeof(struct invertedpt));
	ipt->elem=malloc(size*sizeof(struct entry));
	return ipt;
}

void DestructIPT(iptPtr ipt){
	free(ipt->elem);
	free(ipt);
	return ;
}

void init_ipt(iptPtr ipt,int size){
	int i;
	for(i=0;i<size;i++)
		set_pid(-1,&(ipt->elem[i]));
	return ;
}

int search_elem(iptPtr ipt,int size,int id,char ph[]){
	int found=0;
	int i;
	for(i=0;i<size;i++){
		if(cmp_phash(ph,ipt->elem[i])){
			found=1;
			break ;
		}
	}
	return found;
}

void insert_elem(iptPtr ipt,int size,int id,char ph[],char action){
	int i;
	for(i=0;i<size;i++){
		if(get_pid(ipt->elem[i])==-1){
			set_pid(id,&(ipt->elem[i]));
			set_phash(ph,&(ipt->elem[i]));
			if(action=='R') set_dirty(0,&(ipt->elem[i]));
			else set_dirty(1,&(ipt->elem[i]));
			break ;
		}
	}
	return ;
}

int FlushWhenFull(iptPtr ipt,int size,int id){
	int i;
	int wbcounter=0;
	for(i=0;i<size;i++){
		if(get_pid(ipt->elem[i])==id){
			if(get_dirty(ipt->elem[i])==1)
				wbcounter++;
			set_pid(-1,&(ipt->elem[i]));
		}
	}
	return wbcounter;
}

void check_if_modified(iptPtr ipt,int size,int id,char ph[],char action){
	int i;
	if(action=='W')
		for(i=0;i<size;i++)
			if(get_pid(ipt->elem[i])==id)
				if(cmp_phash(ph,ipt->elem[i]))
					set_dirty(1,&(ipt->elem[i]));
	return ;
}
