import os

import librosa
import paddle
import pandas as pd
import numpy as np

from tqdm import tqdm
from paddle import nn
from paddle import Model
from paddle.io import Dataset
from sklearn.model_selection import train_test_split


class DataLoader:
    def __init__(self):
        self.audio_path = 'UrbanSound8K/audio'
        self.data_path = 'data/UrbanSound8K.npy'
        self.info_path = 'UrbanSound8K/metadata/UrbanSound8K.csv'

        self.n_mfcc = 40
        self.data = pd.read_csv(self.info_path)
        self.dataset = self.load_mfcc()

        self.id_to_name = dict(zip(self.data['classID'], self.data['class']))

    def info_sum(self):
        df = self.data
        dist = []
        for i in range(1, 11):
            dist.append(df[df['fold'] == i]['class'].value_counts())

        dist = pd.DataFrame(dist)
        dist = dist.reset_index()
        dist['index'] = ["fold" + str(x) for x in range(1, 11)]
        print(dist.columns.values)

    def class_id_and_full_path(self, filename):
        row = self.data[self.data['slice_file_name'] == filename]
        full_path = '/'.join([self.audio_path, 'fold' + str(row['fold'].values[0]), filename])
        return row['classID'].values[0], full_path

    def load_mfcc(self):
        if os.path.exists(self.data_path):
            print('Numpy array file exists. Loaded.')
            return np.load(self.data_path, allow_pickle=True)

        dataset = []
        for i in tqdm(range(self.data.shape[0])):
            class_id, full_path = self.class_id_and_full_path(self.data['slice_file_name'][i])
            wav, rate = librosa.load(full_path, res_type='kaiser_fast')
            mfcc = np.mean(librosa.feature.mfcc(y=wav, sr=rate, n_mfcc=self.n_mfcc).T, axis=0)
            dataset.append((mfcc, class_id))

        dataset = np.array(dataset, dtype=object)
        np.save(self.data_path, dataset, allow_pickle=True)
        return dataset

    def preprocess(self):
        x = np.array(self.dataset[:, 0].tolist())
        y = np.array(self.dataset[:, 1].tolist())

        x_train, x_test, y_train, y_test = train_test_split(x, y)
        return x_train, y_train, x_test, y_test


class AudioDataset(Dataset):
    def __init__(self, is_train=True):
        super().__init__()

        self.loader = DataLoader()

        data = self.loader.preprocess()
        if is_train:
            self.data = data[0]
            self.label = data[1]
        else:
            self.data = data[2]
            self.label = data[3]

    def __getitem__(self, idx):
        return paddle.to_tensor(self.data[idx]), paddle.to_tensor(self.label[idx])

    def __len__(self):
        return len(self.data)


class AudioNet:
    def __init__(self):
        self.train = AudioDataset()
        self.test = AudioDataset(False)

        self.net = nn.Sequential(
            nn.Linear(40, 128),
            nn.ReLU(),
            nn.Linear(128, 256),
            nn.ReLU(),
            nn.Linear(256, 512),
            nn.ReLU(),
            nn.Linear(512, 256),
            nn.ReLU(),
            nn.Linear(256, 10),
            nn.Sigmoid()
        )

        self.model = Model(self.net)
        self.model.summary((1, 40))

    def fit(self):
        self.model.prepare(paddle.optimizer.Adam(learning_rate=0.001, parameters=self.net.parameters()),
                           paddle.nn.CrossEntropyLoss(),
                           paddle.metric.Accuracy())

        self.model.fit(self.train,
                       self.test,
                       epochs=50,
                       batch_size=256,
                       verbose=1)


if __name__ == '__main__':
    net = AudioNet()
    net.fit()
