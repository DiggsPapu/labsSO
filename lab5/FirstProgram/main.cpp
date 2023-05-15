/**
 * Diego Andres Alonzo Medinilla 20172
 * Sistemas operativos
 * Lab6
 * Programa 1-> semaphores
*/
// Importing the libraries, threads, semaphores, prints, waits, times, random, etc.
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
// Defining adjustable variables
#define NUM_THREADS     500
#define NUM_ITERATION   10
// Defining the available resources, shared variable
int available_resources = 10;
// Defining the semaphores
sem_t semaphore;
// Defining the mutex
pthread_mutex_t mutex;
// Defining the shared function
void *threadFunction(void *arg){
    // Defining the threadId
    long threadId = (long) arg;
    // Defining the iteration
    for (int i = 0; i < NUM_ITERATION; i++)
    {
        // Printing the enter of the thread
        printf("Hello from my thread %ld! Initialization of the iteration #%d\n", threadId, i);
        // Defining the random time variables
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1,100);
        int randomNum1 = dist(gen);
        int randomNum2 = dist(gen);
        // Defining the wait to substract from the semaphore variable
        sem_wait(&semaphore);
        // In case the shared resources are at 0 it enters in an infinite loop till the resource is released
        while (available_resources == 0){};
        // pthread_mutex_lock(&mutex);
        // Substracting resources
        available_resources--;
        std::cout <<"Doing something with the resource in the thread #"<<threadId<<" , "<<available_resources<<" resources available\n";
        // Doing something or emulating something with the resource
        usleep(randomNum1*randomNum2);
        // Returning the shared resource
        available_resources++;
        std::cout <<"Returning the resource in the thread #"<<threadId<<", "<<available_resources<<" resources available\n";
        // pthread_mutex_unlock(&mutex);
        // Releasing semaphore
        sem_post(&semaphore);
    }
    // Exiting the thread
    pthread_exit(NULL);
}
int main(){
    // Creating the semaphore, notice that it allows multiple threads to enter while <10
    sem_init(&semaphore, 1, 10);
    // Creating the mutex
    pthread_mutex_init(&mutex, NULL);
    // Creating the threads
    pthread_t threads[NUM_THREADS];
    // Creating a value to check if the thread was incorrectly created
    int check;
    // Iteration to create and execute the threads
    for (long i = 0; i < NUM_THREADS; i++)
    {
        // Assigning the creation to variable "check" to check if the thread was correctly created
        check = pthread_create(&threads[i], NULL, threadFunction, (void *) i);
        // Printing error in case of error
        if (check) {
            printf("ERROR: while creating thread %ld and error code is %d\n", i, check);
            return 1;
        }
    }
    // Waiting for the threads to finish
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }
    // Destroying the semaphore
    sem_destroy(&semaphore);
    // Destroying the mutex
    pthread_mutex_destroy(&mutex);
    // Exiting the program
    return 0;
}