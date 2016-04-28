
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <time.h>
#include "H2SO4.h"

void* oxygen(void* );
void* hydrogen(void* );
void* sulfur(void* );
void* formMolecule();
void openSems();
void closeSems();
void delay(int);
void shuffle(int*, int);

sem_t* hydro_sem;
sem_t* hydroMutex;
sem_t* sulfur_sem;
sem_t* sulfurMutex;
sem_t* oxy_sem;
sem_t* oxyMutex;
sem_t* mol_sem;

int totOxy;
int totSulfur;
int totHydro;

// int main(int argc, char* argv[]){

// 	closeSems();
// 	if (argc < 4) {
//     	printf("\nmust include num H, S, O atoms as command line arguments (e.g. './a.out 4 2 8')\n\n");
//     	exit(1);
//   	}

//   	openSems();

//   	const int numhydros = atoi(argv[1]);
//   	const int numsulfurs = atoi(argv[2]);
//   	const int numoxys = atoi(argv[3]);
//   	const int total = numoxys+numhydros+numsulfurs; 

//   	srand(time(NULL));

// 	int order[total];
// 	int i;
// 	for (i=0; i<numoxys; i++) {
// 		order[i] = 1;
// 	}
// 	for (; i<numhydros+numoxys; i++) {
// 		order[i] = 2;
// 	}
// 	for (; i<total; i++) {
// 		order[i] = 3;
// 	}

// 	shuffle(order, total);

// 	pthread_t atoms[total];
// 	for(i = 0; i < total; i++){
// 		if(order[i] == 1){
// 			pthread_create(&atoms[i], NULL, oxygen, NULL);
// 		}
// 		else if (order[i] == 2){
// 			pthread_create(&atoms[i], NULL, hydrogen, NULL);
// 		}
// 		else if (order[i] == 3){
// 			pthread_create(&atoms[i], NULL, sulfur, NULL);
// 		}
// 		else{
// 			printf("\nSomething went horribly wrong!!!");
// 		}
// 	}


// 	for(i = 0; i < total; i++){
// 		pthread_join(atoms[i], NULL);
// 	}

// 	closeSems();
// 	printf("\n");
// 	fflush(stdout);
// 	return 0;
// }

void* oxygen(void* args){
	delay(rand()%2000);
	sem_wait(mol_sem);
	sem_wait(oxyMutex);
	totOxy++;
	printf("\noxygen produced");
	fflush(stdout);
	sem_wait(sulfurMutex);
	sem_wait(hydroMutex);
	if(totOxy == 4 && totHydro >= 2 && totSulfur >= 1){
		formMolecule();
		for(int i = 0; i < 3; i++){
			sem_post(oxy_sem);
		}
		sem_post(hydro_sem);
		sem_post(hydro_sem);
		sem_post(sulfur_sem);
		sem_post(oxyMutex);
		sem_post(hydroMutex);
		sem_post(sulfurMutex);
		printf("\noxygen exiting");
		fflush(stdout);
		sem_post(mol_sem);
	}
	else{
		sem_post(oxyMutex);
		sem_post(hydroMutex);
		sem_post(sulfurMutex);
		sem_post(mol_sem);
		sem_wait(oxy_sem);
		printf("\noxygen exiting");
		fflush(stdout);
	}
	return (void*) 0;
}

void* hydrogen(void* args){
	delay(rand()%2000);
	sem_wait(mol_sem);
	sem_wait(hydroMutex);
	totHydro++;
	printf("\nhydrogen produced");
	fflush(stdout);
	sem_wait(sulfurMutex);
	sem_wait(oxyMutex);
	if(totOxy >= 4 && totHydro == 2 && totSulfur >= 1){
		formMolecule();
		for(int i = 0; i < 4; i++){
			sem_post(oxy_sem);
		}
		sem_post(hydro_sem);
		sem_post(sulfur_sem);
		sem_post(oxyMutex);
		sem_post(hydroMutex);
		sem_post(sulfurMutex);
		printf("\nhydrogen exiting");
		fflush(stdout);
		sem_post(mol_sem);
	}
	else{
		sem_post(oxyMutex);
		sem_post(hydroMutex);
		sem_post(sulfurMutex);
		sem_post(mol_sem);
		sem_wait(hydro_sem);
		printf("\nhydrogen exiting");
		fflush(stdout);
	}
	return (void*) 0;
}

void* sulfur(void* args){
	delay(rand()%2000);
	sem_wait(mol_sem);
	sem_wait(sulfurMutex);
	totSulfur++;
	printf("\nsulfur produced ");
	fflush(stdout);
	sem_wait(hydroMutex);
	sem_wait(oxyMutex);
	if(totOxy >= 4 && totHydro >= 2 && totSulfur == 1){
		formMolecule();
		for(int i = 0; i < 4; i++){
			sem_post(oxy_sem);
		}
		sem_post(hydro_sem);
		sem_post(hydro_sem);
		sem_post(oxyMutex);
		sem_post(hydroMutex);
		sem_post(sulfurMutex);
		printf("\nsulfur exiting");
		fflush(stdout);
		sem_post(mol_sem);
	}
	else{
		sem_post(oxyMutex);
		sem_post(hydroMutex);
		sem_post(sulfurMutex);
		sem_post(mol_sem);
		sem_wait(sulfur_sem);
		printf("\nsulfur exiting");
		fflush(stdout);
	}
	return (void*) 0;
}

void* formMolecule(){
	totOxy -= 4;
	totSulfur--;
	totHydro -= 2;
	printf("\n\n***molecule produced***\n");
	fflush(stdout);
	delay(1000);
}

