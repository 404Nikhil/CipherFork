#include <iostream>
#include "ProcessManagement.hpp"
#include <unistd.h>
#include <cstring>
#include <vector>
#include <sys/wait.h>
#include "../encryptDecrypt/Cryption.hpp"

ProcessManagement::ProcessManagement() {}

bool ProcessManagement::submitToQueue(std::unique_ptr<Task> task) {
    taskQueue.push(std::move(task));
    return true;
}

void ProcessManagement::executeTasks() {
    while (!taskQueue.empty()) {
       std::vector<pid_t> pids;
        while(!taskQueue.empty()){
            std::unique_ptr<Task> taskToExecute = std::move(taskQueue.front());
            taskQueue.pop();
            pid_t childprocess = fork();
            if(childprocess == 0) {
                std::string taskstr = taskToExecute->toString();
                std::cout << "child process " << getpid() << " running the task " << taskstr << std::endl;
                char *args[3];
                args[0] = strdup("./cryption");
                args[1] = strdup(taskstr.c_str());
                args[2] = nullptr;
                if(execv("./cryption", args) == -1) {
                    perror("execv failed");
                    free(args[0]);
                    free(args[1]);
                }
                free(args[0]);
                free(args[1]);
            } else if (childprocess > 0) {
                // wait state
                std::cout << "Parent called child process " << childprocess << std::endl;
                pids.push_back(childprocess);
            } else {
                std::cerr << "Fork Failed" << std::endl;
                exit(1);
            }
        }

        for (pid_t childPid : pids) {
            int status;
            waitpid(childPid, &status, 0);
            if (WIFEXITED(status)) {
                std::cout << "Child process " << childPid << " exited with status " << WEXITSTATUS(status) << std::endl;
            } else {
                std::cout << "Child process " << childPid << " terminated abnormally" << std::endl;
            }
        }
    }
}