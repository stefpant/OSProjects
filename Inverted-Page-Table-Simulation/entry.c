#include "entry.h"
#include <string.h>

void set_pid(int id,entryPtr e){
	e->pid=id;
	return ;
}

int get_pid(struct entry e){
    return e.pid;
}

int cmp_phash(char ph[],struct entry e){
    return !strcmp(e.phash,ph);
}

void set_phash(char ph[],entryPtr e){
    strcpy(e->phash,ph);
    return ;
}

void set_dirty(int d,entryPtr e){
    e->dirty=d;
    return ;
}

int get_dirty(struct entry e){
    return e.dirty;
}
