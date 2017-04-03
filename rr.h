#ifndef RR_H
#define RR_H

#include "queue.h"
#include "network.h"
#include "rcb.h"
#include "http.h"
#include "globals.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

RCB* rr_serve(RCB*);
void rr_enqueue(RCB*);
RCB* rr_dequeue();

#endif
