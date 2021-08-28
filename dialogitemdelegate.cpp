#include "dialogitemdelegate.h"

#include <QPainter>
#include <QApplication>

DialogItemDelegate::DialogItemDelegate(QObject *parent) :
    QAbstractItemDelegate(parent)
{
}

QString getAvatarText(QString title)
{
    QStringList split = title.split(" ", QString::SkipEmptyParts);
    QString result;

    for (qint32 i = 0; i < split.size(); ++i) {
        if (result.size() > 1) break;
        QString item = split[i];
        if (item.isEmpty() || !item[0].isLetterOrNumber()) continue;
        result += item[0].toUpper();
    }

    if (result.isEmpty() && !title.isEmpty()) result += title[0].toUpper();

    return result;
}

void DialogItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //TODO
    if (!index.isValid()) return;

    qint32 x = option.rect.x();
    qint32 y = option.rect.y();
    qint32 w = option.rect.width();
    qint32 h = option.rect.height();

    QFontMetrics fm = option.fontMetrics;
    QFont font = option.font;

    qint32 padding = 4;
    qint32 fontHeight = fm.height();
    qint32 avatarSize = padding + fontHeight + padding + fontHeight + padding;
    qint32 height = padding + avatarSize + padding;

    QString title = index.data().toString();

    x += padding;
    y += padding;

    QColor circleColor;
    QRect circleRect(x, y, avatarSize, avatarSize);
    circleColor.setHsl(index.data(Qt::UserRole).toInt() % 360, 140, 140);
    painter->setBrush(circleColor);
    painter->setPen(circleColor);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawEllipse(circleRect);

    QFont circleFont = option.font;
    circleFont.setBold(true);
    circleFont.setPixelSize(avatarSize / 3);
    painter->setFont(circleFont);
    painter->setPen(Qt::white);
    painter->drawText(circleRect, Qt::AlignHCenter | Qt::AlignVCenter, getAvatarText(title), &circleRect);

    x += avatarSize + padding + padding;
    y += padding;

    painter->setPen(QApplication::palette().text().color());

    font.setBold(true);
    painter->setFont(font);
    painter->drawText(x, y, w, h, Qt::TextSingleLine, fm.elidedText(title, Qt::ElideRight, w - x - padding));

    y += fontHeight + padding;

    font.setBold(false);
    painter->setFont(font);
    painter->drawText(x, y, w, h, Qt::TextSingleLine, fm.elidedText("<нет сообщения>", Qt::ElideRight, w - x - padding)); //TODO
}

QSize DialogItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) return option.rect.size();

    QFontMetrics fm = option.fontMetrics;

    qint32 padding = 4;
    qint32 fontHeight = fm.height();

    return QSize(option.rect.width(), padding + padding + fontHeight + padding + fontHeight + padding + padding);
}
