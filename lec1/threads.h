#ifndef THREADS_H
#define THREADS_H

#include <time.h>
#include <pthread.h>

#define threadCount 16


void * GUI(void *args);
void *generate_data(void *args);
void *locate_object(void *args);
void *locate_object_pthreads(void *args);
void locate_object_openMP();
void locate_object_openMP_for();
void timespec_add(const struct timespec *a, const struct timespec *b, struct timespec * res);
pthread_mutex_t* myMutex();
pthread_barrier_t* myBarrier();

#endif
