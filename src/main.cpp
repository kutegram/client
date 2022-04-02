#include "mainwindow.h"

#include "main.h"
#include <QtGui/QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QFile>
#include <QTextStream>
#include "devicehelper.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setApplicationVersion("0.0.2");
    QApplication::setApplicationName("Kutegram");
    QApplication::setOrganizationName("curoviyxru");
    QApplication::setOrganizationDomain("kg.curoviyx.ru");

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load(":/translations/kutegram_" + QLocale::system().name());
    app.installTranslator(&myappTranslator);

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5)
    QFile stylesheetFile(":/styles/stylesheet.css");
    if (stylesheetFile.open(QFile::ReadOnly)) {
        app.setStyleSheet(QTextStream(&stylesheetFile).readAll());
        stylesheetFile.close();
    }
#endif

    MainWindow mainWindow;
    setOrientation(&mainWindow, ScreenOrientationAuto);
    showExpanded(&mainWindow);

//    foreach (QWidget* widget, QApplication::allWidgets())
//        widget->setContextMenuPolicy(Qt::NoContextMenu);

//    app.setQuitOnLastWindowClosed(false);

    return app.exec();
}

void setOrientation(QMainWindow* window, ScreenOrientation orientation)
{
#if defined(Q_OS_SYMBIAN)
    // If the version of Qt on the device is < 4.7.2, that attribute won't work
    if (orientation != ScreenOrientationAuto) {
        const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
        if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
            qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
            return;
        }
    }
#endif // Q_OS_SYMBIAN

    Qt::WidgetAttribute attribute;
    switch (orientation) {
#if QT_VERSION < 0x040702
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
    case ScreenOrientationLockPortrait:
        attribute = static_cast<Qt::WidgetAttribute>(128);
        break;
    case ScreenOrientationLockLandscape:
        attribute = static_cast<Qt::WidgetAttribute>(129);
        break;
    default:
    case ScreenOrientationAuto:
        attribute = static_cast<Qt::WidgetAttribute>(130);
        break;
#else // QT_VERSION < 0x040702
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_LockPortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_LockLandscapeOrientation;
        break;
    default:
    case ScreenOrientationAuto:
        attribute = Qt::WA_AutoOrientation;
        break;
#endif // QT_VERSION < 0x040702
    };
    window->setAttribute(attribute, true);
}

void showExpanded(QMainWindow* window)
{
    writeX11OrientationAngleProperty(window);
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR) || defined(Q_WS_MAEMO_5)
    window->showMaximized();
#else
    window->show();
#endif
}
