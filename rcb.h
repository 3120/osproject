#ifndef RCB_H
#define RCB_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
	int client_connection;
    char *filename;
	FILE *requested_file;
	unsigned long bytes_unsent;
    unsigned int chunks_served;
} RCB;

RCB* rcb_init(int client_connection);
bool rcb_process(RCB *rcb);
unsigned long rcb_get_filesize(FILE *file);
void rcb_update_record(RCB *rcb, int sent);
bool rcb_completed(RCB *rcb);
void rcb_destroy(RCB *rcb);

#endif