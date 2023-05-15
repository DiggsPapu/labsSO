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
#define NUM_THREADS     50
#define NUM_ITERATION   10
// Defining the available resources, shared variable
int available_resources = 10;
std::ofstream outputFile;
// Defining the semaphores
sem_t semaphore;
// Defining the mutex
pthread_mutex_t mutex;
// Defining a struct to pass arguments to write in the file
struct data {
    // Thread id
    long id;
    // Iteration of the thread
    int iteration;
    // A lil protocol to know what 2 print
    int signal;
    // At that time the available resource
    int availableResource;
};
// Writing in the file
void writeFile(void* arg) {
    // Getting the arguments
    struct data* info = (struct data*)arg;
    std::ifstream inputFile("results.txt");
    if (inputFile.is_open()) 
    {
        std::string line;
        std::string lastLine;
        while (std::getline(inputFile, line)){ lastLine = line;}
        inputFile.close();
        outputFile.open("results.txt", std::ios::app);
        // Depending on the signal it will be writen in the text file
        switch (info->signal)
        {
        case 1:
        // Initialization
            outputFile << "Hello from my thread "<<info->id<<" Initialization of the iteration "<<info->iteration<<std::endl;
            outputFile.close();
            // printf("Hello from my thread %ld! Initialization of the iteration #%d\n", info->id, info->iteration);
            break;
        case 2:
        // Using the resource
            outputFile << "Doing something with the resource in the thread # "<<info->id<<" , "<<info->availableResource<<std::endl;
            outputFile.close();
            // std::cout <<"Doing something with the resource in the thread #"<<info->id<<" , "<<info->availableResource<<" resources available\n";
            break;
        case 3:
        // Returning the resource
            outputFile << "Returning the resource in the thread #"<<info->id<<" , "<<info->availableResource<<" resources available"<<std::endl;
            outputFile.close();
            // std::cout <<"Returning the resource in the thread #"<<info->id<<", "<<info->availableResource<<" resources available\n";
            break;
        default:
            break;
        }
    } 
    else
    {
        std::cout <<"Failed to open the file."<<std::endl;
    }
    
}
// Defining the shared function
void *threadFunction(void *arg){
    // Defining the struct
    data results;
    // Defining the threadId
    results.id = (long) arg;
    // Defining the iteration
    for (int i = 0; i < NUM_ITERATION; i++)
    {
        results.iteration = i;
        results.signal = 1;
        // Printing the enter of the thread
        pthread_mutex_lock(&mutex);
        writeFile(&results);
        pthread_mutex_unlock(&mutex);
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
        // Substracting resources
        available_resources--;
        results.signal = 2;
        results.availableResource = available_resources;
        pthread_mutex_lock(&mutex);
        writeFile(&results);
        pthread_mutex_unlock(&mutex);
        // Doing something or emulating something with the resource
        usleep(randomNum1*randomNum2);
        // Returning the shared resource
        available_resources++;
        results.signal = 3;
        results.availableResource = available_resources;
        pthread_mutex_lock(&mutex);
        writeFile(&results);
        pthread_mutex_unlock(&mutex);
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