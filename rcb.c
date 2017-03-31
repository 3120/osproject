/* 
 * File: rcb.c
 * Author: Nicholas McDonald
 * Purpose: This file contains the functionality associated with request
 * 		control blocks. These are low-level functions and should not be
 *		used in abstracted code segments.
 */
#include "rcb.h"
#include <unistd.h>
#include <stdlib.h>

RCB* rcb_init(int client_connection, FILE *file) {
	if (!client_connection) return NULL;

	RCB *rcb = malloc(sizeof(RCB));
	if (!rcb) {
		perror("Error while allocating memory");
		abort();
	}

	rcb->client_connection = client_connection;
	rcb->requested_file = file;
	rcb->bytes_unsent = rcb_get_filesize(file);

	return rcb;
}

/* Note: The code used in this function is taken from the top response at
 * stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c . */
unsigned long rcb_get_filesize(FILE *file) {
	fseek(file, 0, SEEK_END);
	unsigned long len = (unsigned long)ftell(file);
	rewind(file);
	return len;
}

void rcb_destroy(RCB *rcb) {
	close(rcb->client_connection);
	fclose(rcb->requested_file);
	free(rcb);
	rcb = NULL;
}

void rcb_update_record(RCB *rcb, int sent) {
	if (rcb->bytes_unsent < sent) {
		rcb->bytes_unsent = 0;
	} else {
		rcb->bytes_unsent -= sent;
	}
}

bool rcb_completed(RCB *rcb) {
	return rcb->bytes_unsent == 0;
}