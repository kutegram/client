#include "messages.h"

#include <QStringList>
#include <QDebug>
#include "tlschema.h"
#include "avatars.h"

using namespace TLType;

QString peerNameToHtml(TObject peer)
{
    QString peerName;

    switch (ID(peer)) {
    case TLType::UserEmpty:
    case TLType::User:
        peerName = peer["first_name"].toString() + " " + peer["last_name"].toString();
        break;
    default:
        //this is a chat, probably.
        peerName = peer["title"].toString();
        break;
    }

    peerName = peerName.mid(0, 40) + (peerName.length() > 40 ? "..." : "");

    return "<span style=\"font-weight:bold;color:" + userColor(peer["id"].toLongLong()).name() + "\">" + peerName + "</span>";
}

QString replyToHtml(TObject reply, TObject replyPeer)
{
    if (ID(reply) != Message) {
        qWarning() << "[replyToHtml] Invalid object." << ID(reply);
        return QString();
    }

    QString replyText = reply["message"].toString().replace('\n', " ");

    return "<table><tr><td style=\"background-color:" + userColor(replyPeer["id"].toLongLong()).name() + ";\">&nbsp;</td><td>&nbsp;" + peerNameToHtml(replyPeer) + "<br>&nbsp;" + replyText.mid(0, 40) + (replyText.length() > 40 ? "..." : "") + "</td></tr></table>";
}

QString messageToHtml(TObject message)
{
    if (ID(message) != Message) {
        qWarning() << "[messageToHtml] Invalid object." << ID(message);
        return QString();
    }

    QStringList items;
    QList<char> count;

    QString originalText = message["message"].toString();
    items << QString(originalText).replace('<', "&lt;").replace('>', "&gt;") + ' ';
    count << true;
    TVector entities = message["entities"].toList();

    for (qint32 i = 0; i < entities.size(); ++i) {
        TObject entity = entities[i].toMap();
        qint32 offset = entity["offset"].toInt();
        qint32 length = entity["length"].toInt();
        QString textPart = originalText.mid(offset, length);

        QString sTag;
        QString eTag;

        switch (ID(entity)) {
        case MessageEntityUnknown: //just ignore it.
            //sTag = "<unknown>";
            //eTag = "</unknown>";
            break;

        case MessageEntityMention:
            sTag = "<a href=\"kutegram://profile/" + textPart + "\">";
            eTag = "</a>";
            break;
        case MessageEntityHashtag:
            sTag = "<a href=\"kutegram://search/" + textPart + "\">";
            eTag = "</a>";
            break;
        case MessageEntityBotCommand:
            sTag = "<a href=\"kutegram://execute/" + textPart + "\">";
            eTag = "</a>";
            break;
        case MessageEntityUrl:
            sTag = "<a href=\"" + textPart + "\">";
            eTag = "</a>";
            break;
        case MessageEntityEmail:
            sTag = "<a href=\"mailto:" + textPart + "\">";
            eTag = "</a>";
            break;

        case MessageEntityBold:
            sTag = "<b>";
            eTag = "</b>";
            break;
        case MessageEntityItalic:
            sTag = "<i>";
            eTag = "</i>";
            break;
        case MessageEntityCode:
            sTag = "<code>";
            eTag = "</code>";
            break;
        case MessageEntityPre: //pre causes some issues with paddings
            sTag = "<code language=\"" + entity["language"].toString() + "\">";
            eTag = "</code>";
            break;

        case MessageEntityTextUrl:
            sTag = "<a href=\"" + entity["url"].toString() + "\">";
            eTag = "</a>";
            break;
        case MessageEntityMentionName:
            sTag = "<a href=\"kutegram://profile/" + entity["user_id"].toString() + "\">";
            eTag = "</a>";
            break;
        case InputMessageEntityMentionName:
            sTag = "<a href=\"kutegram://profile/" + getPeerId(entity["user_id"].toMap()).toString() + "\">";
            eTag = "</a>";
            break;
        case MessageEntityPhone:
            sTag = "<a href=\"tel:" + textPart + "\">";
            eTag = "</a>";
            break;
        case MessageEntityCashtag:
            sTag = "<a href=\"kutegram://search/" + textPart + "\">";
            eTag = "</a>";
            break;

        case MessageEntityUnderline:
            sTag = "<u>";
            eTag = "</u>";
            break;
        case MessageEntityStrike:
            sTag = "<s>";
            eTag = "</s>";
            break;
        case MessageEntityBlockquote:
            sTag = "<blockquote>";
            eTag = "</blockquote>";
            break;

        case MessageEntityBankCard:
            sTag = "<a href=\"kutegram://card/" + textPart + "\">";
            eTag = "</a>";
            break;

        case MessageEntitySpoiler:
            sTag = "<a href=\"kutegram://spoiler/" + QString::number(i) + "\" style=\"background-color:gray;color:gray;\">";
            eTag = "</a>";
            break;
        }

        qint32 posStart = 0;
        qint32 jidStart = -1;
        qint32 posEnd = 0;
        qint32 jidEnd = -1;
        for (qint32 j = 0; j < items.size(); ++j) {
            if (!count[j]) continue;

            posStart += items[j].length();
            posEnd += items[j].length();
            if (posStart > offset && jidStart == -1) {
                posStart = items[j].length() - posStart + offset;
                jidStart = j;
            }
            if (posEnd > offset + length && jidEnd == -1) {
                posEnd = items[j].length() - posEnd + offset + length;
                jidEnd = j;
            }
            if (jidStart != -1 && jidEnd != -1) break;
        }

        if (jidStart == -1 || jidEnd == -1) continue;

        QString sItem = items[jidStart];
        items[jidStart] = sItem.mid(0, posStart);
        items.insert(jidStart + 1, sTag);
        count.insert(jidStart + 1, false);
        items.insert(jidStart + 2, sItem.mid(posStart));
        count.insert(jidStart + 2, true);

        if (jidEnd == jidStart) posEnd -= posStart;
        jidEnd += 2;

        QString eItem = items[jidEnd];
        items[jidEnd] = eItem.mid(0, posEnd);
        items.insert(jidEnd + 1, eTag);
        count.insert(jidEnd + 1, false);
        items.insert(jidEnd + 2, eItem.mid(posEnd));
        count.insert(jidEnd + 2, true);
    }

    if (items.last() == " ") items.removeLast();
    else items.last().chop(1);

    return items.join("").replace('\n', "<br>");
}
