#include "avatars.h"

#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QStyle>

QColor Avatars::userColor(qint64 id) {
   return QColor::fromHsl(id % 360, 140, 140);
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

QPixmap Avatars::generateThumbnail(qint64 id, QString name, qint32 size)
{
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&pixmap);

    QRect circleRect(0, 0, size, size);

    QColor circleColor = userColor(id);
    painter.setBrush(circleColor);
    painter.setPen(circleColor);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawEllipse(circleRect);

    QFont font;
    font.setBold(true);
    font.setPixelSize(size / 3);
    painter.setFont(font);
    painter.setPen(Qt::white);
    painter.drawText(circleRect, Qt::AlignHCenter | Qt::AlignVCenter, getAvatarText(name), &circleRect);

    painter.end();
    return pixmap;
}
