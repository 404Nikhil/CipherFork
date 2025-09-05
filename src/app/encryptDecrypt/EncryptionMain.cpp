#include <iostream>
#include "Encryption.hpp"
using namespace std;
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: ./encryption <task_data>" << endl;
        return 1;
    }
    executeEncryption(argv[1]);
    return 0;
}
// ./encryption "ENCRYPT|input.txt"