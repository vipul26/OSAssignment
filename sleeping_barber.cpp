#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <random>
#include <unistd.h>

using namespace std;

const int SEATS = 100;

sem_t barber_ready;
sem_t access_seats;
sem_t access_ready_customer;
int free_seats = SEATS;

void *barber(void *arg);
void *customer(void *arg);

int main(){
    pthread_t barber_thread;
    pthread_t customers_thread;

    sem_init(&barber_ready, 0, 0);
    sem_init(&access_seats, 0, 1);
    sem_init(&access_ready_customer, 0, 0);

    pthread_create(&barber_thread, NULL, barber, NULL);
    pthread_create(&customers_thread, NULL, customer, NULL);

    pthread_exit(NULL);
}

void *barber(void *arg){
    while(true){
        sem_wait(&access_ready_customer);
        sem_wait(&access_seats);     // Awake - try to get access to modify # of available seats, otherwise sleep.
        free_seats++;    // One waiting room chair becomes free.
        sem_post(&barber_ready);        // I am ready to cut.
        sem_post(&access_seats);       // Don't need the lock on the chairs anymore.
        cout << "Barber Cuts Customer Hair" << endl;
        sleep(rand()%3);
    }
}

void *customer(void *arg){
    while(true){
        sem_wait(&access_seats);         // Try to get access to the waiting room chairs.
        if(free_seats > 0){
            free_seats--;               //   sit down in a chair
            sem_post(&access_ready_customer);     //  notify the barber, who's waiting until there is a customer
            sem_post(&access_seats);     //  don't need to lock the chairs anymore
            sem_wait(&barber_ready);
            cout << "Customer gets his hair Cut." << endl;
        }         
        else{                  // otherwise, there are no free seats
            sem_post(&access_seats);
            cout << "Customer Returns Back" << endl;
        }
        sleep(rand()%4);
    }
}