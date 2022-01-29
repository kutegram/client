import requests
import os

url = 'https://api.telegram.org/bot' + str(os.environ['BOT_TOKEN']) + '/sendDocument'

f = {
    'document': (str(os.environ['UPLOAD_FILE']), open(str(os.environ['UPLOAD_FILE']), 'rb')),
    'caption': (None, """*%s(https://github.com/$s)* pushed commit:
                         [%s](https://github.com/$s/commit/$s): `$s`
                         
                         #kutegram #symbian""" % (str(os.environ['COMMIT_AUTHOR']), str(os.environ['COMMIT_USERNAME']), str(os.environ['COMMIT_SHA'][:7]), str(os.environ['COMMIT_REPO']), str(os.environ['COMMIT_SHA']), str(os.environ['COMMIT_MESSAGE']))),
    'chat_id': (None, str(os.environ['BOT_CHANNEL'])),
    'parse_mode': (None, 'MarkdownV2'),
    'disable_notification': (None, 'false'),
    'protect_content': (None, 'false')
}
r = requests.post(url, files=f)
print(r)
