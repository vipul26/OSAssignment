/* This file contains the reader writer solution
 * Three semaphores are used to produce a starve free solution
 * A file test.txt(empty) need to be generated before the execution.
 */

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fstream>

using namespace std;

// Number of readers count declaration
int read_count;

// Semaphores declaration
// resource_access semaphore will be used to provide access to the resource i.e. the file.
sem_t resource_access;
// resource_count_access preserves changes to shared variable read_count
sem_t resource_count_access;
// service_queue preserves the order of requests i.e. FIFO
sem_t service_queue;

// Reader declaration
void *reader(void *arg);

// Writer declaration
void *writer(void *arg);

// Shared file declaration
fstream file;

int main(){
	// Reader Thread declaration
	pthread_t reader_thread[10];
	// Writer Thread declaration
       	pthread_t writer_thread[10];

	// initialization of semaphores to 1
	sem_init(&resource_access, 0, 1);
	sem_init(&resource_count_access, 0, 1);
	sem_init(&service_queue, 0, 1);

	// creation of reader and writer thread
	for(int i=0; i<3; i++)
		pthread_create(&reader_thread[i], NULL, reader, NULL);
	
	for(int i=0; i<3; i++)
		pthread_create(&writer_thread[i], NULL, writer, (void *)(intptr_t)i);
	
	for(int i=3; i<6; i++){
		pthread_create(&reader_thread[i], NULL, reader, NULL);
		pthread_create(&writer_thread[i], NULL, writer, (void *)(intptr_t)i);
	}
	
	for(int i=6; i<8; i++)
		pthread_create(&writer_thread[i], NULL, writer, (void *)(intptr_t)i);
	
	for(int i=6; i<8; i++)
		pthread_create(&reader_thread[i], NULL, reader, NULL);
	
	for(int i=8; i<10; i++){
		pthread_create(&writer_thread[i], NULL, writer, (void *)(intptr_t)i);
		pthread_create(&reader_thread[i], NULL, reader, NULL);
	}
	pthread_exit(NULL);
}

void *reader(void *arg){
	sem_wait(&service_queue);
	sem_wait(&resource_count_access);
	if(read_count == 0)
		sem_wait(&resource_access);
	read_count++;
	sem_post(&service_queue);
	sem_post(&resource_count_access);
	string line;
	file.open("test.txt", ios::in);
	if(file.is_open()){
		cout << "Read File begin : " << endl;
		while(getline(file, line))
    	{
      		cout << line << endl;
   		}
    	file.close();
		cout << "Read File End" << endl;
	}else{
		cout << "Unable to read file." << endl;
	}
	sem_wait(&resource_count_access);
    read_count--;                // update count of active readers
    if (read_count == 0)         // if there are no readers left:
        sem_post(&resource_access);     // release resource access for all
    sem_post(&resource_count_access);
}

void *writer(void *arg){
	sem_wait(&service_queue);
	sem_wait(&resource_access);
	sem_post(&service_queue);
	// Open a file for writing
	file.open("test.txt", ios::app);
	int data = ((intptr_t)arg);
	cout << "Data Write Begin : " << endl;
	file << data << endl;
	file.close();
	cout << "Data Write End" << endl;
	sem_post(&resource_access);
}