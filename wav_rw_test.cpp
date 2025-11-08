#include <iostream>
#include <memory>
#include <string>
#include <fstream>
using std::ifstream;

#include "wav_rw.hpp"
constexpr float KILOBYTE = 1024.0f;

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
        std::cout << "Total file size : " << (myWAVHeaderPtr->subChunk2Size + sizeof(wav_header)) / KILOBYTE << "KB"<< std::endl;
    }
    else {
        std::cout << "Failed to read the header." << std::endl;
    }
    
    std::string ofile_path;
    if(argc != 3)
        ofile_path = R"(wav-files\sample-1.0.wav)";
    else 
        ofile_path = argv[2];
    
    if(writeBackWAV(file_path, ofile_path)) {
        std::cout << "Write Task complete!" << std::endl;
    }
    else std::cout << "Failed to copie contents" << std::endl;

    return 0;
}
