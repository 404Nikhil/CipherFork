#include <iostream>
#include "Encryption.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./encryption <task_data>" << std::endl;
        return 1;
    }
    executeEncryption(argv[1]);
    return 0;
}
// ./encryption "ENCRYPT|input.txt"