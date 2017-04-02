#include "rr.h"

Queue *requests;

void rr_loop() {
			Node *next = queue_dequeue(requests);
			printf("Sending file of size %lu\n", next->rcb->bytes_unsent);
			rr_serve(next->rcb);
            
            /* Requeue the request, or remove it if complete */
            rr_finish_cycle(next);
		printf("Queue is empty! Sleeping now.\n");
	}
}


/**
 * Round robin processes requests by simply adding them to the back
 * of the queue.
 */
void rr_enqueue(RCB *request) {
	queue_enqueue(top_queue, request);
}


/**
 * Send a block of a requested file to the client.
 */
void rr_serve(RCB *rcb) {
	int len;
	char *buffer = http_create_buffer(MAX_HTTP_SIZE);

    /* Read up to the maximum size into the write buffer. */
	len = fread( buffer, 1, MAX_HTTP_SIZE, rcb->requested_file );
	if( len < 0 ) {
		perror( "Error while writing to client" );
	} else if( len > 0 ) {
        /* Transmit the buffer contents to the client */
		len = write( rcb->client_connection, buffer, len );
		if( len < 1 ) {
			perror( "Error while writing to client" );
		} else {
            /* Reflect the transmission in the RCB */
			rcb_update_record(rcb, MAX_HTTP_SIZE);
		}
	}

    /* Clear the buffer for the next transfer. */
	free(buffer);
}


/**
 * After every attempt to send, we check if there's still work
 * left for the just-processed job
 */
void rr_finish_cycle(Node *processed) {
    if (rcb_completed(processed->rcb)) {
        node_destroy(processed);
    } else {
        queue_enqueue(requests, processed->rcb);
        free(processed);
    }
}

void rr_dequeue() {

}