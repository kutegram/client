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
        return QString();
    }

    QString html = message["message"].toString();
    TVector entities = message["entities"].toList();

    for (qint32 i = 0; i < entities.size(); ++i) {
        TObject entity = entities[i].toMap();
        qint32 offset = entity["offset"].toInt();
        qint32 width = entity["width"].toInt();

        qDebug() << entity;

        //messageEntityMentionName -> user_id:long
        //inputMessageEntityMentionName -> user_id:InputUser

        switch (ID(entity)) {
        case MessageEntityUnknown:
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
            break;
        case MessageEntityItalic:
            break;
        case MessageEntityCode:
            break;
        case MessageEntityPre:
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
            break;
        case MessageEntityStrike:
            break;
        case MessageEntityBlockquote:
            break;

        case MessageEntityBankCard:
            break;

        case MessageEntitySpoiler:
            break;
        }
    }

    return html;
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
        return QSize(doc.size().toSize());

    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);

    doc.drawContents(painter);

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
