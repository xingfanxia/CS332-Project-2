#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

//counters
int child_0 = 0; //Num of children at Oahu
int child_1 = 0; //Num of children at Molokai
int adult_0 = 0; //Num of adult at Oahu
int adult_1 = 0; //Num of adult at Molokai
int total_0 = 0; //Num of total people at Oahu
int total_1 = 0; //Num of total people at Molokai
int boat = 0; //Boat location, if 0 -> it's at Oahu; if 1 -> it's at Molokai

pthread_mutex_t boatFull;
pthread_mutex_t lock;
pthread_mutex_t waitAt0; //wait at Oahu
pthread_mutex_t waitAt1; //wait at Molokai


void* adult(void*);
void* children(void*);
void initSync();
void closeSync();

void initSync() {
	pthread_mutex_init(&adultsC_mutex, NULL);
	pthread_mutex_init(&childrenC_mutex, NULL);

	pthread_cond_init(&adultsOnBoard, NULL);
	pthread_cond_init(&childrenOnBoard, NULL);
}

void* adult (void* args) {
	if (Boh)
}

void* children (void* args) {
	
}

int main(int argc, int *argvp[]) { 
	int adultsPthread;
	int childrenPthread;
	if ( argc != 3 ) /* argc should be 2 for correct execution */
    {
        printf( "2 arguments are needed as input for the number of audlts and children" );
    }
    else 
    {
        adultsPthread = argv[1]
        childrenPthread = argv[2]

        for( a = 1; a <= argv[1]; a = a + 1 ){
	      pthread_t adultsP[a];
	      pthread_create(&adultsP[a], NULL, adult, NULL);
	      pthread_join(adultsP[a], NUll);
		}

		for( c = 1; c <= argv[2]; c = c + 1 ){
	      pthread_t adultsP[c];
	      pthread_create(&adultsP[c], NULL, adult, NULL);
	      pthread_join(adultsP[c], NUll);
		}
	}
}