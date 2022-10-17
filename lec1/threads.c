#define _GNU_SOURCE
#include "threads.h"
//#include "sdl_utils.h"
#include "play_utils.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <time.h>
//#include <SDL.h>
#include <pthread.h>
#include <omp.h>


#define PERIOD_MS 50

bool data[MAP_WIDTH][MAP_HEIGHT];
int quit = 0;
int xrange, yrange, is, js;
pthread_mutex_t lock;
pthread_barrier_t barrier;
double Wcet = 0.0;

pthread_mutex_t* myMutex()
{
    return &lock;
}

pthread_barrier_t* myBarrier()
{
    return &barrier;
}

bool ball_found = false;

void timespec_add(const struct timespec *a, const struct timespec *b, struct timespec *res)
{
    res->tv_nsec = a->tv_nsec + b->tv_nsec;
    res->tv_sec = a->tv_sec + b->tv_sec;
    while (res->tv_nsec >= 1000000000)
    {
        res->tv_nsec -= 1000000000;
        res->tv_sec += 1;
    }
}

void *GUI(void *args)
{

    //    SDL_init();
    play_init();

    struct timespec next, period = {0, PERIOD_MS * 1000000};
    clock_gettime(CLOCK_MONOTONIC, &next);

    while (quit == 0)
    {
        timespec_add(&next, &period, &next);

        // SDL_PumpEvents();
        // const uint8_t *keystate = SDL_GetKeyboardState(NULL);
        // if (keystate[SDL_SCANCODE_ESCAPE])
        //{
        //     quit = 1;
        // }

        // SDL_clear_map();

        play_move_ball();
        // SDL_draw_ball();
        // SDL_draw_map();

        if (is != -10)
        {
            // SDL_draw_detection(is, js, xrange, yrange);
        }

        // SDL_refresh();

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }
    // SDL_destroy();
}

/* Object Location using Sequential Code */

void *locate_object(void *args)
{
    double time_spent = 0.0;
    clock_t begin,end;

    struct timespec next, period = {0, PERIOD_MS * 1000000};
    clock_gettime(CLOCK_MONOTONIC, &next);
    while (quit == 0)
    {
        begin = clock();
        timespec_add(&next, &period, &next);
        for (int i = 0; i < MAP_WIDTH; i++)
        {
            for (int j = 0; j < MAP_HEIGHT; j++)
            {
                if (data[i][j] == true)
                {
                    end = clock();
                    ball_found = true;
                    goto loopEnd;
                }
            }
        }

loopEnd:
        time_spent = (end-begin) / (double)CLOCKS_PER_SEC;
        printf("The elapsed time is %f seconds\n", time_spent);
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }
}

/* Object Location using Pthreads */

void *locate_object_pthreads(void *args)
{

    int *tid = (int*) args;
    int id = *tid;

    double time_spent = 0.0;
    clock_t begin,end;

    struct timespec next, period = {0, PERIOD_MS * 1000000};
    clock_gettime(CLOCK_MONOTONIC, &next);
    while (quit == 0)
    {
        begin = clock();
        pthread_barrier_wait(&barrier);
        pthread_mutex_lock(&lock);
        ball_found = false;
        pthread_mutex_unlock(&lock);
        timespec_add(&next, &period, &next);
        for (int i = id; i < MAP_WIDTH; i+=threadCount)
        {
            for (int j = 0; j < MAP_HEIGHT; j++)
            {
                if (data[i][j] == true)
                {
                    pthread_mutex_lock(&lock);
                    ball_found = true;
                    pthread_mutex_unlock(&lock);
                    goto loopEnd;
                }
            }
        }

loopEnd:
        pthread_barrier_wait(&barrier);
        end = clock();

        time_spent = (end-begin) / (double)CLOCKS_PER_SEC;

        if(time_spent>Wcet)
        {
            Wcet = time_spent;
        }
        printf("The WCET time is %f seconds\n", Wcet);

        printf("The computation time is %f seconds\n", time_spent);
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }
}

/* Object Location using OpenMP */

void locate_object_openMP()
{
    int nthreads=0;
    #pragma omp parallel
    {
        nthreads= omp_get_num_threads();
    }

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();

        double time_spent = 0.0;
        clock_t begin,end;

        struct timespec next, period = {0, PERIOD_MS * 1000000};
        clock_gettime(CLOCK_MONOTONIC, &next);
        while (quit == 0)
        {
            begin = clock();
            #pragma omp barrier
            #pragma omp critical
            {
                ball_found = false;
            }

            timespec_add(&next, &period, &next);
            for (int i = tid; i < MAP_WIDTH; i+=nthreads)
            {
                for (int j = 0; j < MAP_HEIGHT; j++)
                {
                    if (data[i][j] == true)
                    {
                        #pragma omp critical
                        {
                            ball_found = true;
                        }
                        goto loopEnd;
                    }
                }
            }

loopEnd:
            #pragma omp barrier
            end = clock();
            time_spent = (end-begin) / (double)CLOCKS_PER_SEC;

            if(time_spent>Wcet)
            {
                Wcet = time_spent;
            }
            printf("The WCET time is %f seconds\n", Wcet);
            printf("The computation time is %f seconds\n", time_spent);
            clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
        }
    }
}


void *generate_data(void *args)
{

    struct timespec next, period = {0, PERIOD_MS * 1000000};
    clock_gettime(CLOCK_MONOTONIC, &next);
    while (quit == 0)
    {
        timespec_add(&next, &period, &next);
        for (int i = 0; i < MAP_WIDTH; i++)
        {
            for (int j = 0; j < MAP_HEIGHT; j++)
            {
                data[i][j] = ((i >= getBall()->x) && (i <= getBall()->x + BALL_WIDTH) && (j >= getBall()->y) && (j >= getBall()->y + BALL_HEIGHT)) ? true : false;
            }
        }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }
}
