cmake_minimum_required(VERSION 3.10)
project(neural_compress)
# add_executable(neural_compress encode.cpp stft.cpp data_handler.cpp)
add_executable(neural_compress stft.cpp fft.cpp)