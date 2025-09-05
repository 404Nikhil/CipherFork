#include <iostream>
#include "ProcessManagement.hpp"
#include <unistd.h>
#include <cstring>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include "../encryptDecrypt/Encryption.hpp"
#include <sys/mman.h>
#include <atomic>
#include <semaphore.h>

ProcessManagement::ProcessManagement() {
    sem_t* itemsSemaphore = sem_open("/items_semaphore", O_CREAT, 0666, 0); // 0666 permissions, initial value 0
    sem_t* emptySlotsSemaphore = sem_open("/empty_slots_semaphore", O_CREAT, 0666, 1000);
    shmFd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666); // mmap file descriptor
    ftruncate(shmFd, sizeof(SharedMemory));
    // mmap init shared memory
    sharedMem = static_cast<SharedMemory *>(mmap(nullptr, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0));
    sharedMem->front = 0;
    sharedMem->rear = 0;
    sharedMem->size.store(0);
}

ProcessManagement::~ProcessManagement() {
    munmap(sharedMem, sizeof(SharedMemory));
    shm_unlink(SHM_NAME);
}

bool ProcessManagement::submitToQueue(std::unique_ptr<Task> task) {
    sem_wait(emptySlotsSemaphore); // queue slots available
    std::unique_lock<std::mutex> lock(queueLock);

    if (sharedMem->size.load() >= 1000) {
        return false;
    }
    strcpy(sharedMem->tasks[sharedMem->rear], task->toString().c_str()); // serialization of task into string
    sharedMem->rear = (sharedMem->rear + 1) % 1000; // cyclic queue implementation + to prevent out of bounds
    sharedMem->size.fetch_add(1);
    lock.unlock();
    sem_post(itemsSemaphore); // if someone is waiting for items, notify them

    int pid = fork(); // child process spinup, parent processid
    if (pid < 0) { 
        return false;
    } else if (pid == 0) {
        executeTask();
        exit(0);
    }
    return true;
}

void ProcessManagement::executeTask() {
    sem_wait(itemsSemaphore);
    std::unique_lock<std::mutex> lock(queueLock);
    char taskStr[256];
    strcpy(taskStr, sharedMem->tasks[sharedMem->front]);
    sharedMem->front = (sharedMem->front + 1) % 1000;
    sharedMem->size.fetch_sub(1); // subtract one from size
    lock.unlock();
    sem_post(emptySlotsSemaphore);
    executeEncryption(taskStr);
}