#ifndef MAIN_H
#define MAIN_H

#include <QString>

class QWidget;
class QMainWindow;

enum ScreenOrientation {
    ScreenOrientationLockPortrait,
    ScreenOrientationLockLandscape,
    ScreenOrientationAuto
};

enum ScreenOrientationX11
{
    Landscape = 0,
    Portrait = 270,
    LandscapeInverted = 180,
    PortraitInverted = 90
};

void setOrientation(QMainWindow* window, ScreenOrientation orientation);
void showExpanded(QMainWindow* window);

void writeX11OrientationAngleProperty(QWidget* widget, ScreenOrientationX11 orientation = Portrait);

void showAvkonPopup(QString title, QString message);

#endif // MAIN_H
