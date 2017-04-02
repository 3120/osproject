#include "sjf.h"

/**
 * Loops over a file, sending it in MAX_HTTP_SIZE chunks until the
 * client has received it in its entirety.
 */
RCB* sjf_serve(RCB *rcb) {
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
    rcb_destroy(rcb);
    sem_post(&permission_to_queue);
    return NULL;
}

/**
 * At the beginning of every cycle, add any new requests to the
 * queue in order of filesize (ascending).
 */
void sjf_enqueue(RCB *rcb) {
    if (!rcb) return;

    pthread_mutex_lock(&top_lock);
        queue_enqueue_priority(top_queue, rcb);
    pthread_mutex_unlock(&top_lock);
}

/**
 * Grab the RCB at the front of the only queue.
 */
RCB* sjf_dequeue() {
    pthread_mutex_lock(&top_lock);
        RCB *ready_to_serve = queue_dequeue(top_queue);
    pthread_mutex_unlock(&top_lock);

    return ready_to_serve;
}