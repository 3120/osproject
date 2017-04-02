#include "sws.h"

int main( int argc, char **argv ) {
    /* Handle commandline args */
    int port = -1;
    if( ( argc < 3 ) || ( sscanf( argv[1], "%d", &port ) < 1 )) {
        printf( "usage: sws <port> <scheduler>\n" );
        return 0;
    }

    if (!choose_scheduler(argv[2])) return 0;

    /* Initialize thread-safety */
    pthread_mutex_init(&work_lock, NULL);
    pthread_mutex_init(&top_lock, NULL);
    pthread_mutex_init(&middle_lock, NULL);
    pthread_mutex_init(&bottom_lock, NULL);
    network_init(port);
    work_queue = queue_init();
    sem_init(&permission_to_queue, 0, 100);

    /*
     * Main loop. Always be checking for requests from the network. If a
     * request is found, try to add it to the work queue. If no space is
     * available in the queue, block until a slot opens.
     */
    while (true) {
        int request = -1;
        while (request == -1) request = network_open();
        sem_wait(&permission_to_queue);
        add_request(rcb_init(request));
        close(request);
    }
}

void add_request(RCB *rcb) {
    pthread_mutex_lock(&work_lock);
    queue_enqueue(work_queue, rcb);
    pthread_mutex_unlock(&work_lock);
}

bool choose_scheduler(char *scheduler) {
    if (strcmp(scheduler, "SJF") == 0) {
        top_queue = queue_init();
        scheduler_dequeue = &sjf_dequeue;
        scheduler_enqueue = &sjf_enqueue;
        scheduler_serve = &sjf_serve;
    } else if (strcmp(scheduler, "RR") == 0) {
        top_queue = queue_init();
        scheduler_dequeue = &rr_dequeue;
        scheduler_enqueue = &rr_enqueue;
        scheduler_serve = &rr_serve;
    } else if (strcmp(scheduler, "MLFB") == 0) {
        top_queue = queue_init();
        middle_queue = queue_init();
        bottom_queue = queue_init();
    } else {
        printf("Valid schedulers are SJF/RR/MLFB\n");
        return false;
    }

    return true;
}