#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define THREAD_NUM 3
#define TOT_THREAD 6

// Global Variables
int buffer[THREAD_NUM] = {0};
int buffer2[THREAD_NUM] = {0};
int correct_counter = 0;
int correct_counter2 = 0;
int total_counter = 0;
int total_counter2 = 0;

// Declare func
void *do_work(void *args);
void *do_work2(void *args);

// Declare arg struct
typedef struct arguments
{
    int threadID;
    pthread_mutex_t *lock_in;
    pthread_cond_t *is_finished;

} Args;


// Do_work function
void *do_work(void *args)
{
    // Digest arg input
    Args *argument = (Args *)args;
    int local_thread_id = argument->threadID;
    pthread_mutex_t *local_mutex = argument->lock_in;
    pthread_cond_t loc_is_finished = *(argument->is_finished);

    // While loop that will keep going until the correct values are reached
    while (correct_counter < 10)
    {
        pthread_mutex_lock(local_mutex);
        printf("My id: %d\n", local_thread_id);

        int i;
        for (i = 0; i < THREAD_NUM; i++)
        {
            if (buffer[i] == 0)
            {
                buffer[i] = local_thread_id;
                break;
            }
        }
        pthread_mutex_unlock(local_mutex);
        usleep(500000);

        // once the buffer is full
        if (buffer[0] != 0 &&
            buffer[1] != 0 &&
            buffer[2] != 0)
        {
            pthread_mutex_lock(local_mutex);
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
            pthread_mutex_unlock(local_mutex);

            // After execution, in the case that the other team won, wait until the winning team finishes operations
            if (correct_counter2 == 10)
            {
                pthread_cond_wait(&loc_is_finished, local_mutex);
            }
        }
    }

    // Now that we're outside of the for-loop, that means that somebody's won! We need to lock it
    // so that other threads wont keep going wile this thread assesses victory
    pthread_mutex_lock(local_mutex);
    if (correct_counter == 10)
    {
        fprintf(stderr, "Team 1 won!\n");
        pthread_cond_broadcast(&loc_is_finished);
    }
    pthread_mutex_unlock(local_mutex);
    return NULL;
}


// Do_work 456 function
void *do_work2(void *args)
{
    // Digest arg input
    Args *argument = (Args *)args;
    int local_thread_id = argument->threadID;
    pthread_mutex_t *local_mutex = argument->lock_in;
    pthread_cond_t loc_is_finished = *(argument->is_finished);
    // While loop that will keep going until the correct values are reached
    while (correct_counter2 < 10)
    {
        pthread_mutex_lock(local_mutex);
        printf("My id: %d\n", local_thread_id);

        //printf("Thread %d Locking\n", local_thread_id);
        int i;
        for (i = 0; i < THREAD_NUM; i++)
        {
            //printf("Checking if element %d is available...\n", i);
            if (buffer2[i] == 0)
            {
                buffer2[i] = local_thread_id;
                //printf("Adding value %d to element %d\n", local_thread_id, i);
                break;
            }
        }
        //printf("Thread %d unlocking\n", local_thread_id);
        pthread_mutex_unlock(local_mutex);
        usleep(500000);

        // once the buffer2 is full
        if (buffer2[0] != 0 &&
            buffer2[1] != 0 &&
            buffer2[2] != 0)
        {
            pthread_mutex_lock(local_mutex);
            //printf("End of buffer2 reached with thread %d at index %d\n", local_thread_id, i);
            // check the sequence
            if (buffer2[0] == 4 &&
                buffer2[1] == 5 &&
                buffer2[2] == 6)
            {
                // if sequence is correct, iterate the correct counter
                correct_counter2++;
                printf("%d%d%d\n", buffer2[0], buffer2[1], buffer2[2]);
            }

            //// Clear buffer2 ///
            buffer2[0] = 0;
            buffer2[1] = 0;
            buffer2[2] = 0;
            ////////////////////

            //increment total sequence iterations
            total_counter2++;
            //printf("\nResults of this Iteration:\nTotal Iter: %d\nCorrect Iter: %d\n", total_counter, correct_counter);
            pthread_mutex_unlock(local_mutex);

            // After execution, in the case that the other team won, wait until the winning team finishes operations
            if (correct_counter == 10)
            {
                pthread_cond_wait(&loc_is_finished, local_mutex);
            }
        }
    }

    // Now that we're outside of the for-loop, that means that somebody's won! We need to lock it
    // so that other threads wont keep going wile this thread assesses victory
    pthread_mutex_lock(local_mutex);
    if (correct_counter2 == 10)
    {
        fprintf(stderr, "Team 2 won!\n");
        pthread_cond_broadcast(&loc_is_finished);
    }

    return NULL;
}

// Entry point
int main()
{

    // Declare variables
    pthread_t worker_threads[TOT_THREAD];
    pthread_mutex_t mutex;
    pthread_mutex_t mutex2;
    Args *args[THREAD_NUM];
    Args *args2[THREAD_NUM];
    pthread_cond_t is_finished;

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex2, NULL);
    pthread_cond_init(&is_finished, NULL);

    //////// Initialize struct args ////////
    int loop_iter;

    //Struct args for 123 and 456
    for (loop_iter = 0; loop_iter < THREAD_NUM; loop_iter++)
    {
        args[loop_iter] = (Args *)calloc(1, sizeof(Args));
        args[loop_iter]->threadID = loop_iter + 1;
        args[loop_iter]->lock_in = &mutex;
        args[loop_iter]->is_finished = &is_finished;

        args2[loop_iter + THREAD_NUM] = (Args *)calloc(1, sizeof(Args));
        args2[loop_iter + THREAD_NUM]->threadID = loop_iter + 1;
        args2[loop_iter + THREAD_NUM]->lock_in = &mutex2;
        args2[loop_iter + THREAD_NUM]->is_finished = &is_finished;
    }

    //////// Create Threads ////////

    // Create threads for 123 and 456
    for (loop_iter = 0; loop_iter < THREAD_NUM; loop_iter++)
    {
        //123
        if (pthread_create(&worker_threads[loop_iter], NULL, do_work, (void *)args[loop_iter]))
        {
            printf("Error while creating thread %d\n", loop_iter);
            exit(1);
        }

        //456
        if (pthread_create(&worker_threads[loop_iter+THREAD_NUM], NULL, do_work2, (void *)args2[loop_iter]))
        {
            printf("Error while creating thread %d\n", loop_iter);
            exit(1);
        }
    }

    //////// Join threads ////////
    for (loop_iter = 0; loop_iter < TOT_THREAD; loop_iter++)
    {
        if (pthread_join(worker_threads[loop_iter], NULL))
        {
            printf("Error while joining thread %d\n", loop_iter);
            exit(1);
        }
    }

    // Modify the end result so that it generates win results
    printf("Total sequences generated team1: %d\n", total_counter);
    printf("Number of correct sequences team1: %d\n", correct_counter);
    printf("Total sequences generated team2: %d\n", total_counter2);
    printf("Number of correct sequences team2: %d\n", correct_counter);
    exit(0);
}
