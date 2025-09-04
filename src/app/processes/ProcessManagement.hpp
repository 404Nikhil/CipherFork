#ifndef PROCESS_MANAGEMENT_HPP
#define PROCESS_MANAGEMENT_HPP

#include "TaskDataT.hpp"
#include <queue>
#include <memory>
#include <bits/stdc++.h>

class ProcessManagement {
    public:
        ProcessManagement() = default;
        bool submitToQueue(unique_ptr<TaskDataT> task);
        void executeTasks();
    private:
        queue<unique_ptr<TaskDataT>> taskQueue;
        // unique_ptr
        // ownership is only belonging to the assigned ptr that too is scope limited
        

};
#endif