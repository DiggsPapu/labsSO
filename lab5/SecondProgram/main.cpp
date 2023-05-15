/**
 * Diego Andres Alonzo Medinilla 20172
 * Sistemas operativos
 * Lab6
 * Programa 2->monitors
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
// Defining a 
#define NUM_THREADS     500
#define NUM_ITERATION   10
int available_resources = 10;
sem_t semaphore;
pthread_mutex_t mutex;
void *threadFunction(void *arg){
    long threadId = (long) arg;
    for (int i = 0; i < NUM_ITERATION; i++)
    {
        printf("Hello from my thread %ld! Initialization of the iteration #%d\n", threadId, i);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1,100);
        int randomNum1 = dist(gen);
        int randomNum2 = dist(gen);
        sem_wait(&semaphore);
        while (available_resources == 0){};
        // pthread_mutex_lock(&mutex);
        available_resources--;
        std::cout <<"Doing something with the resource in the thread #"<<threadId<<" , "<<available_resources<<" resources available\n";
        usleep(randomNum1*randomNum2);
        available_resources++;
        std::cout <<"Returning the resource in the thread #"<<threadId<<", "<<available_resources<<" resources available\n";
        // pthread_mutex_unlock(&mutex);
        sem_post(&semaphore);
    }
    
    pthread_exit(NULL);
}
int decrease_count(int count) {
    if (available_resources < count) { return -1;}
    available_resources -= count;return 0;
}
int increase_count(int count) { available_resources +=count; return 0;}
int main(){
    sem_init(&semaphore, 1, 10);
    pthread_mutex_init(&mutex, NULL);
    pthread_t threads[NUM_THREADS];
    int check;
    for (long i = 0; i < NUM_THREADS; i++)
    {
        check = pthread_create(&threads[i], NULL, threadFunction, (void *) i);
        if (check) {
            printf("ERROR: while creating thread %ld and error code is %d\n", i, check);
            return 1;
        }
    }
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }
    sem_destroy(&semaphore);
    pthread_mutex_destroy(&mutex);
    return 0;
}