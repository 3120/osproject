#ifndef NODE_H
#define NODE_H

#include "rcb.h"

typedef struct _node {
    struct _node *next;
    RCB *rcb;
} Node;

Node* node_create();
Node* node_init(RCB *rcb);
void node_destroy(Node *victim);

#endif
