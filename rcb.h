#ifndef RCB_H
#define RCB_H

#include <stdio.h>
#include <stdbool.h>

typedef struct {
	int client_connection;
	FILE *requested_file;
	unsigned long bytes_unsent;
} RCB;

RCB* rcb_init(int client_connection, FILE *file);
unsigned long rcb_get_filesize(FILE *file);
void rcb_update_record(RCB *rcb, int sent);
void rcb_destroy(RCB *rcb);
bool rcb_completed(RCB *rcb);

#endif