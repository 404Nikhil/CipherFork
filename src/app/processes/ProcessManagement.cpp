#include <ProcessManagement.hpp>
#include <bits/stdc++.h>
#include<cstring>
#include <memory>
#include <sys/wait.h>
# include "../encryptDecrypt/Encryption.hpp"
ProcessManagement::ProcessManagement(){}

bool ProcessManagement::submitToQueue(unique_ptr<TaskDataT> task){
    if(task == nullptr){
        cerr << "Error: Task is null" << endl;
        return false;
    }
    taskQueue.push(move(task));
    return true;
}

void ProcessManagement::executeTasks(){
    while(!taskQueue.empty()){
        unique_ptr<TaskDataT> task = move(taskQueue.front());
        taskQueue.pop(); // ownership transferred
        cout << "Executing Task: " << task->toString() << endl;
        string taskString = task->toString();
        executeEncryption(taskString);
    }
}