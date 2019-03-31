#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <random>
#include <unistd.h>

using namespace std;

sem_t tobacco_and_paper;
sem_t matches_and_paper;
sem_t tobacco_and_matches;
sem_t done_smoking, lock;

void *agent(void *arg);
void *smoker_first(void *arg);
void *smoker_second(void *arg);
void *smoker_third(void *arg);

int main(){
    pthread_t smoker_thread[3];
    pthread_t agent_thread;
    sem_init(&done_smoking, 0, 1);
    sem_init(&lock, 0, 1);
    sem_init(&tobacco_and_matches, 0, 0);
    sem_init(&matches_and_paper, 0, 0);
    sem_init(&tobacco_and_paper, 0, 0);

    pthread_create(&agent_thread, NULL, agent, NULL);
    pthread_create(&smoker_thread[0], NULL, smoker_first, NULL);
    pthread_create(&smoker_thread[1], NULL, smoker_second, NULL);
    pthread_create(&smoker_thread[0], NULL, smoker_third, NULL);

    pthread_exit(NULL);
}

void *agent(void *arg){
    while(true){
        sem_wait(&lock);
        int random_number = rand()%3 + 1;
        switch (random_number)
        {
            case 1:
                sem_post(&tobacco_and_matches);
                break;
            case 2:
                sem_post(&tobacco_and_paper);
                break;
            case 3:
                sem_post(&matches_and_paper);
                break;
            default:
                break;
        }
        sem_post(&lock);
        sem_wait(&done_smoking);
    }
}

void *smoker_first(void *arg){
    while(true){
        sem_wait(&tobacco_and_matches);
        sem_wait(&lock);
        cout << "Smoker first picks up matches." << endl;
        cout << "Smoker first smokes" << endl;
        sem_post(&done_smoking);
        sem_post(&lock);
    }
}

void *smoker_second(void *arg){
    while(true){
        sem_wait(&tobacco_and_paper);
        sem_wait(&lock);
        cout << "Smoker second picks up matches." << endl;
        cout << "Smoker second smokes" << endl;
        sem_post(&done_smoking);
        sem_post(&lock);
    }
}

void *smoker_third(void *arg){
        while(true){
        sem_wait(&matches_and_paper);
        sem_wait(&lock);
        cout << "Smoker third picks up matches." << endl;
        cout << "Smoker third smokes." << endl;
        sem_post(&done_smoking);
        sem_post(&lock);
    }
}