#ifndef __ENTRY__
#define __ENTRY__

struct entry{
    int pid;
    char phash[6];
    int dirty;
};

typedef struct entry *entryPtr;

void set_pid(int id,entryPtr e);

int get_pid(struct entry e);

int cmp_phash(char ph[],struct entry e);

void set_phash(char ph[],entryPtr e);

void set_dirty(int d,entryPtr e);

int get_dirty(struct entry e);

#endif
