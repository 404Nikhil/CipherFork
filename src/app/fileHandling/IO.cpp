#include <iostream>
#include <IO.hpp>
#include <fstream>
IO::IO(const string &file_path) {
    file_stream.open(file_path, ios::in | ios::out | ios::binary);
    if (!file_stream.is_open()) {
        cerr << "Error opening file: " << file_path << endl;
        exit(EXIT_FAILURE);
    }
}

fstream IO::getFileStream() {
    return move(file_stream); // return the file stream
}

IO::~IO() {
    if (file_stream.is_open()) {
        file_stream.close();
    }
}