#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define MEALS 3
#define NUM_SAVAGES 1

sem_t emptyPot, fullPot, mutex;
int servings = 0;

void *thread_cozinheiro(void* arg) {
    while(1) {
        sem_wait(&emptyPot);
        printf("    Cozinheiro: estou servindo comida na panela.\n");
        sem_post(&fullPot);
    };
}

void *thread_selvagem(void* arg) {
    long *rank;
    rank = (long *)arg;
    printf("--> Início thread selvagem %ld.\n", *rank);
    sem_wait(&mutex);
        printf("Selvagem %ld: Cheguei na panela.\n", *rank);
        if (servings == 0) {
            printf("Selvagem %ld: Não tem comida na panela. Vou avisar o cozinheiro.\n", *rank);
            sem_post(&emptyPot);
            printf("Selvagem %ld: Esperando o cozinheiro encher a panela.\n", *rank);
            sem_wait(&fullPot);
            printf("Selvagem %ld: Panela cheia.\n", *rank);
            servings = MEALS;
        };
        printf("Selvagem %ld: Estou pegando comida na panela.\n", *rank);
        servings -= 1;
        printf("Selvagem %ld: Peguei uma comida na panela. Agora tem %d refeições na panela.\n", *rank, servings);
    sem_post(&mutex);
    printf("Selvagem %ld: Estou comendo.\n", *rank);
}

int main(int argc, char *argv[]) {

    if (MEALS <= 0 || NUM_SAVAGES <= 0) {
        printf("ERRO: a quantidade de MEALS ou SELVAGENS deve ser maior que 0.\n");
        exit(0);
    }

    // ABERTURA THREAD COZINHEIRO
    pthread_t t_cozinheiro;
    int cozinheiro = pthread_create(&t_cozinheiro, NULL, thread_cozinheiro, NULL);
    
    // STARTING SEMAPHORES - [INICIALIZA SEMAFOROS]
    sem_init(&emptyPot, 0, 0);
    sem_init(&fullPot, 0, 0);
    sem_init(&mutex, 0, 1);

    long i, *ids[NUM_SAVAGES];
    pthread_t t_selvagem[NUM_SAVAGES];
    
    // ABERTURA DAS THREADS SELVAGENS
    printf("==> NUM_SAVAGES: %d.\n", NUM_SAVAGES);
    printf("==> MEALS: %d.\n", MEALS);
    for (i = 1; i <= NUM_SAVAGES; i++) {
        ids[i] = malloc(sizeof(long));
        *ids[i] = i;
        pthread_create(&t_selvagem[i], NULL, thread_selvagem, (void*)ids[i]);
        };

    // FECHAMENTO DAS THREADS SELVAGENS
    for (i = 1; i <= NUM_SAVAGES; i++) {
        pthread_join(t_selvagem[i], NULL);
    };

    // FIM DE TODAS AS THREADS
    printf("==> Todas as threads acabaram.\n");

    // DESTROI SEMÁFOROS
    sem_destroy(&emptyPot);
    sem_destroy(&fullPot);
    sem_destroy(&mutex);
    
	return (0);
}
