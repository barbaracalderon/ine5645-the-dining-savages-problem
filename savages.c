#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define MEALS 3
#define NUM_SAVAGES 1

sem_t emptyPot, fullPot, mutex;
int servings = 0;

void *thread_cook(void* arg) {
    while(1) {
        sem_wait(&emptyPot);
        printf("    Cook: I'm serving food into the pot.\n");
        sem_post(&fullPot);
    };
}

void *thread_savage(void* arg) {
    long *rank;
    rank = (long *)arg;
    printf("--> Savage thread starts here. %ld.\n", *rank);
    sem_wait(&mutex);
        printf("Savage %ld: I just arrived at the pot.\n", *rank);
        if (servings == 0) {
            printf("Savage %ld: There isn't any food in the pot. I'm going to warn the cook. \n", *rank);
            sem_post(&emptyPot);
            printf("Savage %ld: I'm waiting the cook to fill the pot. \n", *rank);
            sem_wait(&fullPot);
            printf("Savage %ld: The pot is now full. \n", *rank);
            servings = MEALS;
        };
        printf("Savage %ld: I'm grabbing food from the pot. \n", *rank);
        servings -= 1;
        printf("Savage %ld: I just grabbed some food from the pot. Now there's %d meals in the pot. \n", *rank, servings);
    sem_post(&mutex);
    printf("Savage %ld: I'm eating. \n", *rank);
}

int main(int argc, char *argv[]) {

    if (MEALS <= 0 || NUM_SAVAGES <= 0) {
        printf("ERROR: The number of MEALS or SAVAGES should be above 0. \n");
        exit(0);
    }

    // OPENING THE COOK THREAD
    pthread_t t_cook;
    int cook = pthread_create(&t_cook, NULL, thread_cook, NULL);
    
    // STARTING SEMAPHORES
    sem_init(&emptyPot, 0, 0);
    sem_init(&fullPot, 0, 0);
    sem_init(&mutex, 0, 1);

    long i, *ids[NUM_SAVAGES];
    pthread_t t_savage[NUM_SAVAGES];
    
    // OPENING UP ALL SAVAGE THREADS
    printf("==> NUM_SAVAGES: %d.\n", NUM_SAVAGES);
    printf("==> MEALS: %d.\n", MEALS);
    for (i = 1; i <= NUM_SAVAGES; i++) {
        ids[i] = malloc(sizeof(long));
        *ids[i] = i;
        pthread_create(&t_savage[i], NULL, thread_savage, (void*)ids[i]);
        };

    // CLOSING ALL SAVAGE THREADS
    for (i = 1; i <= NUM_SAVAGES; i++) {
        pthread_join(t_savage[i], NULL);
    };

    // END OF ALL THREADS
    printf("==> All threads are now finished.\n");

    // DESTROYING SEMAPHORES
    sem_destroy(&emptyPot);
    sem_destroy(&fullPot);
    sem_destroy(&mutex);
    
	return (0);
}
