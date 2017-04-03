#include "sws.h"

int main( int argc, char **argv ) {
    /* Handle commandline args */
    int port = -1;
    if( ( argc < 4 ) || ( sscanf( argv[1], "%d", &port ) < 1 )) {
        printf( "usage: sws <port> <scheduler> <thread_count>\n" );
        return 0;
    }
    choose_scheduler(argv[2]);
    set_thread_count(argv[3]);

    /* Initialize thread-safety */
    pthread_mutex_init(&work_lock, NULL);
    pthread_mutex_init(&top_lock, NULL);
    pthread_mutex_init(&middle_lock, NULL);
    pthread_mutex_init(&bottom_lock, NULL);
    network_init(port);
    work_queue = queue_init();
    sem_init(&permission_to_queue, 0, 100);

    /* Initialize worker threads */
    pthread_t workers[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&workers[i], NULL, worker_activity, NULL);
    }

    /*
     * Main loop. Always be checking for requests from the network. If a
     * request is found, try to add it to the work queue. If no space is
     * available in the queue, block until a slot opens.
     */
    while (true) {
        printf("Hello.\n");
        int request = -1;
        while (request == -1) request = network_open();
        sem_wait(&permission_to_queue);
        add_request(rcb_init(request));
        int remaining = 0;
        sem_getvalue(&permission_to_queue, &remaining);
        printf("%d\n", remaining);
    }
}

void add_request(RCB *rcb) {
    pthread_mutex_lock(&work_lock);
    queue_enqueue(work_queue, rcb);
    pthread_mutex_unlock(&work_lock);
}

void choose_scheduler(char *scheduler) {
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
        abort();
    }
}

void set_thread_count(char *arg) {
    THREAD_COUNT = atoi(arg);
    if (THREAD_COUNT < 1 || THREAD_COUNT > 100) {
        perror("Error: thread count must be between 1-100\n");
        abort();
    }
}

void* worker_activity() {
    while(true) {
        pthread_mutex_lock(&work_lock);
        RCB *waiting_for_processing = queue_dequeue(work_queue);
        pthread_mutex_unlock(&work_lock);

        /* If there are requests in the work queue, validate and admit them */
        if (waiting_for_processing) {
            /* On failed validation, free the position in line and destroy the RCB */
            if (rcb_process(waiting_for_processing)) {
                scheduler_enqueue(waiting_for_processing);
                printf("Admitted to scheduler\n");
            } else {
                sem_post(&permission_to_queue);
                rcb_destroy(waiting_for_processing);
            }
        } else {
            /* If there are requests in the ready queue, process them and return
             * them to the queue if they still exist */
            RCB *ready_for_serving = scheduler_dequeue();
            if (ready_for_serving) {
                ready_for_serving = scheduler_serve(ready_for_serving);
                scheduler_enqueue(ready_for_serving);
            } else {
                /* Sleep until something changes */
            }
        }
    }
}