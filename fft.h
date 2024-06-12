#ifndef FFT_H
#define FFT_H

#include <iostream>
#include <complex> 
#include <vector> 


class FFT {
    public: 
        void fft(std::vector<std::complex<float> >& x, uint32_t N); 
        void ifft(std::vector<std::complex<float> >& x, uint32_t N);

    protected: 
        std::complex<float> get_root(int n, int k);
        void bit_reverse(std::vector<std::complex<float> >& x, uint32_t N);
};

#endif