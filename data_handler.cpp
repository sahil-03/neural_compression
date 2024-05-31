#include "data_handler.h"
#include <stdio.h> 
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <thread>
#include <functional>

// Uncomment to enter DEBUG mode
// #define DEBUG


WAVHandler::WAVHandler() {
    this->wav = NULL;
    this->fp = NULL;
    this->num_samples = 0;
}


WAVHandler::~WAVHandler() {
    if (this->fp) fclose(this->fp);
    if (this->wav) free(this->wav);
}


int WAVHandler::open_wav_file(const char *filename) {
    std::string error_message;

    this->fp = fopen(filename, "rb");
    if (!this->fp) {
        throw std::runtime_error("Failed to open WAV file");
    }

    this->wav = (WAV *)malloc(sizeof(WAV));
    if (!this->wav) {
        throw std::runtime_error("Failed to allocate memory for WAV file");
    }

    
    // PARALLELIZED EXECUTION
    // std::thread t1 (std::bind(&WAVHandler::read_wav_file_header, this));
    // std::thread t2 (std::bind(&WAVHandler::read_wav_file_subchunk1, this));
    // std::thread t3 (std::bind(&WAVHandler::read_wav_file_subchunk2, this));
    // t1.join(); 
    // t2.join(); 
    // t3.join(); 

    // SEQUENTIAL EXECUTION
    this->read_wav_file_header();
    this->read_wav_file_subchunk1();
    this->read_wav_file_subchunk2();

    fclose(this->fp);

    // Successfully read entire wav file
    return 0;
}


int WAVHandler::read_wav_file_header() {
    std::string error_message; 

    fread(this->wav->chunkID, sizeof(this->wav->chunkID), 1, this->fp);
    if (std::strcmp(this->wav->chunkID, "RIFF") != 0) {
        throw std::runtime_error("Invalid WAV file. Expected RIFF header, found: " + std::string(this->wav->chunkID));
    }
    fread(&this->wav->chunckSize, sizeof(this->wav->chunckSize), 1, this->fp);
    fread(this->wav->format, sizeof(this->wav->format), 1, this->fp);
    if (std::strcmp(this->wav->format, "WAVE") != 0) {
        throw std::runtime_error("Invalid WAV file. Expected WAVE header, found: " + std::string(this->wav->format));
    }

    // Successfully read header
    return 0;
}


int WAVHandler::read_wav_file_subchunk1() {
    std::string error_message; 

    fread(this->wav->subchunk1ID, sizeof(this->wav->subchunk1ID), 1, this->fp);
    if (std::strcmp(this->wav->subchunk1ID, "fmt ") != 0) {
        throw std::runtime_error("Invalid WAV file. Expected fmt header, found: " + std::string(this->wav->subchunk1ID));
    }
    fread(&this->wav->subchunk1Size, sizeof(this->wav->subchunk1Size), 1, this->fp);
    fread(&this->wav->audioFormat, sizeof(this->wav->audioFormat), 1, this->fp);
    fread(&this->wav->numChannels, sizeof(this->wav->numChannels), 1, this->fp);
    fread(&this->wav->sampleRate, sizeof(this->wav->sampleRate), 1, this->fp);
    fread(&this->wav->byteRate, sizeof(this->wav->byteRate), 1, this->fp);
    fread(&this->wav->blockAlign, sizeof(this->wav->blockAlign), 1, this->fp);
    fread(&this->wav->bitsPerSample, sizeof(this->wav->bitsPerSample), 1, this->fp);

    //Successfully read subchunk1
    return 0; 
}


int WAVHandler::read_wav_file_subchunk2() {
    std::string error_message;

    fread(this->wav->subchunk2ID, sizeof(this->wav->subchunk2ID), 1, this->fp);
    if (std::strcmp(this->wav->subchunk2ID, "data") != 0) {
        throw std::runtime_error("Invalid WAV file. Expected data header, found: " + std::string(this->wav->subchunk2ID));
    }
    fread(&this->wav->subchunk2Size, sizeof(this->wav->subchunk2Size), 1, this->fp);
    
    // Read the data
    this->num_samples = this->wav->subchunk2Size / (this->wav->numChannels * (this->wav->bitsPerSample / 8));
    this->wav_data.resize(this->num_samples); 
    fread(&this->wav_data[0], sizeof(this->wav_data[0]), this->num_samples, this->fp);

#if defined(DEBUG)
    std::ofstream outfile; 
    outfile.open("../debug_data.txt"); 
    for (int i = 0; i < this->num_samples; i++) {
        outfile << this->wav_data[i] << std::endl;
    }
#endif

    // Successfully read subchunk2
    return 0;
}


int main() {
    WAVHandler wav_handler;
    auto start = std::chrono::high_resolution_clock::now();
    wav_handler.open_wav_file("/Users/sahil/Documents/neuralink_compression/data/0ab237b7-fb12-4687-afed-8d1e2070d621.wav");
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); 
    std::cout << "Time taken to read WAV file: " << duration / 1000 << " milliseconds" << std::endl;

    return 0;
}