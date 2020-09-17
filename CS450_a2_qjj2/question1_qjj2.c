#include <stdio.h>
#include <pthread.h>

#define THREAD_NUM 3

// Declare func
void *do_work(void *args);

// Declare arg struct
typedef struct arguments
{
    int threadID;
    pthread_mutex_t *lock_in;
    int *buffer_addr;
} Args;

// Do_work function
void *do_work(void *args)
{
    // Digest arg input
    Args *argument = (Args *)args;
    int local_thread_id = argument->threadID;
    pthread_mutex_t *local_mutex = argument->lock_in;
    int out_buffer[THREAD_NUM] = argument->buffer_addr;
}

// Entry point
int main()
{

    // Declare variables
    pthread_t worker_threads[THREAD_NUM];
    pthread_mutex_t mutex;
    int buffer[THREAD_NUM];
    Args *args[THREAD_NUM];
    int correct_counter = 0;

    //Initialize struct args
    int loop_iter;
    for (loop_iter = 0; loop_iter < THREAD_NUM; loop_iter++)
    {
        args[loop_iter] = (Args *)calloc(1, sizeof(Args));
        args[loop_iter]->threadID = loop_iter + 1;
        args[loop_iter]->buffer_addr = buffer;
        args[loop_iter]->lock_in = &mutex;
    }

    // Create threads
    for (loop_iter = 0; loop_iter < THREAD_NUM; loop_iter++)
    {
        if (pthread_create(&worker_threads[loop_iter], NULL, do_work, (void *)args[loop_iter]))
        {
            fprintf(stderr, "Error while creating thread %d\n", loop_iter);
            exit(1);
        }
    }

    // Join threads
    for (loop_iter = 0; loop_iter < THREAD_NUM; loop_iter++)
    {
        if (pthread_join(&worker_threads[loop_iter], NULL))
        {
            fprintf(stderr, "Error while joining thread %d\n", loop_iter);
            exit(1);
        }
    }
}