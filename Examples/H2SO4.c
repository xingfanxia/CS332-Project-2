#include "H2SO4.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

void* oxygen(void*);
void* hydrogen(void*);
void* sulfur(void*);
void openSems();
void closeSems();
void delay(int);



// declare hydrogen semaphore as global variable
sem_t* hydro_sem;
sem_t* s_sem;
sem_t* h2so4_sem;
sem_t* mutex;

void openSems() {
  hydro_sem = sem_open("hydrosmphr", O_CREAT|O_EXCL, 0466, 0);
  h2so4_sem = sem_open("h2so4sem", O_CREAT|O_EXCL, 0466, 0);
  s_sem = sem_open("sSem", O_CREAT|O_EXCL, 0466, 0);
}

void closeSems() {
  sem_close(hydro_sem);
  sem_unlink("hydrosmphr");

  sem_close(h2so4_sem);
  sem_unlink("h2so4sem");

  sem_close(s_sem);
  sem_unlink("sSem");
  
}
void* oxygen(void* args) {
  // produce an oxygen molecule - takes some (small) random amount of time 
  delay(rand()%2000);
  printf("oxygen produced\n");
  fflush(stdout);

  // oxygen waits (calls down) twice on the hydrogen semaphore
  // meaning it cannot continue until at least 2 hydrogen atoms
  // have been produced
  int err = sem_wait(hydro_sem);
  int err2 = sem_wait(hydro_sem);
  int err3 = sem_wait(s_sem);
  if (err==-1 || err2==-1 || err3 == -1) printf("error on oxygen wait for hydro_sem, error # %d\n", errno);
  
  // produce a water molecule
  printf("made H2SO4\n");
  
  fflush(stdout);


  // oxygen exits

  printf("oxygen leaving\n");
  fflush(stdout);
  sem_post(h2so4_sem);
  return (void*) 0;
}

void* hydrogen(void* args) {
  // produce a hydrogen molecule - takes some (small) random amount of time 
  delay(rand()%3000);
  printf("hydrogen produced\n");
  fflush(stdout);
  
  // post (call up) on hydrogen semaphore to signal that a hydrogen atom
  // has been produced
  sem_post(hydro_sem);

  int err4 = sem_wait(h2so4_sem);
  // hydrogen exits
  // sem_wait(h2so4_sem);
  printf("hydrogen leaving\n");
  // printf("hydrogen leaving\n");
  fflush(stdout);
  
  return (void*) 0;
}

void* sulfur(void* args) {
  // produce a hydrogen molecule - takes some (small) random amount of time 
  delay(rand()%3000);
  printf("sulfur produced\n");
  fflush(stdout);
  
  // post (call up) on hydrogen semaphore to signal that a hydrogen atom
  // has been produced
  sem_post(s_sem);

  int err4 = sem_wait(h2so4_sem);
  // hydrogen exits
  // sem_wait(h2so4_sem);
  printf("sulfur leaving\n");

  fflush(stdout);
  
  return (void*) 0;
}
/*
 * NOP function to simply use up CPU time
 * arg limit is number of times to run each loop, so runs limit^2 total loops
 */
void delay( int limit )
{
  int j, k;

  for( j=0; j < limit; j++ )
    {
      for( k=0; k < limit; k++ )
        {
        }
    }
}

int main() {

  // create the hydrogen semaphore, very important to use last 3 arguments as shown here
  // first argument is simply filename for semaphore, any name is fine but must be a valid path
  openSems();

  // deal with possibility that previous run of program created semaphore with same name and
  // didn't properly close & unlink for whatever reason
  while (hydro_sem==SEM_FAILED) {
    if (errno == EEXIST) {
      printf("semaphore hydrosmphr already exists, unlinking and reopening\n");
      fflush(stdout);
      sem_unlink("hydrosmphr");
      hydro_sem = sem_open("hydrosmphr", O_CREAT|O_EXCL, 0466, 0);
    }
    else {
      printf("semaphore could not be opened, error # %d\n", errno);
      fflush(stdout);
      exit(1);
    }
  }

  while (s_sem==SEM_FAILED) {
    if (errno == EEXIST) {
      printf("semaphore sSem already exists, unlinking and reopening\n");
      fflush(stdout);
      sem_unlink("sSem");
      s_sem = sem_open("sSem", O_CREAT|O_EXCL, 0466, 0);
    }
    else {
      printf("semaphore could not be opened, error # %d\n", errno);
      fflush(stdout);
      exit(1);
    }
  }

  while (h2so4_sem==SEM_FAILED) {
    if (errno == EEXIST) {
      printf("semaphore h2so4sem already exists, unlinking and reopening\n");
      fflush(stdout);
      sem_unlink("h2so4sem");
      h2so4_sem = sem_open("h2so4sem", O_CREAT|O_EXCL, 0466, 0);
    }
    else {
      printf("semaphore could not be opened, error # %d\n", errno);
      fflush(stdout);
      exit(1);
    }
  }

  // testing water molecule creation, 2nd molecule should not be made until all 4 hydrogen
  // and 2 oxygen atoms have been produced
  pthread_t oxy1, oxy2, oxy3, oxy4, oxy5, oxy6, oxy7, oxy8, hydro1, hydro2, hydro3, hydro4, sulfur1, sulfur2;
  pthread_create(&hydro1, NULL, hydrogen, NULL);
  pthread_create(&hydro2, NULL, hydrogen, NULL);
  pthread_create(&hydro3, NULL, hydrogen, NULL);
  pthread_create(&hydro4, NULL, hydrogen, NULL);
  pthread_create(&oxy1, NULL, oxygen, NULL);
  pthread_create(&oxy2, NULL, oxygen, NULL);
  pthread_create(&oxy3, NULL, oxygen, NULL);
  pthread_create(&oxy4, NULL, oxygen, NULL);
  pthread_create(&oxy5, NULL, oxygen, NULL);
  pthread_create(&oxy6, NULL, oxygen, NULL);
  pthread_create(&oxy7, NULL, oxygen, NULL);
  pthread_create(&oxy8, NULL, oxygen, NULL);
  pthread_create(&sulfur1, NULL, sulfur, NULL);
  pthread_create(&sulfur2, NULL, sulfur, NULL);


  pthread_join(hydro1, NULL);
  pthread_join(hydro2, NULL);
  pthread_join(hydro3, NULL);
  pthread_join(hydro4, NULL);
  pthread_join(oxy1, NULL);
  pthread_join(oxy2, NULL);
  pthread_join(oxy3, NULL);
  pthread_join(oxy4, NULL);
  pthread_join(oxy5, NULL);
  pthread_join(oxy6, NULL);
  pthread_join(oxy7, NULL);
  pthread_join(oxy8, NULL);
  pthread_join(sulfur1, NULL);
  pthread_join(sulfur2, NULL);
  
  closeSems();
  return 0;
}
