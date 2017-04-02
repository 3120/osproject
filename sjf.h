#ifndef SJF_H
#define SJF_H

#include "rcb.h"
#include "queue.h"
#include "network.h"
#include "http.h"
#include "globals.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

RCB* sjf_serve(RCB*);
void sjf_enqueue(RCB*);
RCB* sjf_dequeue();

#endif
