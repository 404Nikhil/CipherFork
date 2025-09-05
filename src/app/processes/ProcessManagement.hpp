#ifndef PROCESS_MANAGEMENT_HPP
#define PROCESS_MANAGEMENT_HPP

#include "Task.hpp"
#include <memory>
#include <mutex>
#include <atomic>
#include <semaphore.h>
using namespace std;
class ProcessManagement
{
    sem_t *itemsSemaphore;
    sem_t *emptySlotsSemaphore;

public:
    ProcessManagement();
    ~ProcessManagement();
    bool submitToQueue(unique_ptr<Task> task);
    void executeTask();

private:
    // shared memory data structure to share queue between processes
    // mmap files
    // shared file descriptor
    struct SharedMemory
    {                     //
        atomic<int> size; // one at a time
        char tasks[1000][256];
        int front;
        int rear;

        void printSharedMemory()
        {
            cout << size << endl;
            cout << front << endl;
            cout << rear << endl;
        }
    };
    SharedMemory *sharedMem;
    int shmFd;
    const char *SHM_NAME = "/my_queue"; // name of the shared memory object file
    mutex queueLock;
};

#endif