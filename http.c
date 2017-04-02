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

