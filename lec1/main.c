#include "threads.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>


int threadID[threadCount];

int main(int argc, char **argv)
{

    srand(time(NULL));

    pthread_t gui_th, gendata_th,locate_th,locate_th_pthreads[threadCount];
    pthread_create(&gui_th, NULL, GUI, NULL);
    pthread_create(&gendata_th, NULL,generate_data, NULL);
    pthread_mutex_init(myMutex(),NULL);
    pthread_barrier_init(myBarrier(),NULL,threadCount);



    /* Sequential Execution */
    //pthread_create(&locate_th,NULL,locate_object,NULL);
    //pthread_join(locate_th, NULL);

    /* Parallel Execution With Pthreads */
    // for(int i = 0; i < threadCount; i++ ){
    //     threadID[i]=i;
    //     pthread_create(&locate_th_pthreads[i], NULL,locate_object_pthreads,&threadID[i]);
    // }

    /* Parallel Execution With OpenMp */
    locate_object_openMP();


    pthread_join(gui_th, NULL);
    pthread_join(gendata_th, NULL);

    /* Parallel Execution With Pthreads */
    // for(int i = 0; i < threadCount; i++ ){
    //     pthread_join(locate_th_pthreads[i], NULL);
    // }





}


