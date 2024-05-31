#ifndef _DATA_HANDLER_H
#define _DATA_HANDLER_H

#include <stdio.h>
#include <stdint.h>
#include <vector>

class WAVHandler {
    public: 
        typedef struct {
            /* Wav file header */
            char chunkID[4];
            uint32_t chunckSize; 
            char format[4]; 

            /* Wav file subchunk1 */
            char subchunk1ID[4];
            uint32_t subchunk1Size;
            uint16_t audioFormat;  
            uint16_t numChannels;
            uint32_t sampleRate;
            uint32_t byteRate;
            uint16_t blockAlign;
            uint16_t bitsPerSample;

            /* Wav file subchunk2 */    
            char subchunk2ID[4];
            uint32_t subchunk2Size;
        } WAV;

        WAV *wav;
        uint32_t num_samples;
        std::vector<int16_t> wav_data;

        WAVHandler();
        ~WAVHandler();
        int open_wav_file(const char *filename);


    protected: 
        FILE *fp;

        int read_wav_file_header();
        int read_wav_file_subchunk1();
        int read_wav_file_subchunk2();

};

#endif