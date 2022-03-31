APPNAME = kutegram
TARGET = kutegram
TEMPLATE = app

QT += xml

VERSION = 0.0.2
DATE = $$system(date /t)
DEFINES += VERSION=\"\\\"$$VERSION\\\"\"
DEFINES += BUILDDATE=\"\\\"$$DATE\\\"\"
COMMIT_SHA = $$system(git log --pretty=format:%h -n 1);
DEFINES += COMMIT_SHA=\"\\\"$$COMMIT_SHA\\\"\"

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

DEFINES += QT_USE_FAST_CONCATENATION QT_USE_FAST_OPERATOR_PLUS

symbian: include(rc/platforms/symbian.pri)
unix:!macx:!android:!haiku:!symbian: include(rc/platforms/nix.pri)
# qtcAddDeployment()

include(src/library/library.pri)
include(src/slidingstackedwidget/SlidingStackedWidget.pri)

INCLUDEPATH += src

SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/dialogitemmodel.cpp \
    src/dialogitemdelegate.cpp \
    src/flickcharm.cpp \
    src/historywindow.cpp \
    src/avatars.cpp \
    src/messages.cpp \
    src/messagelabel.cpp

HEADERS += src/mainwindow.h \
    src/dialogitemmodel.h \
    src/dialogitemdelegate.h \
    src/flickcharm.h \
    src/historywindow.h \
    src/main.h \
    src/avatars.h \
    src/messages.h \
    src/messagelabel.h

FORMS += src/mainwindow.ui \
    src/historywindow.ui

OTHER_FILES += rc/stylesheet.css

RESOURCES += rc/resources.qrc \
    rc/translations.qrc

include(rc/locale.pri)
