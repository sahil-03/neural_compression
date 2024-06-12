#include "fft.h"
#include <iostream>
#include <complex>
#include <vector>


std::complex<float> FFT::get_root(int n, int k) {
    return std::polar(1.0f, static_cast<float>(-2 * M_PI * k / n));
}


void FFT::bit_reverse(std::vector<std::complex<float> >& x, uint32_t N) {
    for (int i = 1, j = 0; i < N; i++) {
        int bit = N >> 1;
        for (; j >= bit; bit >>= 1) j -= bit;
        j += bit;
        if (i < j) swap(x[i], x[j]);
    }
}


void FFT::fft(std::vector<std::complex<float> >& x, uint32_t N) {
    bit_reverse(x, N);

    for (int s = 1; s <= log2(N); s++) {
        int m = 1 << s;
        int m2 = m >> 1;
        std::complex<float> wm = get_root(m, 1);
        std::complex<float> w = 1;

        for (int j = 0; j < m2; j++) {
            for (int k = j; k < N; k += m) {
                int k1 = k + m2;
                std::complex<float> t = w * x[k1];
                x[k1] = x[k] - t;
                x[k] += t;
            }
            w *= wm;
        }
    }
}


void FFT::ifft(std::vector<std::complex<float> >& x, uint32_t N) {
    fft(x, N);
    for (int i = 0; i < N; i++) x[i] /= N;
    std::reverse(x.begin() + 1, x.end());
}

// int main() {
//     // Example usage
//     std::vector<std::complex<float> > x;
//     x.push_back(std::complex<float>(1, 0));
//     x.push_back(std::complex<float>(2, 0));
//     x.push_back(std::complex<float>(3, 0));
//     x.push_back(std::complex<float>(4, 0)); 

//     FFT f;

//     f.fft(x, 4);
//     std::cout << "FFT: ";
//     for (auto& c : x)
//         std::cout << c << " ";
//     std::cout << "\n";

//     f.ifft(x, 4);
//     std::cout << "IFFT: ";
//     for (auto& c : x)
//         std::cout << c << " ";
//     std::cout << "\n";

//     return 0;
// }