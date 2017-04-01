#ifndef HTTP_H
#define HTTP_H

#define MAX_HTTP_SIZE 8192
#define CHUNK_8KB 8192
#define CHUNK_64KB 65536

#include <stdio.h>
#include "rcb.h"

RCB* http_process_request(int client_connection);
void http_read_request(int client_connection, char *buffer);
char* http_parse_request();
char* http_create_buffer(int chunk);
FILE* http_open_file(int client_connection, char *requested_file);
void http_respond(int error_code, int client_connection, char *buffer);

#endif