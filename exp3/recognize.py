from aip import AipSpeech


if __name__ == '__main__':
    print(
        'Recognize result:',
        AipSpeech(
            appId='26127211',
            apiKey='VLrjADiUbkYYIQhb5l8nqjaD',
            secretKey='giOR6riRLjspSwgzMGo6qEy0Gh14ou47'
        ).asr(
            (lambda p: open(p, 'rb').read())('data/recAudio.pcm'),
            format='pcm',
            rate=16000,
            options={
                'dev_pid': 1537
            }
        )["result"][0]
    )
