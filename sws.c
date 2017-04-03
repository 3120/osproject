#include "sws.h"
#include "rcb.h"

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
    pthread_cond_init(&request_wait, NULL);
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
        int request = -1;
        while (request == -1) request = network_open();
        sem_wait(&permission_to_queue);
        add_request(rcb_init(request));
    }
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
        scheduler_dequeue = &mlfb_dequeue;
        scheduler_enqueue = &mlfb_enqueue;
        scheduler_serve = &mlfb_serve;
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

void add_request(RCB *rcb) {
    pthread_mutex_lock(&work_lock);
        queue_enqueue(work_queue, rcb);
        pthread_cond_signal(&request_wait);
    pthread_mutex_unlock(&work_lock);
}

void* worker_activity() {
    while(true) {
        /* Check the work queue first */
        RCB *waiting_for_processing = NULL;
        pthread_mutex_lock(&work_lock);
            waiting_for_processing = queue_dequeue(work_queue);
        pthread_mutex_unlock(&work_lock);

        /* If there are requests in the work queue, validate and admit them */
        if (waiting_for_processing) {
            /* On failed validation, free the position in line and destroy the RCB */
            if (rcb_process(waiting_for_processing)) {
                printf("Request for file %s admitted\n", waiting_for_processing->filename);
                fflush(stdout);
                scheduler_enqueue(waiting_for_processing);
            } else {
                sem_post(&permission_to_queue);
                rcb_destroy(waiting_for_processing);
            }
        } else {
            /* If there are requests in the ready queue, process them and return
             * them to the queue if they still exist */
            RCB *ready = scheduler_dequeue();
            if (ready) {
                ready = scheduler_serve(ready);
                /* If the request was completed, destroy the RCB and write about it. */
                ready = worker_check_completed(ready);
                /* Requeue the RCB, if it wasn't finished */
                scheduler_enqueue(ready);
            } else {
                /* Sleep until a request appears */
                pthread_mutex_lock(&work_lock);
                    while (queue_is_empty(work_queue)) {
                        pthread_cond_wait(&request_wait, &work_lock);
                    }
                pthread_mutex_unlock(&work_lock);
            }
        }
    }
}

RCB* worker_check_completed(RCB *rcb) {
    if (rcb_completed(rcb)) {
        printf("Request for file %s completed\n", rcb->filename);
        fflush(stdout);
        rcb_destroy(rcb);
        sem_post(&permission_to_queue);
        return NULL;
    } else {
        return rcb;
    }
}