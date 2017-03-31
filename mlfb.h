#ifndef MLFB_H
#define MLFB_H

#include "linked_list.h"
#include "network.h"
#include "rcb.h"
#include "http.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void mlfb_serve(RCB *rcb, int chunk);
void mlfb_loop();

#endif