#include "Encryption.hpp"
#include "../processes/Task.hpp"
#include "../fileHandling/IO.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;
namespace {
    class ReadEnv {
    public:
        string getenv() {
            string env_path = ".env";
            IO io(env_path);
            fstream f_stream = io.getFileStream();
            stringstream buffer;
            if (f_stream.is_open()) {
                buffer << f_stream.rdbuf();
            }
            return buffer.str();
        }
    };
}

void executeEncryption(const char* taskStr) {
    try {
        string taskData(taskStr);
        Task task = Task::fromString(taskData);

        ReadEnv envReader;
        string key_str = envReader.getenv();
        if (key_str.empty()) {
            cerr << "ERROR: Encryption key not found in .env file for task: " << task.filePath << endl;
            return;
        }
        char key = key_str[0];

        // child process opens the file here.
        // the single point of responsibility for file I/O.
        fstream file_stream(task.filePath, ios::in | ios::out | ios::binary);
        if (!file_stream.is_open()) {
            cerr << "Child process could not open file: " << task.filePath << endl;
            return;
        }

        file_stream.seekg(0, ios::end);
        streampos fileSize = file_stream.tellg();
        file_stream.seekg(0, ios::beg);

        if (fileSize == 0) { // empty files
            file_stream.close();
            return;
        }

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