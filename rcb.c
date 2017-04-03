/* 
 * File: rcb.c
 * Author: Nicholas McDonald
 * Purpose: This file contains the functionality associated with request
 * 		control blocks. These are low-level functions and should not be
 *		used in abstracted code segments.
 */
#include "rcb.h"
#include "http.h"

/**
 * Create a Request Control Block to associate with an incoming request.
 * Note that a semaphore should be held for every call to rcb_init.
 *
 * @param client_connection File descriptor for inbound connection
 * @return An initialized Request Control Block
 */
RCB* rcb_init(int client_connection) {
    if (!client_connection || client_connection < 0) return NULL;

    RCB *rcb = malloc(sizeof(RCB));
    if (!rcb) {
        perror("Error while allocating memory");
        abort();
    }

    rcb->client_connection = client_connection;
    rcb->filename = NULL;
    rcb->requested_file = NULL;
    rcb->chunks_served = 0;
    return rcb;
}


/**
 * Attempt to process an incoming request. On success, the RCB is eligible for
 * the scheduling queue; on failure, the RCB should be destroyed and its
 * semaphore freed.
 *
 * @param rcb   The Request Control Block to be processed
 * @return True if processing was a success
 */
bool rcb_process(RCB *rcb) {
    /* Create read buffer */
    char *buffer = http_create_buffer(MAX_HTTP_SIZE);
    http_read_request(rcb->client_connection, buffer);
    char *request = http_parse_request(buffer);

    /* If request was malformed, respond with "Bad Request" */
    if (!request) {
        http_respond(400, rcb->client_connection, buffer);
        return false;
    } else {
        /* Open file, respond with "File Not Found" if impossible */
        FILE *requested_file = fopen(request, "r");
        if (!requested_file) {
            http_respond(404, rcb->client_connection, buffer);
            return false;
        } else {
            /* Assign the file descriptor to the request and respond "Ok" */
            rcb->filename = malloc(sizeof(request));
            strcpy(rcb->filename, request);
            rcb->requested_file = requested_file;
            rcb->bytes_unsent = rcb_get_filesize(requested_file);
            http_respond(200, rcb->client_connection, buffer);
        }
    }

    free(buffer);
    return true;
}

/* Note: The code used in this function is taken from the top response at
 * stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c . */
unsigned long rcb_get_filesize(FILE *file) {
	fseek(file, 0, SEEK_END);
	unsigned long len = (unsigned long)ftell(file);
	rewind(file);
	return len;
}

/**
 * Update the number of bytes remaining to be sent before the request is complete
 *
 * @param rcb
 * @param sent  Amount of data sent in the previous transmission
 */
void rcb_update_record(RCB *rcb, int sent) {
	rcb->chunks_served++;
    if (rcb->bytes_unsent < sent) {
		rcb->bytes_unsent = 0;
	} else {
		rcb->bytes_unsent -= sent;
	}
}

/**
 * @param rcb
 * @return  True if there is no more data to be sent.
 */
bool rcb_completed(RCB *rcb) {
	return rcb->bytes_unsent == 0;
}

/**
 * Note that a semaphore should be released when an RCB is destroyed.
 * @param rcb
 */
void rcb_destroy(RCB *rcb) {
    close(rcb->client_connection);
    if (rcb->requested_file) {
        fclose(rcb->requested_file);
    }
    free(rcb->filename);
    free(rcb);
    rcb = NULL;
}