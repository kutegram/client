TARGET = Kutegram
DESTDIR =

TARGET.UID3 = 0xE0713D51
DEFINES += SYMBIAN_UID=$$TARGET.UID3
DEFINES += BUILD_PLATFORM=\"\\\"Symbian\\\"\"

TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
TARGET.EPOCSTACKSIZE = 0x08000
#TARGET.EPOCALLOWDLLDATA = 1

contains(SYMBIAN_VERSION, Symbian3) {
    DEFINES += SYMBIAN3_READY
}

#QMAKE_CXXFLAGS.CW += -O2
#QMAKE_LFLAGS.ARMCC += --rw-base 0xC00000
#QMAKE_LFLAGS.GCCE += -Tdata 0xC00000

vendorinfo = \
    "%{\"curoviyxru\"}" \
    ":\"curoviyxru\""

platform_product_id = S60ProductID

supported_platforms = \
    "[0x1028315F],0,0,0,{\"S60ProductID\"}" \ # Symbian^1
    "[0x20022E6D],0,0,0,{\"S60ProductID\"}" \ # Symbian^3
    "[0x102032BE],0,0,0,{\"S60ProductID\"}" \ # Symbian 9.2
    "[0x102752AE],0,0,0,{\"S60ProductID\"}" \ # Symbian 9.3
    "[0x2003A678],0,0,0,{\"S60ProductID\"}"   # Symbian Belle

default_deployment.pkg_prerules -= pkg_platform_dependencies
myDeployment.pkg_prerules = vendorinfo supported_platforms
DEPLOYMENT += myDeployment

TARGET.CAPABILITY = ReadUserData WriteUserData UserEnvironment NetworkServices LocalServices
ICON = rc/icons/hicolor/scalable/apps/kutegram.svg

LIBS += -lavkon \
#    -laknnotify \
#    -lhwrmlightclient \
    -lapgrfx \
    -lcone \
#    -lws32 \
#    -lbitgdi \
#    -lfbscli \
#    -laknskins \
#    -laknskinsrv \
    -leikcore \
    -lapmime \
#    -lefsrv \
#    -leuser \
#    -lcommondialogs \
#    -lesock \
#    -lmediaclientaudio \
#    -lprofileengine \
#    -lcntmodel \
#    -lbafl \
#    -lmgfetch
