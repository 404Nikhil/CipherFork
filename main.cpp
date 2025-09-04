#include<bits/stdc++.h>
#include <filesystem>
#include "./src/app/processes/ProcessManagement.hpp"
#include "./src/app/processes/TaskDataT.hpp"
#include "./src/app/encryptDecrypt/Encryption.hpp"
namespace fs = std::filesystem;
using namespace std;

int main(int argc, char *argv[]){
    string directory;
    string action;
    cout << "Enter directory path: "<<endl;
    getline(cin, directory);
    cout << "Enter action (ENCRYPT/DECRYPT): "<<endl;
    getline(cin, action);

    try {
        if(fs::exists(directory) && fs::is_directory(directory)){
            ProcessManagement pm;
            for(const auto & entry : fs::directory_iterator(directory)){
                if(entry.is_regular_file()){
                    string file_path = entry.path().string();
                    IO io(file_path);
                    fstream file_stream = move(io.getFileStream());
                    if(file_stream.is_open()){
                        Action actionType = (action == "ENCRYPT") ? Action::ENCRYPT : Action::DECRYPT;
                        auto  task = make_unique<TaskDataT>(actionType, file_path, move(file_stream)); 
                        pm.submitToQueue(move(task));
                    } else {
                        cerr << "Error: Could not open file " << file_path << endl;
                    }
                }
            }
            pm.executeTasks();
    }else {
        cout << "Directory does not exist or is not a directory." << endl;
    }
       } catch(const fs::filesystem_error& e){
        cerr << "Filesystem error: " << e.what() << endl;
    }
}