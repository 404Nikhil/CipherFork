#include "Encryption.hpp"
#include "../processes/Task.hpp"
#include "../fileHandling/ReadEnv.cpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

void executeEncryption(const char* taskStr) {
    try {
        // deserialize the task string
        string taskData(taskStr);
        Task task = Task::fromString(taskData);

        // the encryption key.
        ReadEnv envReader;
        string key_str = envReader.getenv();
        if (key_str.empty()) {
            cerr << "ERROR: Encryption key not found in .env file for task: " << task.filePath << endl;
            return;
        }
        char key = key_str[0];

        fstream file_stream(task.filePath, ios::in | ios::out | ios::binary);
        if (!file_stream.is_open()) {
            cerr << "Child process could not open file: " << task.filePath << endl;
            return;
        }

        file_stream.seekg(0, ios::end);
        streampos fileSize = file_stream.tellg();
        file_stream.seekg(0, ios::beg);

        vector<char> buffer(fileSize);
        file_stream.read(buffer.data(), fileSize);

        for (size_t i = 0; i < buffer.size(); ++i) {
            if (task.action == Action::ENCRYPT) {
                buffer[i] = buffer[i] + key;
            } else {
                buffer[i] = buffer[i] - key;
            }
        }

        file_stream.seekp(0, ios::beg);
        file_stream.write(buffer.data(), fileSize);
        file_stream.close();

    } catch (const exception& e) {
        cerr << "An exception occurred in child process: " << e.what() << endl;
    }
}
