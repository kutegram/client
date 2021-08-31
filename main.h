#ifndef MAIN_H
#define MAIN_H

class QMainWindow;

enum ScreenOrientation {
    ScreenOrientationLockPortrait,
    ScreenOrientationLockLandscape,
    ScreenOrientationAuto
};

void setOrientation(QMainWindow* window, ScreenOrientation orientation);
void showExpanded(QMainWindow* window);

#endif // MAIN_H
