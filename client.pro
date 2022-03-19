TARGET = Kutegram
TEMPLATE = app

VERSION = 0.0.2
DATE = $$system(date /t)
DEFINES += VERSION=\"\\\"$$VERSION\\\"\"
DEFINES += BUILDDATE=\"\\\"$$DATE\\\"\"
COMMIT_SHA = $$system(git log --pretty=format:%h -n 1);
DEFINES += COMMIT_SHA=\"\\\"$$COMMIT_SHA\\\"\"

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

QT += webkit svg
DEFINES += QT_USE_FAST_CONCATENATION QT_USE_FAST_OPERATOR_PLUS

symbian {
    TARGET.UID3 = 0xE0713D51
    DEFINES += SYMBIAN_UID=$$TARGET.UID3

    TARGET.EPOCHEAPSIZE = 0x40000 0x4000000
    TARGET.EPOCSTACKSIZE = 0x8000
    TARGET.EPOCALLOWDLLDATA = 1

    contains(SYMBIAN_VERSION, Symbian3) {
        DEFINES += SYMBIAN3_READY
    }

#    QMAKE_CXXFLAGS.CW += -O2
#    QMAKE_LFLAGS.ARMCC += --rw-base 0xC00000
#    QMAKE_LFLAGS.GCCE += -Tdata 0xC00000

    vendorinfo = \
        "%{\"curoviyxru\"}" \
        ":\"curoviyxru\""

    platform_product_id = S60ProductID

    pkg_platform_dependencies_custom = \
        "; Default HW/platform dependencies" \
        "[0x20022E6D],0,0,0,{\"$$platform_product_id\"}"

    pkg_platform_dependencies_custom += \
        "[0x102032BE],0,0,0,{\"$$platform_product_id\"}" \
        "[0x102752AE],0,0,0,{\"$$platform_product_id\"}" \
        "[0x1028315F],0,0,0,{\"$$platform_product_id\"}" \
        " "

    myDeployment.pkg_prerules = vendorinfo pkg_platform_dependencies_custom
    DEPLOYMENT += myDeployment

    TARGET.CAPABILITY = ReadUserData WriteUserData UserEnvironment NetworkServices LocalServices
    ICON = kutegram.svg

    LIBS += -lavkon \
            -laknnotify \
            -lhwrmlightclient \
            -lapgrfx \
            -lcone \
            -lws32 \
            -lbitgdi \
            -lfbscli \
            -laknskins \
            -laknskinsrv \
            -leikcore \
            -lapmime \
            -lefsrv \
            -leuser \
            -lcommondialogs \
            -lesock \
            -lmediaclientaudio \
            -lprofileengine \
            -lcntmodel \
            -lbafl \
            -lmgfetch
}

# If your application uses the Qt Mobility libraries, uncomment
# the following lines and add the respective components to the 
# MOBILITY variable. 
# CONFIG += mobility
# MOBILITY +=

SOURCES += main.cpp mainwindow.cpp \
    dialogitemmodel.cpp \
    dialogitemdelegate.cpp \
    flickcharm.cpp \
    historyitemdelegate.cpp \
    historyitemmodel.cpp \
    historywindow.cpp \
    historyview.cpp \
    avatars.cpp \
    messagedocument.cpp

HEADERS += mainwindow.h \
    dialogitemmodel.h \
    dialogitemdelegate.h \
    flickcharm.h \
    historyitemdelegate.h \
    historyitemmodel.h \
    historywindow.h \
    historyview.h \
    main.h \
    avatars.h \
    messagedocument.h

FORMS += mainwindow.ui \
    historywindow.ui

TRANSLATIONS += translations/kutegram_en.ts \
    translations/kutegram_es.ts \
    translations/kutegram_ru.ts \
    translations/kutegram_uk.ts

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
    qtc_packaging/debian_fremantle/rules \
    qtc_packaging/debian_fremantle/README \
    qtc_packaging/debian_fremantle/copyright \
    qtc_packaging/debian_fremantle/control \
    qtc_packaging/debian_fremantle/compat \
    qtc_packaging/debian_fremantle/changelog \
    stylesheet.css

RESOURCES += \
    resources.qrc \
    translations.qrc

# include(libqrencode/libqrencode.pri)
include(library/library.pri)
include(SlidingStackedWidget/SlidingStackedWidget.pri)

include(deployment.pri)
qtcAddDeployment()

maemo5 {
    desktopfile.files = client.desktop
    desktopfile.path = /usr/share/applications/hildon
    INSTALLS += desktopfile

    icon.files = client64.png
    icon.path = /usr/share/icons/hicolor/64x64/apps
    INSTALLS += icon
}

contains(MEEGO_EDITION,harmattan) {
    desktopfile.files = client_harmattan.desktop
    desktopfile.path = /usr/share/applications
    INSTALLS += desktopfile

    icon.files = client80.png
    icon.path = /usr/share/icons/hicolor/80x80/apps
    INSTALLS += icon
}
