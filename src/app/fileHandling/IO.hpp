// for only 1 time compilation
#ifndef IO_HPP
#define IO_HPP
#include <iostream>
#include <string>
#include <fstream>

using namespace std;
class IO {
    public:
        IO(const string &file_path);
        ~IO();
        fstream getFileStream();

    private:
        fstream file_stream;

};

#endif