/* 
 * File: mlfb.c
 * Author: Nicholas McDonald
 * Purpose: This file contains the server functions related to the multi-
 * 		level queue with feedback scheduling algorithm. 
 */
#include "mlfb.h"

void mlfb_loop() {
	for( ;; ) {

		do {
			/* Determine which queue to read from, to add to, and
			 * what size chunks to write in. */
			int chunk;
			LinkedList *current_queue = NULL, *next_queue = NULL;
			char *queue_name; 
			if (!list_is_empty(top_queue)) {
				current_queue = top_queue, next_queue = middle_queue;
				chunk = CHUNK_8KB, queue_name = "TOP";
			} else if (!list_is_empty(middle_queue)) {
				current_queue = middle_queue, next_queue = bottom_queue;
				chunk = CHUNK_64KB, queue_name = "MID";
			} else {
				current_queue = bottom_queue, next_queue = bottom_queue;
				chunk = CHUNK_64KB, queue_name = "BOT";
			}

			/* Send up to chunk bytes of the next-in-line file */
			node *next = NULL;
			next = list_pop(current_queue);
			printf("Sending file of size %lu from %s on connection %d\n", 
					next->rcb->bytes_unsent, queue_name, 
					next->rcb->client_connection);
			mlfb_serve(next->rcb, chunk);
			if (rcb_completed(next->rcb)) {
				node_destroy(next);
			} else {
				list_insert_end(next_queue, next->rcb);
				free(next);
			}
		} while (!list_is_empty(top_queue) || !list_is_empty(middle_queue)
				|| !list_is_empty(bottom_queue));
		printf("Queue is empty! Sleeping now.\n");
	}
}

/**
 * Sends a portion of the requested file, based on the queue the request
 * block currently resides in.
 */
void mlfb_serve(RCB *rcb, int chunk) {
	int len;
	char *buffer = http_create_buffer(chunk);

	len = fread( buffer, 1, chunk, rcb->requested_file );
	if( len < 0 ) {
		perror( "Error while writing to client" );
	} else if( len > 0 ) {
		len = write( rcb->client_connection, buffer, len );
		if( len < 1 ) {
			perror( "Error while writing to client" );
		} else {
			rcb_update_record(rcb, chunk);
		}
	}

	free(buffer);
}
