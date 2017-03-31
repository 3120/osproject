#ifndef SJF_H
#define SJF_H

#include "rcb.h"
#include "queue.h"
#include "network.h"
#include "http.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void sjf_serve(RCB *rcb);
void sjf_loop();
void sjf_process_new_requests();

#endif
