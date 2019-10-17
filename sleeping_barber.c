/* Stacy Day
 * comp322l
 * lab4
 * This lab is the sleeping barber program with 10 customers
 * 3 seats and 1 barber using 2 semaphores, a mutex and a pipe. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>

#include <errno.h>
#include <sys/ipc.h>

// define max number customers
#define MAX_CUSTOMERS 10

// define max number seats
#define MAX_SEATS 3

// how many customers are waiting
// in waiting room seats for haircuts.
// starts out with 0 waiting
int count = 0;

// define semaphores & mutex
// define customer semaphore
sem_t customer;

// define barber semaphore
// ready to cut = 1, not ready to cut = 0
sem_t barber;

// define mutex accessSeats
sem_t accessSeatsMutex;

// functions barber_run and customer_run
void barber_run(void *arg);
void customer_run(void *arg);

// initialize number of free seats to MAX_SEATS
int numFreeSeats = MAX_SEATS;
// ??
// define pipe numFreeSeats



// initialize number of customerx to MAX_CUSTOMERS
int numCustomers = MAX_CUSTOMERS;

// initalize barber to 1;
int barberNum = 1;

int main(int argc, char *argv[])
{ 
     // create threads for barber and customer
     pthread_t barb, cust;
     int status=0;

     // initialize semaphores
     sem_init(&accessSeatsMutex, 0, 1);
     sem_init(&customer, 0, 0);
     sem_init(&barber, 0, 1);

     // ? initialize pipe numFreeSeats = MAX_SEATS ?

     // initialize barber thread
     status = pthread_create(&barb, NULL, (void *)barber_run, NULL);
     if (status != 0) {
          perror("barber_run failure!\n");
     }

     // initialize customer thread
     status = pthread_create(&cust, NULL, (void *)customer_run, NULL);
     if (status != 0) {
          perror("customer_run failure!\n");
     }

     // first join cust thread then barb thread
     pthread_join(cust, NULL);
     pthread_join(barb, NULL);

     wait();
     printf("Done!\n");
     exit(0);
}

// This is the barber process - an infinite loop
void barber_run(void *arg) {
     int hair = 0;
     while(1) {
     // get a customer or go to sleep
     sem_wait(&customer);    
     printf("Barber %d is trying to get a customer\n", barberNum);
     hair++;
     // wake up and change number of seats
     sem_wait(&accessSeatsMutex);     
     printf("Barber %d is waiting for the seat to become free\n", barberNum);
     // increase number of seats by one
     numFreeSeats++;
     printf("Barber %d is increasing the number of free seats to %d\n", barberNum, numFreeSeats);
     // release lock on seats
     // sem_post(&barber);     
     sem_post(&accessSeatsMutex);
     // barber is cutting hair
     sem_post(&barber);
     printf("Barber is now cutting hair %d\n", hair);     
     //printf("-Customer is now getting a haircut\n");
     barberNum++;
     wait(3);
     } 
}

// This is the customer process
void customer_run(void *arg) {
       // while there are still customers
       while(numCustomers > 0) {
          printf("- New customer trying to find a seat\n");
          // if there are seats in the waiting room
          if (numFreeSeats > 0) {
               // decrease NumFreeSeats
               numFreeSeats--;
               printf("- Customer is decreasing the number of free seats to %d\n", numFreeSeats);
               // notify barber there is a new customer
               // release lock on seats
               sem_post(&accessSeatsMutex);
               printf("- Customer is now waiting for the barber\n");
               // it's customers turn, or he waits
               sem_post(&customer);     
               // now customer is having his hair cut
               sem_wait(&barber);
               printf("- Customer is now getting a haircut\n");
          }
          // else if no seats customer leaves
          else {
               // Customer leaves without haircut
               // release lock on seats
               sem_post(&accessSeatsMutex);
               printf("* Customer giving up: No free chairs in waiting room.\n");
          }
          wait(3);
          // decrease number of customers
          numCustomers--;
     }
}
