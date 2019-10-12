#ifndef __IPT__
#define __IPT__

typedef struct invertedpt *iptPtr;

iptPtr ConstructIPT(int size);

void DestructIPT(iptPtr ipt);

void init_ipt(iptPtr ipt,int size);

int search_elem(iptPtr ipt,int size,int id,char ph[]);

void insert_elem(iptPtr ipt,int size,int id,char ph[],char action);

int FlushWhenFull(iptPtr ipt,int size,int id);

void check_if_modified(iptPtr ipt,int size,int id,char ph[],char action);

#endif
