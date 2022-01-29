import requests
import sys

url = 'https://api.telegram.org/bot' + sys.argv[1] + '/sendDocument'

f = {
    'document': (sys.argv[2], open(sys.argv[2], 'rb')),
    'caption': (None, """*%s* pushed commit:
                         [%s](https://github.com/$s/commit/$s): `$s`
                         
                         #kutegram #symbian""" % (sys.argv[3], sys.argv[4], sys.argv[5], sys.argv[4], sys.argv[6])),
    'chat_id': (None, sys.argv[7]),
    'parse_mode': (None, 'MarkdownV2'),
    'disable_notification': (None, 'false'),
    'protect_content': (None, 'false')
}
r = requests.post(url, files=f)
print(r)
