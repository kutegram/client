#ifndef AVATARS_H
#define AVATARS_H

#include <QPixmap>
#include <QColor>

namespace Avatars {

QPixmap generateThumbnail(qint64 id, QString name, qint32 size);
QColor userColor(qint64 id);

}

#endif // AVATARS_H
