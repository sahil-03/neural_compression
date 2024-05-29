import os 
import numpy as np
import wave
import matplotlib.pyplot as plt
from pathlib import Path 
from scipy.io import wavfile

DATA_PATH = Path(__file__).parent / 'data/'

def plot_wav(file_path: str, file_no: int) -> None: 
    sample_rate, data = wavfile.read(file_path)
    if len(data.shape) == 2: 
        data = data.mean(axis=1)
    time = np.arange(data.shape[0]) / sample_rate

    plt.figure(figsize=(10, 6))
    plt.plot(time, data)
    plt.title(f'Waveform #{file_no}')
    plt.xlabel('Time [s]')
    plt.ylabel('Amplitude')
    plt.grid()
    plt.savefig(f'./raw_data/waveform_{file_no}.png')


def get_wav_file_info(file_path: str, file_no: int) -> None: 
    with wave.open(r'{}'.format(file_path)) as wav_file:
        # Extracting the parameters
        num_channels = wav_file.getnchannels()
        sample_width = wav_file.getsampwidth()
        frame_rate = wav_file.getframerate()
        num_frames = wav_file.getnframes()
        compression_type = wav_file.getcomptype()
        compression_name = wav_file.getcompname()
        
        # Reading frames
        frames = wav_file.readframes(num_frames)
        
        print(f'File #{file_no}:')
        print(f'Number of Channels: {num_channels}')
        print(f'Sample Width (bytes): {sample_width}')
        print(f'Frame Rate (Hz): {frame_rate}')
        print(f'Number of Frames: {num_frames}')
        print(f'Compression Type: {compression_type}')
        print(f'Compression Name: {compression_name}')
        print(f'First 100 bytes of Data: {frames[:100]}')
        print(f'------------------ End ------------------\n\n')

def main(): 
    file_no = 0
    for file_path in DATA_PATH.iterdir():
        if file_no == 5: 
            break
        if file_path.is_file():
            file_no += 1
            get_wav_file_info(file_path, file_no)
            plot_wav(file_path, file_no)
    # print(f'Done plotting {file_no} files')


if __name__ == '__main__':
    main()
