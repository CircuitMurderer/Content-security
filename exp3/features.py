import librosa.display
import numpy as np
import matplotlib.pyplot as plt


if __name__ == '__main__':
    wave, _ = librosa.load('data/test.wav')

    plt.figure(figsize=(12, 6))
    plt.subplot(121)
    spec = librosa.amplitude_to_db(np.abs(librosa.stft(wave)), ref=np.max)
    librosa.display.specshow(spec, y_axis='linear', x_axis='time')
    plt.colorbar(format='%+2.0f dB')
    plt.title('Lin-frequency power spectrogram')

    plt.subplot(122)
    mfcc = librosa.feature.mfcc(y=wave)
    librosa.display.specshow(mfcc, x_axis='time')
    plt.title('MFCC')

    plt.savefig('fig.jpg')
    plt.show()
