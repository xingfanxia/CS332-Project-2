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
int childOffBoat = 0; //Child got off boat
int child_Waitboat = 0; //num of children waiting for boat
int boatAt0 = 1;
//mutex lock and cond vars
//lock one island so no concurrent access is happening
pthread_mutex_t lock0; //lock for critical section, lock Oahu
pthread_mutex_t lock1; //lock for critical section, lock Molokai

pthread_cond_t adult0; //cond var for adults; if 2+ children at Oahu then wait

//cond var for children
pthread_cond_t cWaitAt1; //wait at Molokai if not the child boating back
pthread_cond_t cWaitAt0; //wait at A if there are 2+ children to go across the river
pthread_cond_t cWaitToBow0; //wait for a second child to go with to go across the river

//sem for main
sem_t* finish_sem; //sem to signal if boat arrived
sem_t* startThread_sem;
sem_t* waitThread_sem;

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
	      sem_wait(waitThread_sem);
		}

		for( int c = 1; c <= childrenPthread; c = c + 1 ){
	      sem_wait(waitThread_sem);
		}

		
		for( int a = 1; a <= adultsPthread; a = a + 1 ){
	      sem_post(startThread_sem);
		}

		for( int c = 1; c <= childrenPthread; c = c + 1 ){
	      sem_post(startThread_sem);
		}
		sem_wait(finish_sem);

		for( int a = 1; a <= adultsPthread; a = a + 1 ){
	      
	      pthread_join(adultsP[a], NULL);
		}

		for( int c = 1; c <= childrenPthread; c = c + 1 ){
	      pthread_join(childrenP[c], NULL);
		}

	}

	
	
	closeSync();
	return 0;
}

void initSync() {
	pthread_mutex_init(&lock0, NULL);
	pthread_mutex_init(&lock1, NULL);
	pthread_cond_init(&adult0, NULL);
	pthread_cond_init(&cWaitAt0, NULL);
	pthread_cond_init(&cWaitAt1, NULL);
	pthread_cond_init(&cWaitToBow0, NULL);


	finish_sem = sem_open("finishSem", O_CREAT|O_EXCL, 0466, 0);

	startThread_sem = sem_open("startSem", O_CREAT|O_EXCL, 0466, 0);

	waitThread_sem = sem_open("waitSem", O_CREAT|O_EXCL, 0466, 0);

	while (finish_sem==SEM_FAILED) {
	    if (errno == EEXIST) {
	      printf("semaphore finishSem already exists, unlinking and reopening\n");
	      fflush(stdout);
	      sem_unlink("finishSem");
	      finish_sem = sem_open("finishSem", O_CREAT|O_EXCL, 0466, 0);
	    }
	    else {
	      printf("semaphore could not be opened, error # %d\n", errno);
	      fflush(stdout);
	      exit(1);
	    }
	}

    while (startThread_sem==SEM_FAILED) {
	    if (errno == EEXIST) {
	      printf("semaphore startSem already exists, unlinking and reopening\n");
	      fflush(stdout);
	      sem_unlink("startSem");
	      startThread_sem = sem_open("startSem", O_CREAT|O_EXCL, 0466, 0);
	    }
	    else {
	      printf("semaphore could not be opened, error # %d\n", errno);
	      fflush(stdout);
	      exit(1);
	    }
	}

    while (waitThread_sem==SEM_FAILED) {
	    if (errno == EEXIST) {
	      printf("semaphore waitSem already exists, unlinking and reopening\n");
	      fflush(stdout);
	      sem_unlink("waitSem");
	      waitThread_sem = sem_open("waitSem", O_CREAT|O_EXCL, 0466, 0);
	    }
	    else {
	      printf("semaphore could not be opened, error # %d\n", errno);
	      fflush(stdout);
	      exit(1);
	    }
	}
}

