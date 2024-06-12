#ifndef _ENCODE_H 
#define _ENCODE_H


#include <iostream> 
#include <stdio.h> 
#include <vector>

class Encoder {
    public: 
        std::string output_file_path;

        Encoder(std::string file_path); 
        void encode(std::vector<int16_t> &data, uint32_t sample_rate);

    protected: 
        // todo
};

#endif 