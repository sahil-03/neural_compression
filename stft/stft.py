import pathlib 
import IPython
import os
import numpy as np
import librosa as lr
import soundfile as sf
import matplotlib.pyplot as plt

from sklearn.decomposition import TruncatedSVD
from scipy.io import wavfile
from typing import Tuple

from scipy.signal import stft
from scipy.io import wavfile

DATA_PATH = pathlib.Path(__file__).parent.parent / 'data/'
STFT_PATH = pathlib.Path(__file__).parent.parent /'stft/'


def compress(imageIn, n_components=100, random_seed=0):
    
    image = imageIn
    
    if len (image.shape) != 3:
        raise('not sure what image type this')

    if image.shape[2] == 2:
        image_type = 'real_imaginary'
        
    if image.shape[2] == 3:
        image_type = 'RGB'

    print(f'Found {image_type} image')
            
    n_of_layers = image.shape[2]

    print(n_of_layers)
    
    compressed_list = []
    for layer in range(n_of_layers):
        image_layer = image[:,:,layer]
        clf = TruncatedSVD(n_components=n_components)
        clf.fit(image_layer)
        compressed_layer = clf.inverse_transform(clf.transform(image_layer))
        compressed_list.append(compressed_layer)
        
        
    compressed = np.stack(compressed_list, axis=2)    
    
    if image_type =='RGB':
        compressed = np.clip(compressed, a_min=0, a_max=255)
        compressed = np.array(compressed, dtype=image.dtype)
    compressed = compressed.reshape(imageIn.shape)

    SHAPE = compressed.shape[:2]
    print('shape:', SHAPE)

    original_mem = np.prod(SHAPE)
    compressed_mem = (1 + np.sum(SHAPE) ) * n_components
    print(f'The compressed memory is roughly \
      {100 * compressed_mem / original_mem:0.0f}% of the original')
    
    return compressed


def compress_complex(complex_image, n_components=100,random_seed=0, doplot=False):
    real = np.real(complex_image)
    imaginary = np.imag(complex_image)

    image_ri = np.stack([real, imaginary], axis=2) 
    compressed_image_ri = compress(image_ri, n_components, random_seed)

    compressed_real = compressed_image_ri[:,:,0]
    compressed_imaginary = compressed_image_ri[:,:,1]
    
    return compressed_real + 1j * compressed_imaginary


def load_wav_file(file_path: str) -> Tuple[np.ndarray, int]: 
    data, sample_rate = lr.load(file_path)
    return data, sample_rate
    

def plot_spectrogram(data, sr: int, save_to: str): 
    D = np.abs(lr.stft(data))
    D_db = lr.amplitude_to_db(D, ref=np.max)

    plt.figure(figsize=(10, 6))
    lr.display.specshow(D_db, sr=sr, x_axis='time', y_axis='log')
    plt.colorbar(format='%+2.0f dB')
    plt.title('Spectrogram')
    plt.xlabel('Time [s]')
    plt.ylabel('Frequency [Hz]')
    plt.savefig(save_to)


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

    output_path = os.path.join(STFT_PATH, f'waveform_{file_no}.png')
    plt.savefig(output_path)



def main(): 
    file_path = '/Users/sahil/Documents/neuralink_compression/data/0ab237b7-fb12-4687-afed-8d1e2070d621.wav'
    
    data, sr = load_wav_file(file_path)

    print(len(data))

    s = lr.stft(data, n_fft=1024, hop_length=512)
    plot_spectrogram(data, sr, 'orig.png') 

    print(s.shape)

    f, t, Zxx = stft(data, sr, nperseg=1024, noverlap=512)
    print(f.shape, t.shape, Zxx.shape)
    



if __name__ == '__main__':
    main()