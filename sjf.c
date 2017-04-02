#include "sjf.h"

Queue *queue;

void sjf_loop() {
    queue_init(queue);

	for(;;) {
		network_wait();

		do {
			/* Process next in queue completely */
			Node *first = queue_dequeue(queue);
			if (!first) break;
			printf("Sending file of size %lu on connection %d\n", 
				first->rcb->bytes_unsent, first->rcb->client_connection);
			sjf_serve(first->rcb);
            
            /* Clean up request */
			node_destroy(first);
		} while (!queue_is_empty(queue));
		printf("Queue is empty! Sleeping now.\n");
	}
}

/**
 * Loops over a file, sending it in MAX_HTTP_SIZE chunks until the
 * client has received it in its entirety.
 */
void sjf_serve(RCB *rcb) {
	int len;
	char *buffer = http_create_buffer(MAX_HTTP_SIZE);

    /* Send chunks until complete */
	do {
		len = fread( buffer, 1, MAX_HTTP_SIZE, rcb->requested_file );
		if( len < 0 ) {
			perror( "Error while writing to client" );
		} else if( len > 0 ) {
			len = write( rcb->client_connection, buffer, len );
			if( len < 1 ) {
				perror( "Error while writing to client" );
			}
		}
	} while(len == MAX_HTTP_SIZE);
	free(buffer);
}

/**
 * At the beginning of every cycle, add any new requests to the
 * queue in order of filesize (ascending).
 */
void sjf_enqueue(RCB *rcb) {
    queue_enqueue_priority(top_queue, rcb);
}

void sjf_dequeue() {

}