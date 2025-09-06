#ifndef PROCESS_MANAGEMENT_HPP
#define PROCESS_MANAGEMENT_HPP

#include "Task.hpp"
#include <memory>
#include <atomic>
#include <semaphore.h>
#include <iostream>
#include <unistd.h> // pid_t

using namespace std;

class ProcessManagement {
public:
    ProcessManagement();
    ~ProcessManagement();
    pid_t submitToQueue(unique_ptr<Task> task);
    void executeTask();

private:
    struct SharedMemory {
        atomic<int> size;
        char tasks[1000][256];
        int front;
        int rear;
    };

    SharedMemory* sharedMem;
    int shmFd;
    
    // names for shared memory and semaphores
    const char* SHM_NAME = "/forkcrypt_shm";
    const char* MUTEX_SEM_NAME = "/forkcrypt_mutex";
    const char* ITEMS_SEM_NAME = "/forkcrypt_items";
    const char* SLOTS_SEM_NAME = "/forkcrypt_slots";

    // semaphores for synchronization between processes
    sem_t* mutexSemaphore;
    sem_t* itemsSemaphore;
    sem_t* emptySlotsSemaphore;
};

#endif

