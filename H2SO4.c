
// @Authors: Aman Panda and Xingfan Xia

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <time.h>
#include "H2SO4.h"

// Function declarations
void* oxygen(void* );
void* hydrogen(void* );
void* sulfur(void* );
void* formMolecule();
void openSems();
void closeSems();
void delay(int);
void shuffle(int*, int);

// Global semaphores and mutex semaohores
sem_t* hydro_sem;	// Getting hydrogen atoms to wait until ready for molecule formation.
sem_t* hydroMutex;	// Mutex sem to control access to totHydro
sem_t* sulfur_sem;	// Getting sulfur atoms to wait until ready for molecule formation
sem_t* sulfurMutex;	// Mutex sem to control access to totSulfur
sem_t* oxy_sem;		// Getting oxygen atoms to wait until ready for molecule formation.
sem_t* oxyMutex;	// Mutex sem to control access to totOxy
sem_t* mol_sem;		// Mutex sem to control exiting of atoms

// Global variables to hold number of atoms of each type created so far
int totOxy;
int totSulfur;
int totHydro;

// Function to create an oxygen atom
void* oxygen(void* args){

	// Small delay before atom formation.
	delay(rand()%2000);

	// Only form if atoms aren't currently exiting
	sem_wait(mol_sem);
	// Get lock to increment counter
	sem_wait(oxyMutex);
	totOxy++;
	//Indicate atom creation
	printf("\noxygen produced");
	fflush(stdout);

	// Get other locks to test if ready for molecule creation.
	sem_wait(sulfurMutex);
	sem_wait(hydroMutex);
	if(totOxy == 4 && totHydro >= 2 && totSulfur >= 1){
		// Create an H2SO4 molecule
		formMolecule();

		// Release necessary amount of atoms required in forming molecule
		for(int i = 0; i < 3; i++){
			sem_post(oxy_sem);
		}
		sem_post(hydro_sem);
		sem_post(hydro_sem);
		sem_post(sulfur_sem);

		// Let go of locks used to test for molecule creation.
		sem_post(oxyMutex);
		sem_post(hydroMutex);

		// Indicate that this atoms is leaving
		sem_post(sulfurMutex);
		printf("\noxygen exiting");
		fflush(stdout);

		// Another atoms may be created now
		sem_post(mol_sem);
	}
	else{
		// Let go of locks used to test for molecule creation
		sem_post(oxyMutex);
		sem_post(hydroMutex);
		sem_post(sulfurMutex);

		// Another atom may be created now
		sem_post(mol_sem);

		// Wait if not ready yet for molecule creation.
		sem_wait(oxy_sem);

		// Indicate that the atom is exiting
		printf("\noxygen exiting");
		fflush(stdout);
	}
	return (void*) 0;
}

// Function to create a hydrogen atom
void* hydrogen(void* args){

	// Small delay before atom formation.
	delay(rand()%2000);

	// Only form if atoms aren't currently exiting
	sem_wait(mol_sem);

	// Get lock to increment counter
	sem_wait(hydroMutex);
	totHydro++;

	// Indicate that an atom has been produced.
	printf("\nhydrogen produced");
	fflush(stdout);

	// Get locks to test if molecule creation may happen
	sem_wait(sulfurMutex);
	sem_wait(oxyMutex);
	if(totOxy >= 4 && totHydro == 2 && totSulfur >= 1){

		// Form a molecule of H2SO4
		formMolecule();

		// Release necessary amount of atoms used in molecule creation
		for(int i = 0; i < 4; i++){
			sem_post(oxy_sem);
		}
		sem_post(hydro_sem);
		sem_post(sulfur_sem);

		// Let go of the locks used in checking if ready for molecule creation
		sem_post(oxyMutex);
		sem_post(hydroMutex);
		sem_post(sulfurMutex);

		// Indicate that this atom will be exiting
		printf("\nhydrogen exiting");
		fflush(stdout);

		// Another atom may form now
		sem_post(mol_sem);
	}
	else{
		// Let go of locks used in checking if ready to form molecule
		sem_post(oxyMutex);
		sem_post(hydroMutex);
		sem_post(sulfurMutex);

		// Another atom may form now
		sem_post(mol_sem);

		// Wait if molecule formation is not ready yet
		sem_wait(hydro_sem);

		// Indicate that this atom will be exiting
		printf("\nhydrogen exiting");
		fflush(stdout);
	}
	return (void*) 0;
}

