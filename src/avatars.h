#ifndef AVATARS_H
#define AVATARS_H

#include <QPixmap>
#include <QColor>

namespace Avatars {

QPixmap generateThumbnail(qint64 id, QString name, qint32 size, qint32 fontSize = 0);
QColor userColor(qint64 id);

}

#endif // AVATARS_H
