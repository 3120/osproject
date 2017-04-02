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

void sjf_serve(RCB*);
void sjf_enqueue(RCB*);
void sjf_dequeue();

#endif
