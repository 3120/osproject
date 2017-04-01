/* 
 * File: sws.c
 * Author: Nicholas McDonald
 * Purpose: This file contains the implementation of a simple web server.
 *          It loads a scheduler based on a CLI flag and runs the server
 *          indefinitely. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "network.h"
#include "http.h"
#include "sjf.h"
#include "rr.h"
#include "mlfb.h"


/* This function is where the program starts running.
 *    The function first parses its command line parameters to determine port #
 *    Then, it initializes, the network and enters the main loop.
 *    The main loop waits for a client (1 or more to connect, and then processes
 *    all clients by calling the seve_client() function for each one.
 * Parameters: 
 *             argc : number of command line parameters (including program name
 *             argv : array of pointers to command line parameters
 * Returns: an integer status code, 0 for success, something else for error.
 */
int main( int argc, char **argv ) {
  int port = -1;                                    /* server port # */

  /* check for and process parameters 
   */
  if( ( argc < 3 ) || ( sscanf( argv[1], "%d", &port ) < 1 )) {
    printf( "usage: sws <port> <scheduler>\n" );
    return 0;
  }

  char *flag = argv[2];
  void (*scheduler_loop)(); 
  if (strcmp(flag, "SJF") == 0) {
    scheduler_loop = &sjf_loop;
  } else if (strcmp(flag, "RR") == 0) {
    scheduler_loop = &rr_loop;
  } else if (strcmp(flag, "MLFB") == 0) {
    /* scheduler_loop = &mlfb_loop; */
  } else {
    printf("Valid schedulers are SJF/RR/MLFB\n");
    return 0;
  }

  network_init( port );                             /* init network module */

  (*scheduler_loop)();
}
