#include <stdlib.h>
#include "node.h"

Node* node_create() {
    Node *new = malloc(sizeof(Node));
    if (!new) {
        perror("Error while allocating memory");
        abort();
    }
    
    return new;
}

Node* node_init(RCB *rcb) {
    Node *new = node_create();
    new->next = NULL;
    new->rcb = rcb;
    return new;
}

void node_destroy(Node *victim) {
    if (victim->rcb) rcb_destroy(victim->rcb);
    
    free(victim);
    victim = NULL;
}
