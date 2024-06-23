#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#define CHAIRS 5

sem_t waitingRoom;
sem_t barberChair;
sem_t barberSleep;
sem_t seatBelt;
int allDone = 0;

void *barber(void *arg) {
    while (!allDone) {
        sem_wait(&barberSleep); // Sleep until a customer arrives
        if (!allDone) {
            printf("Barber is cutting hair\n");
            sleep(2); // Cutting hair
            printf("Barber has finished cutting hair\n");
            sem_post(&seatBelt); // Release the customer
        }
    }
    return NULL;
}

void *customer(void *arg) {
    sem_wait(&waitingRoom); // Try to enter waiting room
    printf("Customer entering waiting room\n");
    sem_wait(&barberChair); // Try to sit in barber chair
    
    sem_post(&waitingRoom); // Leave waiting room
    printf("Customer waking the barber\n");
    sem_post(&barberSleep); // Wake the barber
    
    sem_wait(&seatBelt); // Wait for the haircut to finish
    sem_post(&barberChair); // Leave the barber chair
    printf("Customer leaving barber shop\n");
    return NULL;
}

int main() {
    pthread_t btid, ctid[CHAIRS];
    sem_init(&waitingRoom, 0, CHAIRS);
    sem_init(&barberChair, 0, 1);
    sem_init(&barberSleep, 0, 0);
    sem_init(&seatBelt, 0, 0);
    
    pthread_create(&btid, NULL, barber, NULL);
    for (int i = 0; i < CHAIRS; i++) {
        pthread_create(&ctid[i], NULL, customer, NULL);
        sleep(1); // Staggered arrival of customers
    }
    
    for (int i = 0; i < CHAIRS; i++) {
        pthread_join(ctid[i], NULL);
    }
    
    allDone = 1;
    sem_post(&barberSleep); // Wake the barber one last time to finish
    pthread_join(btid, NULL);
    
    sem_destroy(&waitingRoom);
    sem_destroy(&barberChair);
    sem_destroy(&barberSleep);
    sem_destroy(&seatBelt);
    
    return 0;
}
