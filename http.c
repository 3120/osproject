/* 
 * File: http.c
 * Author: Nicholas McDonald
 * Purpose: This file contains the implementation of the low-level functions
 * 		used in processing HTTP requests. Only the function
 *		http_process_request should be considered "public".
 */
#include "http.h"
#include "rcb.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

char* http_create_buffer(int chunk) {
	char *buffer = malloc(chunk);
	if(!buffer) {
		perror( "Error while allocating memory" );
		abort();
	}

	return buffer;
}

void http_write_to_client(RCB *rcb, int chunk_size) {
    /* Read up to the maximum size into the write buffer. */
    char *buffer = http_create_buffer(chunk_size);
    int len = fread( buffer, 1, chunk_size, rcb->requested_file );
    if( len < 0 ) {
        perror( "Error while writing to client" );
    } else if( len > 0 ) {
        /* Transmit the buffer contents to the client */
        len = write( rcb->client_connection, buffer, len );
        if( len < 1 ) {
            perror( "Error while writing to client" );
        } else {
            /* Reflect the transmission in the RCB */
            rcb_update_record(rcb, chunk_size);
            printf("Sent %d bytes of file %s\n", len, rcb->filename);
            fflush(stdout);
        }
    }

    /* Clear the buffer for the next transfer. */
    free(buffer);
}

void http_read_request(int client_connection, char *buffer) {
	memset(buffer, 0, MAX_HTTP_SIZE);
	if( read(client_connection, buffer, MAX_HTTP_SIZE) <= 0 ) {
		perror( "Error while reading request" );
		abort();
	}
}

/**
 * Brodsky obviously wrote this.
 */
char* http_parse_request(char *buffer) {
	char *tmp, *brk, *req = NULL;

	tmp = strtok_r( buffer, " ", &brk );
	if( tmp && !strcmp( "GET", tmp ) ) {
		req = strtok_r( NULL, " ", &brk );
	}

	if(req) {
		req++;
	}

	return req;
}

void http_respond(int error_code, int client_connection, char *buffer) {
	int len;

	switch (error_code) {
		case 404: len = sprintf(buffer, "HTTP/1.1 404 File not found\n\n");
			break;
		case 400: len = sprintf(buffer, "HTTP/1.1 400 Bad request\n\n");
			break;
		case 200: len = sprintf(buffer, "HTTP/1.1 200 OK\n\n");
			break;
	}

	write(client_connection, buffer, len);
}

