/* 
 * File: http.c
 * Author: Nicholas McDonald
 * Purpose: This file contains the implementation of the low-level functions
 * 		used in processing HTTP requests. Only the function
 *		http_process_request should be considered "public".
 */
#include "http.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

/* Take a file handle to an incoming connection. If the client has sent
 * 	a valid request, create a Request Control Block to represent the
 *	request and send a success response. Otherwise, send an error response.
 *
 *	Params:
 *		client connection as FileDescriptor
 *	Returns:
 *		Request control block as RCB*, or null if request failed
 */ 
RCB* http_process_request(int client_connection) {
	if (client_connection < 0) return NULL;
	
	RCB *block = NULL;
	char *buffer = http_create_buffer(MAX_HTTP_SIZE);
	http_read_request(client_connection, buffer);
	char *request = http_parse_request(buffer);

	if (!request) {
		http_respond(400, client_connection, buffer);
	} else {
		FILE *requested_file = http_open_file(client_connection, request);
		if (!requested_file) {
			http_respond(404, client_connection, buffer);
		} else {
			/* Create a request control block to add to the service queue. */
			block = rcb_init(client_connection, requested_file);
			http_respond(200, client_connection, buffer);
		}
	}

	free(buffer);
	return block;
}

char* http_create_buffer(int chunk) {
	char *buffer = malloc(chunk);
	if(!buffer) {
		perror( "Error while allocating memory" );
		abort();
	}

	return buffer;
}

void http_read_request(int client_connection, char *buffer) {
	memset(buffer, 0, MAX_HTTP_SIZE);
	if( read(client_connection, buffer, MAX_HTTP_SIZE) <= 0 ) {
		perror( "Error while reading request" );
		abort();
	}
}

char* http_parse_request(int client_connection, char *buffer) {
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

FILE* http_open_file(int client_connection, char *requested_file) {
    return fopen(requested_file, "r");
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

