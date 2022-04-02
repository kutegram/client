#ifndef DEVICEHELPER_H
#define DEVICEHELPER_H

#include <QString>
#include <QUrl>

//TODO: Symbian Vibration Example
//TODO: Maemo Vibration Example
//TODO: desktop notification
//TODO: notifications

enum ScreenOrientationX11
{
    Landscape = 0,
    Portrait = 270,
    LandscapeInverted = 180,
    PortraitInverted = 90
};

bool isInSilentMode();
void showChatIcon();
void hideChatIcon();
void playNotification(QString path);
void setAppHiddenState(bool state);
void writeX11OrientationAngleProperty(QWidget* widget, ScreenOrientationX11 orientation = Portrait);
void notificationBlink(int device);
void showSystemNotificationPopup(QString title, QString message);
void showNotification(QString title, QString message);
void openUrl(const QUrl &url);

#endif // DEVICEHELPER_H
