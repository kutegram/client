#ifndef MESSAGES_H
#define MESSAGES_H

#include "tl.h"

QString peerNameToHtml(TObject peer);
QString replyToHtml(TObject reply, TObject replyPeer);
QString messageToHtml(TObject message);

#endif // MESSAGES_H
