TARGET = Kutegram

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

# Add files and directories to ship with the application 
# by adapting the examples below.
# file1.source = myfile
# dir1.source = mydir
DEPLOYMENTFOLDERS = # file1 dir1

QT += webkit

QMAKE_LFLAGS.ARMCC += --rw-base 0xC00000
QMAKE_LFLAGS.GCCE += -Tdata 0xC00000

symbian:TARGET.UID3 = 0xE0713D51

# Smart Installer package's UID
# This UID is from the protected range 
# and therefore the package will fail to install if self-signed
# By default qmake uses the unprotected range value if unprotected UID is defined for the application
# and 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# Allow network access on Symbian
symbian:TARGET.CAPABILITY = ReadUserData WriteUserData UserEnvironment NetworkServices LocalServices
symbian:ICON = kutegram.svg

# If your application uses the Qt Mobility libraries, uncomment
# the following lines and add the respective components to the 
# MOBILITY variable. 
# CONFIG += mobility
# MOBILITY +=

SOURCES += main.cpp mainwindow.cpp \
    logintypedialog.cpp \
    qrlogindialog.cpp \
    dialogitemmodel.cpp \
    dialogitemdelegate.cpp \
    flickcharm.cpp \
    historyitemdelegate.cpp \
    historyitemmodel.cpp \
    historywindow.cpp \
    historyview.cpp \
    avatars.cpp
HEADERS += mainwindow.h \
    logintypedialog.h \
    qrlogindialog.h \
    dialogitemmodel.h \
    dialogitemdelegate.h \
    flickcharm.h \
    historyitemdelegate.h \
    historyitemmodel.h \
    historywindow.h \
    historyview.h \
    main.h \
    avatars.h
FORMS += mainwindow.ui \
    logintypedialog.ui \
    qrlogindialog.ui \
    historywindow.ui
TRANSLATIONS += translations/kutegram_en.ts \
    translations/kutegram_ru.ts

# Please do not modify the following two lines. Required for deployment.
include(deployment.pri)
qtcAddDeployment()

include(libqrencode/libqrencode.pri)
include(library/library.pri)

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
    stylesheet.css

RESOURCES += \
    resources.qrc \
    translations.qrc
