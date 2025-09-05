#include "Encryption.hpp"
#include <iostream>
#include "../fileHandling/ReadEnv.cpp"
#include "../processes/TaskDataT.hpp"

int executeEncryption(const string &taskData){
    TaskDataT task = TaskDataT::fromString(taskData);
    ReadEnv env;
    string env_content = env.getenv();
    int key = stoi(env_content);
    if(task.action == Action::ENCRYPT){
        char ch;
        while(task.inputFileStream.get(ch)){
            ch = (ch+key)%126;
            task.inputFileStream.seekp(-1, ios::cur);
            task.inputFileStream.put(ch);
        }
        task.inputFileStream.close();
    }
    else {
        char ch;
        while(task.inputFileStream.get(ch)){
            ch = (ch-key+126)%126;
            task.inputFileStream.seekp(-1, ios::cur);
            task.inputFileStream.put(ch);
        }
        task.inputFileStream.close();
    }
    return 0;
}