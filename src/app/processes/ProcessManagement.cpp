#include "ProcessManagement.hpp"
#include "../encryptDecrypt/Encryption.hpp"
#include <iostream>
#include <stdexcept> //std::runtime_error
#include <unistd.h>
#include <cstring>
#include <cerrno> //perror
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <sys/mman.h>

using namespace std;

ProcessManagement::ProcessManagement() {
    shm_unlink(SHM_NAME);
    sem_unlink(MUTEX_SEM_NAME);
    sem_unlink(ITEMS_SEM_NAME);
    sem_unlink(SLOTS_SEM_NAME);

    // If a semaphore or shared memory segment fails to be created, the program
    // must be stopped immediately to prevent a segfault from using a bad pointer.

    mutexSemaphore = sem_open(MUTEX_SEM_NAME, O_CREAT, 0666, 1);
    if (mutexSemaphore == SEM_FAILED) {
        perror("sem_open(mutex) failed");
        throw runtime_error("Failed to create mutex semaphore.");
    }

    itemsSemaphore = sem_open(ITEMS_SEM_NAME, O_CREAT, 0666, 0);
    if (itemsSemaphore == SEM_FAILED) {
        perror("sem_open(items) failed");
        throw runtime_error("Failed to create items semaphore.");
    }

    emptySlotsSemaphore = sem_open(SLOTS_SEM_NAME, O_CREAT, 0666, 1000);
    if (emptySlotsSemaphore == SEM_FAILED) {
        perror("sem_open(slots) failed");
        throw runtime_error("Failed to create empty slots semaphore.");
    }

    shmFd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shmFd == -1) {
        perror("shm_open failed");
        throw runtime_error("Failed to create shared memory file descriptor.");
    }

    ftruncate(shmFd, sizeof(SharedMemory));
    
    sharedMem = static_cast<SharedMemory *>(mmap(nullptr, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0));
    if (sharedMem == MAP_FAILED) {
        perror("mmap failed");
        throw runtime_error("Failed to map shared memory.");
    }
    
    sharedMem->front = 0;
    sharedMem->rear = 0;
    sharedMem->size.store(0);
}

ProcessManagement::~ProcessManagement() {
    munmap(sharedMem, sizeof(SharedMemory));
    close(shmFd); // close the file descriptor
    shm_unlink(SHM_NAME);
    sem_close(mutexSemaphore);
    sem_close(itemsSemaphore);
    sem_close(emptySlotsSemaphore);
    sem_unlink(MUTEX_SEM_NAME);
    sem_unlink(ITEMS_SEM_NAME);
    sem_unlink(SLOTS_SEM_NAME);
}

pid_t ProcessManagement::submitToQueue(unique_ptr<Task> task) {
    sem_wait(emptySlotsSemaphore);
    sem_wait(mutexSemaphore);

    string taskStr = task->toString();
    
    strncpy(sharedMem->tasks[sharedMem->rear], taskStr.c_str(), 255);
    sharedMem->tasks[sharedMem->rear][255] = '\0';
    
    sharedMem->rear = (sharedMem->rear + 1) % 1000;
    sharedMem->size.fetch_add(1);

    sem_post(mutexSemaphore);
    sem_post(itemsSemaphore);

    pid_t pid = fork();
    if (pid < 0) { 
        perror("fork failed");
        // rollback the queue state if fork fails
        sem_wait(itemsSemaphore);
        sem_wait(mutexSemaphore);
        sharedMem->rear = (sharedMem->rear == 0) ? 999 : sharedMem->rear - 1;
        sharedMem->size.fetch_sub(1);
        sem_post(mutexSemaphore);
        sem_post(emptySlotsSemaphore);
        return -1;
    } else if (pid == 0) {
        executeTask();
        exit(0);
    }
    
    return pid;
}

void ProcessManagement::executeTask() {
    sem_wait(itemsSemaphore);
    sem_wait(mutexSemaphore);
    
    char taskStr[256];
    strcpy(taskStr, sharedMem->tasks[sharedMem->front]);
    sharedMem->front = (sharedMem->front + 1) % 1000;
    sharedMem->size.fetch_sub(1);

    sem_post(mutexSemaphore);
    sem_post(emptySlotsSemaphore);
    
    executeEncryption(taskStr);
}
