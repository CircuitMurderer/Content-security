import requests
import json
from bs4 import BeautifulSoup


class TiebaSpider:
    def __init__(self, tieba_name):
        self.tieba_name = ''
        self.tieba_url = 'https://tieba.baidu.com/f?kw={}&ie=utf-8&pn=0'
        self.tieba_base = 'https://tieba.baidu.com'
        self.encode = 'utf-8'
        self.set_url(tieba_name)
        self.headers = {}
        self.postings = []

    def set_url(self, tieba_name):
        self.tieba_name = tieba_name
        self.tieba_url = self.tieba_url.format(tieba_name)

    def content(self, url=''):
        url = self.tieba_url if url == '' else url
        return requests \
            .get(url, headers=self.headers) \
            .content \
            .decode(self.encode)


if __name__ == '__main__':
    spider = TiebaSpider('抗压背锅')
    soup = BeautifulSoup(spider.content(), features="lxml")

    for a in soup.find_all('a', attrs={'class': 'j_th_tit'})[2:]:
        spider.postings.append(spider.tieba_base + a['href'])

    res = []
    for h in spider.postings:
        soup = BeautifulSoup(spider.content(h), features="lxml")
        comments = []
        for div in soup.find_all('div', attrs={'class': 'l_post l_post_bright j_l_post clearfix'}):
            comm = div.find('div', attrs={'class': 'd_post_content j_d_post_content'}).text.strip()
            if comm != '':
                comments.append(comm)
        posting = {
            'title': soup.find('div', attrs={'id': 'j_core_title_wrap'}).text.strip().split(' ')[0],
            'comments': comments
        }
        res.append(posting)

    with open('comments.json', 'w', encoding='utf-8') as f:
        json.dump(res, f, ensure_ascii=False)