// Function to create a sulfur atom
void* sulfur(void* args){

	// Random delay on formation of atom.
	delay(rand()%2000);

	// Wait if a certain atom is currently exiting
	sem_wait(mol_sem);

	// Get the lock necessary to increment couter
	sem_wait(sulfurMutex);
	totSulfur++;

	// Indicate that an atom of sulfur has been created.
	printf("\nsulfur produced ");
	fflush(stdout);

	// Get locks necessary for checking if molecule creation is ready
	sem_wait(hydroMutex);
	sem_wait(oxyMutex);
	if(totOxy >= 4 && totHydro >= 2 && totSulfur == 1){

		// Actually form the molecule
		formMolecule();

		// Release necessary number of atoms that were previously waiting for molecule creation
		for(int i = 0; i < 4; i++){
			sem_post(oxy_sem);
		}
		sem_post(hydro_sem);
		sem_post(hydro_sem);

		// Let go of locks used for checking if molecule creation is ready
		sem_post(oxyMutex);
		sem_post(hydroMutex);
		sem_post(sulfurMutex);

		// Indicate that this atom is now going to exit
		printf("\nsulfur exiting");
		fflush(stdout);

		// Another atom may form nw
		sem_post(mol_sem);
	}
	else{

		// Let go of locks used in checking if molecule creation may happen
		sem_post(oxyMutex);
		sem_post(hydroMutex);
		sem_post(sulfurMutex);

		// Another atom may form now
		sem_post(mol_sem);

		// Wait until molecule creation happens
		sem_wait(sulfur_sem);

		// Indicate that this atom is going to exit now
		printf("\nsulfur exiting");
		fflush(stdout);
	}
	return (void*) 0;
}

void* formMolecule(){

	// Adjust counters post-Molecule creation
	totOxy -= 4;
	totSulfur--;
	totHydro -= 2;

	// Indicate that a molecule has been formed
	printf("\n\n***molecule produced***\n");
	fflush(stdout);

	// Delay to allow atom threads to exit before new atom threads can be formed.
	delay(1000);
}

// Function to open all semaphores
void openSems(){

	// Open the 7 different semaphores used in the problem
	hydroMutex = sem_open("hydroMutex", O_CREAT|O_EXCL, 0466, 1);
	sulfurMutex = sem_open("sulfurMutex", O_CREAT|O_EXCL, 0466, 1);
	oxyMutex = sem_open("oxyMutex", O_CREAT|O_EXCL, 0466, 1);
	hydro_sem = sem_open("hydro_sem", O_CREAT|O_EXCL, 0466, 0);
	sulfur_sem = sem_open("sulfur_sem", O_CREAT|O_EXCL, 0466, 0);
	oxy_sem = sem_open("oxy_sem", O_CREAT|O_EXCL, 0466, 0);
	mol_sem = sem_open("mol_sem", O_CREAT|O_EXCL, 0466, 1);

	// Code previously used to fix broken semaphores

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

// Function to close all semaphores at the end of the program
void closeSems(){
	//Close semapahores
	sem_close(hydroMutex);
	sem_close(sulfurMutex);
	sem_close(oxyMutex);
	sem_close(hydro_sem);
	sem_close(sulfur_sem);
	sem_close(oxy_sem);
	sem_close(mol_sem);
	//Unlink semaphore
	sem_unlink("hydroMutex");
	sem_unlink("sulfurMutex");
	sem_unlink("oxyMutex");
	sem_unlink("hydro_sem");
	sem_unlink("sulfur_sem");
	sem_unlink("oxy_sem");
	sem_unlink("mol_sem");
}

// Function provided by Sherri to fix semaphores
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

// Nested for loop used to have delays in the programs.
void delay(int limit){
  	int j, k;

    for( j=0; j < limit; j++ )
    {
      for( k=0; k < limit; k++ )
        {
        }
    }
}