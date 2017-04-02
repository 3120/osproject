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

void rr_serve(RCB *rcb);
void rr_enqueue(RCB *rcb);
void rr_dequeue();
void rr_finish_cycle(Node *processed);

#endif
