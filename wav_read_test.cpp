#include <iostream>
#include <memory>
#include <string>
#include <fstream>
using std::ifstream;

#include "wav_read.hpp"

int main(int argc, char** argv) {
    std::string file_path;
    if(argc != 2)
        file_path = R"(wav-files\sample-1.wav)";
    else
        file_path = argv[1];

    // pointer to access header information
    wav_header_ptr myWAVHeaderPtr = std::make_unique<wav_header>();

    if(readWAVHeader(file_path, myWAVHeaderPtr)) {
        std::cout << "Successfully read the header. " << std::endl;
        std::cout << "Channels : " << myWAVHeaderPtr->numChannels << std::endl;
    }
    else {
        std::cout << "Failed to read the header." << std::endl;
    }

    return 0;
}