void closeSync() {
	pthread_mutex_destroy(&lock0);
	pthread_mutex_destroy(&lock1);
	pthread_cond_destroy(&adult0);
	pthread_cond_destroy(&cWaitAt0);
	pthread_cond_destroy(&cWaitAt1);
	pthread_cond_destroy(&cWaitToBow0);

  	sem_close(finish_sem);
 	sem_unlink("finishSem");

 	sem_close(waitThread_sem);
 	sem_unlink("waitSem");

 	sem_close(startThread_sem);
 	sem_unlink("startSem");
}

void* children (void* args) {
	printf("One Child has showed up at Oahu\n");
	fflush(stdout);
	child_0++;

	sem_post(waitThread_sem);
	sem_wait(startThread_sem);
	while (child_0 + adult_0 > 1) {
		//while there are people still on the island
		pthread_mutex_lock(&lock0);

		if (child_0 == 1) {
			//we can only boat children if there are two of them, but otherwise, then an adult should go
			pthread_cond_signal(&adult0);
		}

		while (child_Waitboat >=2 || boatAt0 == 0) {
			//there are enough children to boat over to B
			//let the old child(child on the island earlier) go first 
			pthread_cond_wait(&cWaitAt0, &lock0);
		}

		if (child_Waitboat == 0) {
			child_Waitboat++;
			//wake up someone to boat with
			pthread_cond_signal(&cWaitAt0);
			//wait for child to go across with
			pthread_cond_wait(&cWaitToBow0, &lock0);
			printf("One child is now rowing from Oahu to Molokai\n");
			//arrived, go to sleep
			pthread_cond_signal(&cWaitToBow0);
		} else { //there is already someone ready to bow
			child_Waitboat++;
			pthread_cond_signal(&cWaitToBow0); //get one more to go
			printf("One child is now rowing from Oahu to Molokai\n");
			pthread_cond_signal(&cWaitToBow0); //arrived, go to sleep
		}

		//finally left Oahu
		child_Waitboat--;
		child_0--;
		boatAt0 = 0;
		//bye Oahu
		pthread_mutex_unlock(&lock0);
		//Get to Molokai
		pthread_mutex_lock(&lock1);
		child_1++;
		childOffBoat++;
		printf("One child has got out of the boat and arrived at Molokai!!! Oh Yeah\n");

		if (childOffBoat == 1) {
			//only one child from the 2 sleeps
			pthread_cond_wait(&cWaitAt1, &lock1);
		}

		child_1--; //always one child has to go back to A 
		childOffBoat = 0;
		pthread_mutex_unlock(&lock1);
		printf("One child is now rowing from Molokai to Oahu\n");
		pthread_mutex_lock(&lock0);
		child_0++;
		boatAt0 = 1;
		pthread_mutex_unlock(&lock0);
	}
	
	//check if anyone on Oahu anymore
	pthread_mutex_lock(&lock0);
	child_0--;
	pthread_mutex_unlock(&lock0);
	printf("One child is now rowing from Oahu to Molokai\n");
	pthread_mutex_lock(&lock1);
	child_1++;
	pthread_mutex_unlock(&lock1);

	printf("Children A/B: %d / %d\n", child_0, child_1);
	printf("Adults A/B: %d / %d\n", adult_0, adult_1);
	sem_post(finish_sem);
}


void* adult (void* args) {
	printf("One Adult has showed up at Oahu\n");
	adult_0++;
	fflush(stdout);
	sem_post(waitThread_sem);
	sem_wait(startThread_sem);
	pthread_mutex_lock(&lock0);
	while(child_0 >1 || boatAt0 == 0) {
		//if there is more than one child, then the children should go first, so go to sleep
		pthread_cond_wait(&adult0, &lock0);
	}
	//otherwise adults can go now
	adult_0--;
	boatAt0 = 0;
	pthread_mutex_unlock(&lock0);
	printf("One Adult is now rowing from Oahu to Molokai\n");
	pthread_mutex_lock(&lock1);
	adult_1++;
	printf("One Adult has got out of the boat and arrived at Molokai!!! Oh Yeah\n");
	pthread_cond_signal(&cWaitAt1);
	pthread_mutex_unlock(&lock1);
}