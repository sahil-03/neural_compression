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
    D = lr.amplitude_to_db(np.abs(lr.stft(data)), ref=np.max)
    plt.figure(figsize=(10, 6))
    lr.display.specshow(D, sr=sr, x_axis='time', y_axis='log')
    plt.colorbar(format='%+2.0f dB')
    plt.title('Spectrogram')
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
    file_no = 0
    for file_path in DATA_PATH.iterdir():
        if file_no == 1: 
            break
        
        data, sample_rate = load_wav_file(file_path)

        # Find stft
        stft = lr.stft(data, n_fft=1024, hop_length=512)
        plot_spectrogram(data, sample_rate, 'spectrogram.png')

        # Perform compression
        # compressed = compress_complex(stft, n_components=20)
        U, S, Vt = np.linalg.svd(stft, full_matrices=False)
        
        r = 20
        U_r = U[:, :r]
        S_r = np.diag(S[:r])
        Vt_r = Vt[:r, :]
        X_approx = U_r @ S_r @ Vt_r


        # Find istft
        inverted_stft = lr.istft(X_approx, hop_length= 512)
        plot_spectrogram(inverted_stft, sample_rate, 'recon_spectrogram.png')

        sf.write('reconstructed_audio.wav', inverted_stft, sample_rate)
        plot_wav('reconstructed_audio.wav', 0)


        original_mem = data.nbytes
        compressed_mem = U_r.nbytes + S_r.nbytes + Vt_r.nbytes
        print(f'Compressed memory is ~ {100 * (compressed_mem / original_mem):0.2f}% of the original.')

        file_no += 1
        

if __name__ == '__main__':
    main()