#ifndef TASK_HPP
#define TASK_HPP

#include "../fileHandling/IO.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>

using namespace std;

enum class Action {
    ENCRYPT,
    DECRYPT
};

struct Task {
    string filePath;
    // tasks only carry information, not open resources like file streams.
    Action action;

    Task(Action act, string path) 
        : action(act), filePath(move(path)) {}

    string toString() const {
        ostringstream oss;
        oss << filePath << "," << (action == Action::ENCRYPT ? "ENCRYPT" : "DECRYPT");
        return oss.str();
    }

    // fromString no longer opens a file, it just creates a Task object.
    static Task fromString(const string& taskData) {
        istringstream iss(taskData);
        string filePath;
        string actionStr;

        if (getline(iss, filePath, ',') && getline(iss, actionStr)) {
            Action action = (actionStr == "ENCRYPT") ? Action::ENCRYPT : Action::DECRYPT;
            return Task(action, filePath);
        } else {
            throw runtime_error("Invalid task data format in fromString");
        }
    }
};
#endif