#ifndef TASK_HPP
#define TASK_HPP
#include <bits/stdc++.h>
#include <string>
#include "../fileHandling/IO.hpp"
#include <iostream>

enum class Action
{
    ENCRYPT,
    DECRYPT
};

struct TaskDataT
{
    Action action;
    string inputFilePath;
    fstream inputFileStream;
    Action actionType;

     TaskDataT(std::fstream&& stream, Action actionType, std::string inputFilePath)
        : inputFilePath(std::move(inputFilePath)), actionType(actionType) 
    {}
        // serialization and deserialization done here

        string toString(){
            ostringstream oss;
            oss << inputFilePath << "," << (actionType == Action::ENCRYPT ? "ENCRYPT" : "DECRYPT");
            
            return oss.str();
        }

        static TaskDataT fromString(const string &str){
            istringstream iss(str);
            string inputFilePath, actionStr;
            if(getline(iss, inputFilePath, ',') && getline(iss, actionStr)){
                Action actionType = (actionStr == "ENCRYPT") ? Action::ENCRYPT : Action::DECRYPT;
                IO io(inputFilePath);
                fstream inputFileStream = move(io.getFileStream());
                if(inputFileStream.is_open()){
                    return TaskDataT(move(inputFileStream), actionType, inputFilePath);
                } else {
                    throw runtime_error("Failed to open file: " + inputFilePath);
                }
            } else {
                throw runtime_error("Invalid string format for TaskDataT");
            }
        }
    
};

#endif