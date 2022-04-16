import os
import re

import jieba.analyse
import matplotlib.pyplot as plt
import numpy as np
import torch

from gensim.models import word2vec, Word2Vec
from tqdm import tqdm


class Model:
    def __init__(self):
        self.root_path = './data'
        self.class_list = os.listdir(self.root_path)
        self.all_txt_list = []
        self.class_all_words = {}
        self.model = None

        self.read_dataset()
        self.build_model()
        self.train_freq(rate)

    def read_dataset(self):
        for item in self.class_list:
            class_path = self.root_path + '/' + item
            file_list = os.listdir(class_path)
            for fi in file_list:
                filename = class_path + '/' + fi
                with open(filename, 'r', encoding='utf8') as f:
                    cont = f.read()
                cont = "".join(cont.split())
                word_list = jieba.analyse.textrank(cont, topK=None,
                                                   withWeight=False,
                                                   allowPOS=('n', 'ns', 'v', 'vn'))
                if len(word_list) != 0:
                    self.all_txt_list.extend(word_list)

        result = " ".join(self.all_txt_list)
        with open('result.txt', 'w', encoding='utf8') as f:
            f.write(result)

    def build_model(self):
        if os.path.exists('w2v_model.mdl'):
            self.model = Word2Vec.load('w2v_model.mdl')
            return
        sentences = word2vec.Text8Corpus("result.txt")
        model = word2vec.Word2Vec(sentences, vector_size=200, min_count=1)
        model.save('w2v_model.mdl')
        self.model = model

    def train_freq(self, rate_train):
        print('\nCal train-set freq...')
        for c in tqdm(self.class_list):
            all_words = {}
            class_path = self.root_path + '/' + c
            file_list = os.listdir(class_path)
            n_file = len(file_list)

            for name in file_list[:int(n_file * rate_train)]:
                file_path = class_path + '/' + name
                with open(file_path, 'r', encoding='utf8') as f:
                    cont = f.read()
                    cont = re.sub('[ \t\n]*', '', cont)
                    cont = "".join(cont.split())
                word_list = jieba.analyse.textrank(cont, topK=None,
                                                   withWeight=False,
                                                   allowPOS=('n', 'ns', 'v', 'vn'))
                if len(word_list) != 0:
                    for word in word_list:
                        if word in all_words.keys():
                            all_words[word] += 1
                        else:
                            all_words[word] = 1

            self.class_all_words[c] = all_words

    def avg_class_vec(self, key_num):
        avg_classes = {}
        for c in self.class_all_words:
            all_words_list = sorted(self.class_all_words[c].items(),
                                    key=lambda item: item[1], reverse=True)
            shp = self.model.wv[all_words_list[0][0]].shape
            total = np.zeros(shp[0])
            limit = min(key_num, len(all_words_list))

            for t in range(limit):
                total += self.model.wv[all_words_list[t][0]]
            avg_classes[c] = total / limit

        return avg_classes

    def accuracy(self, key_num, rate_test, avg_classes):
        true = 0
        false = 0
        print('\nKeyword_num: {}'.format(key_num))
        for c in tqdm(self.class_list):
            class_path = self.root_path + '/' + c
            file_list = os.listdir(class_path)
            n_file = len(file_list)

            for name in file_list[int(n_file * rate_test):]:
                file_path = class_path + '/' + name
                test_data_words = {}
                with open(file_path, 'r', encoding='utf8') as f:
                    txt = f.read()
                    txt = re.sub('[ \t\n]*', '', txt)
                txt = "".join(txt.split())
                word_list = jieba.analyse.textrank(txt, topK=None,
                                                   withWeight=False,
                                                   allowPOS=('n', 'ns', 'v', 'vn'))
                for word in word_list:
                    if word in test_data_words.keys():
                        test_data_words[word] += 1
                    else:
                        test_data_words[word] = 1
                test_words_list = sorted(test_data_words.items(),
                                         key=lambda item: item[1], reverse=True)
                if len(test_words_list) == 0:
                    continue

                shp = self.model.wv[test_words_list[0][0]].shape
                total = np.zeros(shp[0])
                limit = min(key_num, len(test_words_list))

                for t in range(limit):
                    total += self.model.wv[test_words_list[t][0]]
                average_test_vector = total / limit
                pre = predict(avg_classes, data=average_test_vector)
                if pre == c:
                    true += 1
                else:
                    false += 1

        return true / (true + false)


def predict(avg_classes, data):
    sim = {}
    for c in avg_classes:
        sim[c] = cos_sim(data, avg_classes[c])
    test_word_list = sorted(sim.items(), key=lambda item: item[1], reverse=True)
    return test_word_list[0][0]


def cos_sim(v_a, v_b):
    a_n, b_n = np.linalg.norm(v_a), np.linalg.norm(v_b)
    return np.dot(v_a, v_b) / (a_n * b_n)


if __name__ == '__main__':
    plt.rcParams['font.sans-serif'] = ['SimHei']
    plt.rcParams['axes.unicode_minus'] = False

    if not os.path.exists('./data'):
        raise FileNotFoundError('No dataset.')

    np.random.seed(100)
    torch.cuda.manual_seed(100)

    keyword_num_list = [1, 3, 5, 10, 15, 20, 30, 50, 100]
    rate = 0.7
    mdl = Model()

    print('\nCal accuracy...')
    acc_list = []
    for keyword_num in keyword_num_list:
        average_class_dic = mdl.avg_class_vec(keyword_num)
        res = mdl.accuracy(keyword_num, rate, average_class_dic)
        acc_list.append(round(res, 3))

    fig = plt.figure()
    ax = fig.add_subplot(1, 1, 1)
    ax.set_title('关键词个数与准确率间的关系')
    ax.set_xlabel('关键词个数')
    ax.set_ylabel('准确率')
    plt.plot(keyword_num_list, acc_list, color='black',
             markerfacecolor='r', marker='o')

    for a, b in zip(keyword_num_list, acc_list):
        plt.text(a, b, (a, b), ha='center',
                 va='bottom', fontsize=10)
    plt.show()