void openSems(){
	hydroMutex = sem_open("hydroMutex", O_CREAT|O_EXCL, 0466, 1);
	sulfurMutex = sem_open("sulfurMutex", O_CREAT|O_EXCL, 0466, 1);
	oxyMutex = sem_open("oxyMutex", O_CREAT|O_EXCL, 0466, 1);
	hydro_sem = sem_open("hydro_sem", O_CREAT|O_EXCL, 0466, 0);
	sulfur_sem = sem_open("sulfur_sem", O_CREAT|O_EXCL, 0466, 0);
	oxy_sem = sem_open("oxy_sem", O_CREAT|O_EXCL, 0466, 0);
	mol_sem = sem_open("mol_sem", O_CREAT|O_EXCL, 0466, 1);

	// while (checkSem(hydroMutex, "hydroMutex") == -1) {
 //    	hydroMutex = sem_open("hydroMutex", O_CREAT|O_EXCL, 0466, 0);
 //  	}
 //  	while (checkSem(sulfurMutex, "sulfurMutex") == -1) {
 //    	sulfurMutex = sem_open("sulfurMutex", O_CREAT|O_EXCL, 0466, 0);
 //  	}
 //  	while (checkSem(oxyMutex, "oxyMutex") == -1) {
 //    	oxyMutex = sem_open("oxyMutex", O_CREAT|O_EXCL, 0466, 0);
 //  	}
 //  	while (checkSem(hydro_sem, "hydro_sem") == -1) {
 //    	hydro_sem = sem_open("hydro_sem", O_CREAT|O_EXCL, 0466, 0);
 //  	}
 //  	while (checkSem(sulfur_sem, "sulfur_sem") == -1) {
 //    	sulfur_sem = sem_open("sulfur_sem", O_CREAT|O_EXCL, 0466, 0);
 //  	}
 //  	while (checkSem(oxy_sem, "oxy_sem") == -1) {
 //    	oxy_sem = sem_open("oxy_sem", O_CREAT|O_EXCL, 0466, 0);
 //  	}
 //  	while (checkSem(mol_sem, "mol_sem") == -1) {
 //    	mol_sem = sem_open("mol_sem", O_CREAT|O_EXCL, 0466, 0);
 //  	}



	// while(hydro_sem==SEM_FAILED){
	// 	if(errno == EEXIST){
	// 		sem_unlink("hydro_sem");
 //      		hydro_sem = sem_open("hydro_sem", O_CREAT|O_EXCL, 0466, 0);
	// 	}
	// }
	// while(sulfur_sem==SEM_FAILED){
	// 	if(errno == EEXIST){
	// 		sem_unlink("sulfur_sem");
 //      		hydro_sem = sem_open("sulfur_sem", O_CREAT|O_EXCL, 0466, 0);
	// 	}
	// }
	// while(oxy_sem==SEM_FAILED){
	// 	if(errno == EEXIST){
	// 		sem_unlink("oxy_sem");
 //      		hydro_sem = sem_open("oxy_sem", O_CREAT|O_EXCL, 0466, 0);
	// 	}
	// }
	// while(mol_sem==SEM_FAILED){
	// 	if(errno == EEXIST){
	// 		sem_unlink("mol_sem");
 //      		hydro_sem = sem_open("mol_sem", O_CREAT|O_EXCL, 0466, 0);
	// 	}
	// }
	// while(hydroMutex==SEM_FAILED){
	// 	if(errno == EEXIST){
	// 		sem_unlink("hydroMutex");
 //      		hydro_sem = sem_open("hydroMutex", O_CREAT|O_EXCL, 0466, 0);
	// 	}
	// }
	// while(oxyMutex==SEM_FAILED){
	// 	if(errno == EEXIST){
	// 		sem_unlink("oxyMutex");
 //      		hydro_sem = sem_open("oxyMutex", O_CREAT|O_EXCL, 0466, 0);
	// 	}
	// }
	// while(sulfurMutex==SEM_FAILED){
	// 	if(errno == EEXIST){
	// 		sem_unlink("sulfurMutex");
 //      		hydro_sem = sem_open("sulfurMutex", O_CREAT|O_EXCL, 0466, 0);
	// 	}
	// }
}

void closeSems(){
	sem_close(hydroMutex);
	sem_close(sulfurMutex);
	sem_close(oxyMutex);
	sem_close(hydro_sem);
	sem_close(sulfur_sem);
	sem_close(oxy_sem);
	sem_close(mol_sem);
	sem_unlink("hydroMutex");
	sem_unlink("sulfurMutex");
	sem_unlink("oxyMutex");
	sem_unlink("hydro_sem");
	sem_unlink("sulfur_sem");
	sem_unlink("oxy_sem");
	sem_unlink("mol_sem");
}

int checkSem(sem_t* sema, char* filename) {
  if (sema==SEM_FAILED) {
    if (errno == EEXIST) {
      printf("semaphore %s already exists, unlinking and reopening\n", filename);
      fflush(stdout);
      sem_unlink(filename);
      return -1;
    }
    else {
      printf("semaphore %s could not be opened, error # %d\n", filename, errno);
      fflush(stdout);
      exit(1);
    }
  }
  return 0;
}

void delay(int limit){
  	int j, k;

    for( j=0; j < limit; j++ )
    {
      for( k=0; k < limit; k++ )
        {
        }
    }
}

// void shuffle(int* intArray, int arrayLen) {
// 	int i=0;
//     for (i=0; i<arrayLen; i++) {
// 	    int r = rand()%arrayLen;
// 	    int temp = intArray[i];
// 	    intArray[i] = intArray[r];
// 	    intArray[r] = temp;
// 	}
// }