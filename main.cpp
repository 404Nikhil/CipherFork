#include <iostream>
#include <filesystem>
#include <vector>
#include <stdexcept>
#include <sys/wait.h>
#include <ctime>
#include <iomanip>// put_time()
#include "./src/app/processes/ProcessManagement.hpp"
#include "./src/app/processes/Task.hpp"

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    string directory;
    string action;

    cout << "Enter the directory path: ";
    getline(cin, directory);

    cout << "Enter the action (encrypt/decrypt): ";
    getline(cin, action);

    try {
        if (!fs::exists(directory) || !fs::is_directory(directory)) {
            cerr << "Invalid or non-existent directory path!" << endl;
            return 1;
        }

        ProcessManagement processManagement;
        vector<pid_t> childPids;

        time_t t = time(nullptr);
        tm* now = localtime(&t);
        cout << put_time(now, "[%Y-%m-%d %H:%M:%S] ") << "Starting scan and process submission..." << endl;

        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                string filePath = entry.path().string();
                Action taskAction = (action == "encrypt") ? Action::ENCRYPT : Action::DECRYPT;
                
                auto task = make_unique<Task>(taskAction, filePath);
                
                t = time(nullptr);
                now = localtime(&t);
                cout << put_time(now, "[%Y-%m-%d %H:%M:%S] ") << "Queueing task for: " << filePath << endl;

                pid_t pid = processManagement.submitToQueue(move(task));
                if (pid > 0) {
                    childPids.push_back(pid);
                }
            }
        }

        cout << endl << "All tasks queued. Waiting for " << childPids.size() << " child processes to complete..." << endl;
        // wait for all forked children
        for (pid_t pid : childPids) {
            int status;
            waitpid(pid, &status, 0);
        }
        
        t = time(nullptr);
        now = localtime(&t);
        cout << put_time(now, "[%Y-%m-%d %H:%M:%S] ") << "All child processes have finished. Operation complete." << endl;

    } catch (const fs::filesystem_error& ex) {
        cerr << "Filesystem error: " << ex.what() << endl;
        return 1;
    } catch (const std::exception& ex) {
        cerr << "A critical error occurred during setup: " << ex.what() << endl;
        return 1;
    }

    return 0;
}

