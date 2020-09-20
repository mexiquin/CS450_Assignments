#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define THREAD_NUM 3

// Global Variables
int buffer[THREAD_NUM] = {0};
int correct_counter = 0;
int total_counter = 0;

// Declare func
void *do_work(void *args);

// Declare arg struct
typedef struct arguments
{
    int threadID;
    pthread_mutex_t *lock_in;

} Args;

// Do_work function
void *do_work(void *args)
{
    // Digest arg input
    Args *argument = (Args *)args;
    int local_thread_id = argument->threadID;
    pthread_mutex_t *local_mutex = argument->lock_in;

    // While loop that will keep going until the correct values are reached
    while (correct_counter < 10)
    {
        pthread_mutex_lock(local_mutex);
        printf("My id: %d\n", local_thread_id);

        //printf("Thread %d Locking\n", local_thread_id);
        int i;
        for (i = 0; i < THREAD_NUM; i++)
        {
            //printf("Checking if element %d is available...\n", i);
            if (buffer[i] == 0)
            {
                buffer[i] = local_thread_id;
                //printf("Adding value %d to element %d\n", local_thread_id, i);
                break;
            }
        }
        //printf("Thread %d unlocking\n", local_thread_id);
        pthread_mutex_unlock(local_mutex);
        usleep(500000);

        // once the buffer is full
        if (buffer[0] != 0 &&
            buffer[1] != 0 &&
            buffer[2] != 0)
        {
            pthread_mutex_lock(local_mutex);
            //printf("End of buffer reached with thread %d at index %d\n", local_thread_id, i);
            // check the sequence
            if (buffer[0] == 1 &&
                buffer[1] == 2 &&
                buffer[2] == 3)
            {
                // if sequence is correct, iterate the correct counter
                correct_counter++;
                printf("%d%d%d\n", buffer[0], buffer[1], buffer[2]);
            }

            //// Clear buffer ///
            buffer[0] = 0;
            buffer[1] = 0;
            buffer[2] = 0;
            ////////////////////

            //increment total sequence iterations
            total_counter++;
            //printf("\nResults of this Iteration:\nTotal Iter: %d\nCorrect Iter: %d\n", total_counter, correct_counter);
            pthread_mutex_unlock(local_mutex);
        }
    }
}

// Entry point
int main()
{

    // Declare variables
    pthread_t worker_threads[THREAD_NUM];
    pthread_mutex_t mutex;
    Args *args[THREAD_NUM];

    pthread_mutex_init(&mutex, NULL);

    //Initialize struct args
    int loop_iter;
    for (loop_iter = 0; loop_iter < THREAD_NUM; loop_iter++)
    {
        args[loop_iter] = (Args *)calloc(1, sizeof(Args));
        args[loop_iter]->threadID = loop_iter + 1;
        args[loop_iter]->lock_in = &mutex;
    }

    // Create threads
    for (loop_iter = 0; loop_iter < THREAD_NUM; loop_iter++)
    {
        if (pthread_create(&worker_threads[loop_iter], NULL, do_work, (void *)args[loop_iter]))
        {
            printf("Error while creating thread %d\n", loop_iter);
            exit(1);
        }
    }

    // Join threads
    for (loop_iter = 0; loop_iter < THREAD_NUM; loop_iter++)
    {
        if (pthread_join(worker_threads[loop_iter], NULL))
        {
            printf("Error while joining thread %d\n", loop_iter);
            exit(1);
        }
    }

    // Once the loop is done, print out results and exit
    printf("Total sequences generated: %d\n", total_counter);
    printf("Number of correct sequences: %d\n", correct_counter);
    exit(0);
}