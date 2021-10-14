#ifndef AVATARS_H
#define AVATARS_H

#include <QPixmap>

namespace Avatars {

QPixmap generateThumbnail(qint64 id, QString name, qint32 size);

}

#endif // AVATARS_H
