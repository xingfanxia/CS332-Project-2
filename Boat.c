#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <errno.h>

//counters
int child_0 = 0; //Num of children at Oahu
int child_1 = 0; //Num of children at Molokai
int adult_0 = 0; //Num of adult at Oahu
int adult_1 = 0; //Num of adult at Molokai
int total_0 = 0; //Num of total people at Oahu
int total_1 = 0; //Num of total people at Molokai
int boat = 0; //Boat location, if 0 -> it's at Oahu; if 1 -> it's at Molokai
int child_boat = 0; //num of children on boat
int adult_boat = 0; //num of adult on boat
//mutex lock and cond vars
pthread_mutex_t lock; //lock for critical section
pthread_cond_t waitAt0; //cond var for wait at Oahu
pthread_cond_t waitAt1; //cond var for wait at Molokai
pthread_cond_t boatFull; //cond var for if ship is full
pthread_cond_t boatArrive; //cond var for if ship arrived
pthread_cond_t adultGo;

//sem for main
sem_t* arrive_sem; //sem to signal if boat arrived
sem_t* ready_sem; //sem to signal if boat is ready
sem_t* depart_sem; //sem to signal if boat has departed

void* adult(void*);
void* children(void*);
void initSync();
void closeSync();

int main(int argc, char *argv[]) { 
	int adultsPthread;
	int childrenPthread;
	initSync();

	
	if ( argc != 3 ) /* argc should be 2 for correct execution */
    {
        printf( "2 arguments are needed as input for the number of audlts and children" );
    	fflush(stdout);
    } else {
        adultsPthread = atoi(argv[1]);
        childrenPthread = atoi(argv[2]);
        pthread_t adultsP[adultsPthread];
       	pthread_t childrenP[childrenPthread];

        for( int a = 1; a <= adultsPthread; a = a + 1 ){
	      pthread_t adultsP[a];
	      pthread_create(&adultsP[a], NULL, adult, NULL);
	      //pthread_join(adultsP[a], NULL);
		}

		for( int c = 1; c <= childrenPthread; c = c + 1 ){
	      pthread_t childrenP[c];
	      pthread_create(&childrenP[c], NULL, children, NULL);
	      //pthread_join(childrenP[c], NULL);
		}

		for( int a = 1; a <= adultsPthread; a = a + 1 ){
	      // pthread_t adultsP[a];
	      // pthread_create(&adultsP[a], NULL, adult, NULL);
	      pthread_join(adultsP[a], NULL);
		}

		for( int c = 1; c <= childrenPthread; c = c + 1 ){
	      // pthread_t childrenP[c];
	      // pthread_create(&childrenP[c], NULL, children, NULL);
	      pthread_join(childrenP[c], NULL);
		}

	}

	closeSync();
	return 0;
}

void initSync() {
	pthread_mutex_init(&lock, NULL);
	pthread_cond_init(&waitAt0, NULL);
	pthread_cond_init(&waitAt1, NULL);
	pthread_cond_init(&boatFull, NULL);
	pthread_cond_init(&boatArrive, NULL);
	pthread_cond_init(&adultGo, NULL);
}

void closeSync() {
	pthread_mutex_destroy(&lock);
  	pthread_cond_destroy(&waitAt1);
  	pthread_cond_destroy(&waitAt0);
  	pthread_cond_destroy(&boatFull);
  	pthread_cond_destroy(&boatArrive);
}

void* children (void* args) {
	printf("One Child has showed up at Oahu \n");
	fflush(stdout);
	child_0 ++;
	total_0 ++;
	int childLoc = 0; //intial location of the child
	pthread_mutex_lock(&lock);

	while(1) {
		if (childLoc == 0) {	
			if (child_0 != 1) { //if there is more than 1 child
				printf("Child getting into boat\n");
				fflush(stdout);
				printf("Child getting into boat\n");
				fflush(stdout);
				child_0 = child_0 - 2;
				total_0 = total_0 - 2;
				child_boat =child_boat +2;
				printf("Child start to rowing boat from Oahu to Molokai\n");
				fflush(stdout);
				printf("Child getting off boat and arrive on Molokai\n");
				fflush(stdout);
				child_boat = child_boat -2;
				child_1 = child_1 -2;
				total_1 = total_1 -2;
				pthread_cond_signal(&boatArrive);
				pthread_cond_signal(&waitAt1);
			} else { //there is only 1 child but no sure how many adults
				if (adult_0 == 0) { //if only one Child at Oahu 
				printf("Child getting into boat\n");
				fflush(stdout);
				child_0--;
				total_0--;
				child_boat++;
				printf("Child start to rowing boat from Oahu to Molokai\n");
				fflush(stdout);
				printf("Child getting off boat and arrive on Molokai\n");
				fflush(stdout);
				child_boat--;
				child_1++;
				total_1++;
				pthread_cond_signal(&boatArrive);
				pthread_cond_signal(&waitAt1);
				} else { //if there are adults
					pthread_cond_signal(&adultGo);
				}
			}	
		}
	}	
	pthread_mutex_unlock(&lock);
}


void* adult (void* args) {
	printf("One Adult has showed up at Oahu\n");
	fflush(stdout);
	int adultLoc = 0;
	pthread_mutex_lock(&lock);

	if (adultLoc == 0) {
		
		pthread_cond_wait(&adultGo, &lock);
		printf("Adult getting into boat\n");
		fflush(stdout);
		adult_0--;
		total_0--;
		adult_boat++;
		printf("Adult start to rowing boat from Oahu to Molokai\n");
		fflush(stdout);
		printf("Adult getting off boat and arrive on Molokai\n");
		fflush(stdout);
		adult_boat--;
		adult_1++;
		total_1++;
		pthread_cond_signal(&boatArrive);
		pthread_cond_signal(&waitAt1);
	}
	pthread_mutex_unlock(&lock);
}



















