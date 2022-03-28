#include "historyitemdelegate.h"

#include <QPainter>
#include <QApplication>
#include <QPixmap>
#include <Qt>
#include <QDebug>
#include <QTextDocument>
#include "tl.h"
#include "avatars.h"
#include "tlschema.h"
#include <QAbstractTextDocumentLayout>

using namespace TLType;

HistoryItemDelegate::HistoryItemDelegate(QObject *parent) :
    QAbstractItemDelegate(parent)
{
}

QString parseHTML(TObject message)
{
    if (ID(message) != Message) {
        qDebug() << "[parseHTML] Invalid object id: object is not a message.";
        return QString();
    }

    QStringList items;
    QList<char> count;

    items << message["message"].toString() + ' ';
    count << true;
    TVector entities = message["entities"].toList();

    for (qint32 i = 0; i < entities.size(); ++i) {
        TObject entity = entities[i].toMap();
        qint32 offset = entity["offset"].toInt();
        qint32 length = entity["length"].toInt();

        QString sTag;
        QString eTag;

        switch (ID(entity)) {
        case MessageEntityUnknown: //just ignore that.
            break;

        case MessageEntityMention:
            break;
        case MessageEntityHashtag:
            break;
        case MessageEntityBotCommand:
            break;
        case MessageEntityUrl:
            break;
        case MessageEntityEmail:
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
        case MessageEntityPre:
            //sTag = "<pre language=\"" + entity["language"].toString() + "\">";
            sTag = "<pre>";
            eTag = "</pre>";
            break;

        case MessageEntityTextUrl:
            break;
        case MessageEntityMentionName:
            break;
        case InputMessageEntityMentionName:
            break;
        case MessageEntityPhone:
            break;
        case MessageEntityCashtag:
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
            break;

        case MessageEntityBankCard:
            break;

        case MessageEntitySpoiler:
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

        //qDebug() << "[DEBUG]" << message["message"].toString().mid(offset, length);

        if (jidStart == -1 || jidEnd == -1) continue;

        QString sItem = items[jidStart];
        items[jidStart] = sItem.mid(0, posStart);
        items.insert(jidStart + 1, sTag);
        items.insert(jidStart + 2, sItem.mid(posStart));
        count.insert(jidStart + 1, false);
        count.insert(jidStart + 2, true);

        if (jidEnd == jidStart) posEnd -= posStart;
        jidEnd += 2;

        QString eItem = items[jidEnd];
        items[jidEnd] = eItem.mid(0, posEnd);
        items.insert(jidEnd + 1, eTag);
        items.insert(jidEnd + 2, eItem.mid(posEnd));
        count.insert(jidEnd + 1, false);
        count.insert(jidEnd + 2, true);
    }

    if (items.last() == " ") items.removeLast();
    else items.last().chop(1);

    return items.join("").replace('\n', "<br>");
}

QSize paintContent(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    qint32 x = option.rect.x();
    qint32 y = option.rect.y();
    qint32 w = option.rect.width();
    qint32 h = option.rect.height();

    QFontMetrics fm = QApplication::fontMetrics();
    QFont font = QApplication::font();

    qint32 cloudPart = fm.height() * 0.4;
    TObject message = index.data().toMap();

    QTextDocument doc;
    doc.setHtml(parseHTML(message));
    doc.setTextWidth(w);
    doc.setDefaultFont(font);

    if (!painter)
        return QSize(w, doc.size().toSize().height());

    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);

    QPixmap htmlPixmap(doc.size().toSize());
    htmlPixmap.fill(Qt::transparent);
    QPainter htmlPainter(&htmlPixmap);
    doc.drawContents(&htmlPainter);
    painter->drawPixmap(x, y, htmlPixmap);

    return QSize();
}

void HistoryItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    paintContent(painter, option, index);
}

QSize HistoryItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return paintContent(0, option, index);
}
