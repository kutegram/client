#ifndef MAIN_H
#define MAIN_H

#include <QString>

class QMainWindow;

enum ScreenOrientation {
    ScreenOrientationLockPortrait,
    ScreenOrientationLockLandscape,
    ScreenOrientationAuto
};

void setOrientation(QMainWindow* window, ScreenOrientation orientation);
void showExpanded(QMainWindow* window);

void showAvkonPopup(QString title, QString message);

#endif // MAIN_H
