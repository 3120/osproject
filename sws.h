#ifndef SWS_H
#define SWS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#include "network.h"
#include "http.h"
#include "sjf.h"
#include "rr.h"
#include "mlfb.h"
#include "globals.h"

void add_request(RCB*);
void choose_scheduler(char*);
void set_thread_count(char*);
void* worker_activity();
RCB* worker_check_completed(RCB*);

#endif
