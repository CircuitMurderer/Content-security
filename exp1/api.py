import os
import json
import time
import requests
from collections import Counter

access_token = ''
if os.path.exists('token.txt'):
    with open('token.txt', 'r', encoding='utf-8') as f:
        access_token = f.read().strip('\n')
    print('Token read from file:', access_token)

else:
    AK = 'h6Rb7Ulx7IR09ypwByXQ8jWd'
    SK = 'TpQGUNbi1VRGdGb20gI3Oj5H5UdSl2tH'
    host = 'https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials' \
           '&client_id={}&client_secret={}'.format(AK, SK)

    res = requests.get(host)
    if not res:
        raise ValueError('Response is null.')

    with open('token.txt', 'w', encoding='utf-8') as f:
        access_token = res.json()['access_token']
        f.write(access_token)
    print('Token get from server', access_token)

if not os.path.exists('verified.json'):
    req_url = 'https://aip.baidubce.com/rest/2.0/solution/v1/text_censor/v2/user_defined' \
              '?access_token=' + access_token
    headers = {'content-type': 'application/x-www-form-urlencoded'}
    with open('comments.json', 'r', encoding='utf-8') as f:
        data = json.load(f)

    for posting in data:
        verify = []
        for comm in posting['comments']:
            params = {'text': comm}
            res = requests.post(req_url, data=params, headers=headers)
            verify.append(res.json()['conclusion'])
            time.sleep(0.5)
        posting['verify'] = verify

    with open('verified.json', 'w', encoding='utf-8') as f:
        json.dump(data, f, ensure_ascii=False)

with open('verified.json', 'r', encoding='utf-8') as f:
    data = json.load(f)

verified = []
for posting in data:
    verified.extend(posting['verify'])

counter = Counter(verified)
summary = sum(counter.values())
print('合规言论占比：{:.2f}%'.format(counter['合规'] * 100 / summary))
print('不合规言论占比：{:.2f}%'.format(counter['不合规'] * 100 / summary))
print('疑似言论占比：{:.2f}%'.format(counter['疑似'] * 100 / summary))
