/**
 * Diego Andres Alonzo Medinilla 20172
 * Sistemas operativos
 * Lab6
 * Program 2
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

#define NUM_THREADS     50
#define NUM_ITERATION  10
// Recursos compartidos
int availableResources = 10;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
std::ofstream outputFile;
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
    // The resource used
    int amountResource;
};
// Monitor
class ResourceMonitor {
    // Defining the variables
    private:
        pthread_mutex_t mutex_;
        pthread_cond_t cond_;
    // Declaring the methods
    public:
        // Constructor
        ResourceMonitor() {
            mutex_ = PTHREAD_MUTEX_INITIALIZER;
            cond_ = PTHREAD_COND_INITIALIZER;
        }
        // Consuming resources method
        void consumeResources(int numResources) {
            // Locks the var
            pthread_mutex_lock(&mutex_);
            // In case there is not enough resources it enters in a loop
            while (availableResources < numResources) {
                pthread_cond_wait(&cond_, &mutex_);
            }
            // Consuming the resources
            availableResources -= numResources;
            // Unlock the mutex
            pthread_mutex_unlock(&mutex_);
        }
        // Release resources method
        void releaseResources(int numResources) {
            // Locking the mutex exclusion
            pthread_mutex_lock(&mutex_);
            // Free resources
            availableResources += numResources;
            // Sending a signal
            pthread_cond_broadcast(&cond_);
            // Mutex unlock
            pthread_mutex_unlock(&mutex_);
        }
        // Method to write on file
        void writeOnFile(data info){
            // Locks the mutex
            pthread_mutex_lock(&mutex_);
            // Writing the data
            std::ifstream inputFile("secondoutput.txt");
            if (inputFile.is_open()) 
            {
                std::string line;
                std::string lastLine;
                while (std::getline(inputFile, line)){ lastLine = line;}
                inputFile.close();
                outputFile.open("secondoutput.txt", std::ios::app);
                // Depending on the signal it will be writen in the text file
                switch (info.signal)
                {
                case 1:
                // Initialization
                    outputFile << "Hello from my thread "<<info.id<<" Initialization of the iteration "<<info.iteration<<std::endl;
                    outputFile.close();
                    // printf("Hello from my thread %ld! Initialization of the iteration #%d\n", info->id, info->iteration);
                    break;
                case 2:
                // Using the resource
                    outputFile << "Doing something with the resource in the thread # "<<info.id<<" , "<<info.availableResource<<std::endl;
                    outputFile.close();
                    // std::cout <<"Doing something with the resource in the thread #"<<info->id<<" , "<<info->availableResource<<" resources available\n";
                    break;
                case 3:
                // Returning the resource
                    outputFile << "Returning the resource in the thread #"<<info.id<<" , "<<info.availableResource<<" resources available"<<std::endl;
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
            pthread_mutex_unlock(&mutex_);
        }
};
ResourceMonitor monitor;
// Defining the shared function
void *threadFunction(void *arg){
    // Defining the struct
    data results;
    // Defining the threadId
    results.id = (long) arg;
    // Defining the iteration
    for (int i = 0; i < NUM_ITERATION; i++)
    {
        // Signal of init
        results.iteration = i;
        results.signal = 1;
        // Printing the enter of the thread
        monitor.writeOnFile(results);
        // Defining the random time variables
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(1,100);
        int randomNum1 = dist(gen);
        int randomNum2 = dist(gen);
        std::uniform_int_distribution<> dist1(1,10);
        results.amountResource = dist1(gen);
        // Defining the wait to substract from the semaphore variable
        monitor.consumeResources(results.amountResource);
        results.signal = 2;
        results.availableResource = availableResources;
        monitor.writeOnFile(results);
        // Doing something or emulating something with the resource
        usleep(randomNum1*randomNum2);
        // Returning the shared resource
        monitor.releaseResources(results.amountResource);
        results.signal = 3;
        results.availableResource = availableResources;
        monitor.writeOnFile(results);
    }
    // Exiting the thread
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    // Creating the threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        // Creating the thread
        pthread_create(&threads[i], NULL, threadFunction, (void *)i);
    }
    // Joining the threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }
    return 0;
}
