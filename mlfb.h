#ifndef MLFB_H
#define MLFB_H

#include "queue.h"
#include "network.h"
#include "rcb.h"
#include "http.h"
#include "globals.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

RCB* mlfb_serve(RCB*);
void mlfb_enqueue(RCB*);
RCB* mlfb_dequeue();
int mlfb_get_send_length(RCB*);

#endif