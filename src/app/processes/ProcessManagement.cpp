#include <iostream>
#include "ProcessManagement.hpp"
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>
#include "../encryptDecrypt/Encryption.hpp"

ProcessManagement::ProcessManagement() {}

bool ProcessManagement::submitToQueue(std::unique_ptr<Task> task) {
    taskQueue.push(std::move(task));
    return true;
}

void ProcessManagement::executeTasks() {
    while (!taskQueue.empty()) {
        std::unique_ptr<Task> taskToExecute = std::move(taskQueue.front());
        taskQueue.pop();
        std::cout << "Executing task: " << taskToExecute->toString() << std::endl;
        // Add a breakpoint here in VS Code
        executeEncryption(taskToExecute->toString());
        // int childProcessToRun = fork();
        // if (childProcessToRun == 0) {
        //     // Child process
        //     std::string taskStr = taskToExecute->toString();
        //     char* args[3];
        //     args[0] = strdup("./encryption");  // Use the correct path to your encryption executable
        //     args[1] = strdup(taskStr.c_str());
        //     args[2] = nullptr;
        //     execv("./encryption", args);  // Use the correct path to your encryption executable
        //     // If execv returns, there was an error
        //     std::cerr << "Error executing encryption" << std::endl;
        //     exit(1);
        // } else if (childProcessToRun > 0) {
        //     // Parent process
        //     // Wait for the child process to complete
        //     int status;
        //     waitpid(childProcessToRun, &status, 0);
        // } else {
        //     // Fork failed
        //     std::cerr << "Fork failed" << std::endl;
        //     exit(1);
        // }
    }
}