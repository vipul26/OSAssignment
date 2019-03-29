/* Solution for the producer consumer problem using semaphores.
 * The solution uses two semaphores full and empty, and a mutex.
 * The buffersize can be changed by using BUFFER_SIZE constant defined.
 * The code for producer is written in function producer and code for consumer in function consumer.
 * The main defines the execution of producer and consumer and initializes the semaphores and mutex used.
 * Item defined here is a class, having integer values, which will be assigned a random number whenever producer produces an item.
 */

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <random>
#include <unistd.h>

using namespace std;

const int BUFFER_SIZE = 1000;

//Item defined as a class, and each Item object has an item_no initially defined as 0 
//This will be incremented on every production.
class Item{
	private:
		//Current number of item produced.
		static int item_no;
		//identity of every item
		int item_id;
		//value given to a particular item
		int val;
		void generate_item_id(){
			item_id = item_no;
		}
		static void increment_item_no(){
			item_no++;
		}
	public:
		Item(int val){
			this->val = val;
			generate_item_id();
			increment_item_no();
		}
		int get_item_value(){
			return val;
		}
		int get_item_id(){
			return item_id;
		}
};

int Item::item_no = 1;
int index = 0;

//created a buffer for consumer to put items
Item *buffer[BUFFER_SIZE];

pthread_mutex_t mtx; //mutex variable declared using pthread library
sem_t full, empty; // two semaphore variable defined 

void *producer(void *arg);
void *consumer(void *arg);

int main(){
	//producer and consumer threads.
	pthread_t prod, cons;
	//initialization of semaphores
	sem_init(&empty, 0, BUFFER_SIZE);
	sem_init(&full, 0, 0);
	//initialization of mutex incase of multiple producer, consumer scenario
	pthread_mutex_init(&mtx, NULL);
	//creation of producer and consumer threads
	pthread_create(&prod, NULL, producer, NULL);
	pthread_create(&cons, NULL, consumer, NULL);
	pthread_exit(NULL);
}

void *producer(void *arg){
	while(true){
		// random waiting, producer can produce the item after 0s, 1s, or 2s sleep.
		sleep(rand()%3);
		sem_wait(&empty);
		pthread_mutex_lock(&mtx);
		//creation of item in producer
		Item* item = new Item(rand());
		buffer[index++] = item;
		cout << "Producer produced : " << item->get_item_value() << endl;
	       	cout << "With Item id :" << item->get_item_id() << endl;
		pthread_mutex_unlock(&mtx);
		sem_post(&full);
	}
}

void *consumer(void *arg){
	while(true){
		// random waiting, consumer can consume the item after 0s, 1s, or 2s sleep.
		sleep(rand()%3);
		sem_wait(&full);
		pthread_mutex_lock(&mtx);
		//retrieval of item in consumer
		Item* consumed_item = buffer[--index];
		cout << "Consumer Consumed : " << consumed_item->get_item_value() << endl;
	       	cout << "With Item id :" << consumed_item->get_item_id() << endl;
		pthread_mutex_unlock(&mtx);
		sem_post(&empty);
	}
}
