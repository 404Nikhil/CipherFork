#include <iostream>
#include <IO.hpp>
#include <fstream>
#include <sstream>
#include <bits/stdc++.h>
using namespace std;

class ReadEnv {
    public:
        string getenv(){
            string env_path = ".env";
            IO io(env_path);
            fstream file_stream = io.getFileStream();
            stringstream buffer;
            buffer << file_stream.rdbuf();
            string file_content = buffer.str();
            return file_content;
        }
};