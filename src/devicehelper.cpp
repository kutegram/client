#include "devicehelper.h"

#include <QDesktopServices>
#include <QSystemTrayIcon>

#ifdef MOBILITY_READY
#include <QSystemDeviceInfo>

QTM_USE_NAMESPACE
#endif

#if defined(Q_OS_SYMBIAN)
#include <avkon.hrh> // KAknDiscreetPopupDurationLong
#include <eikenv.h> // CEikonEnv
#include <aknnotewrappers.h>
#include <aknglobalnote.h>
#include <AknCommonDialogsDynMem.h>
#include <aknglobalmsgquery.h>
#include <e32svr.h>
#include <eikmenup.h>
#include <coemain.h>
#include <MGFetch.h>
#include <apgcli.h> // RApaLsSession
#include <apgtask.h> // TApaTaskList, TApaTask
#include <e32property.h> //http://katastrophos.net/symbian-dev/GUID-C6E5F800-0637-419E-8FE5-1EBB40E725AA/GUID-C4776034-D190-3FC4-AF45-C7F195093AC3.html
#include <aknglobalnote.h>
#include <aknappui.h>
#include <aknkeylock.h>
#endif

#ifdef SYMBIAN3_READY
#include <coreapplicationuisdomainpskeys.h> //keys for RProperty
#include <akndiscreetpopup.h>
#include <hwrmlight.h>
#endif

#if defined(Q_OS_UNIX) && !defined(Q_OS_SYMBIAN)
#include <QX11Info>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#endif

bool isInSilentMode()
{
#ifdef MOBILITY_READY
    return (QSystemDeviceInfo::Profile) QSystemDeviceInfo().currentProfile() == QSystemDeviceInfo::SilentProfile;
#else
    return false;
#endif
}

void showChatIcon()
{
#if defined(Q_OS_SYMBIAN) && defined(SYMBIAN3_READY)
    static bool chatIconStatus = false;
    if (!chatIconStatus) {
        RProperty iProperty;
        iProperty.Set(KPSUidCoreApplicationUIs, KCoreAppUIsUipInd, ECoreAppUIsShow);
        chatIconStatus = true;
    }
#endif
}

void hideChatIcon()
{
#if defined(Q_OS_SYMBIAN) && defined(SYMBIAN3_READY)
    RProperty iProperty;
    iProperty.Set(KPSUidCoreApplicationUIs, KCoreAppUIsUipInd, ECoreAppUIsDoNotShow);
#endif
}

void playNotification(QString path)
{
    //TODO
}

void setAppHiddenState(bool state)
{
#if defined(Q_OS_SYMBIAN)
    static CAknAppUi* appUi = dynamic_cast<CAknAppUi*>(CEikonEnv::Static()->AppUi());
    appUi->HideApplicationFromFSW(state);
#endif
}

void notificationBlink(int device) {
#if defined(Q_OS_SYMBIAN) && defined(SYMBIAN3_READY) //TODO: why only S^3? it should works on S^1, but it can't find a binary
    if (isInSilentMode()) return;
    static CHWRMLight* notifyLight = CHWRMLight::NewL();
    switch (device) {
    case 1: TRAP_IGNORE(notifyLight->LightBlinkL(CHWRMLight::ECustomTarget1, 30, 1, 1, KHWRMDefaultIntensity)); break;
    case 2: TRAP_IGNORE(notifyLight->LightBlinkL(CHWRMLight::ECustomTarget2, 30, 1, 1, KHWRMDefaultIntensity)); break;
    default: TRAP_IGNORE(notifyLight->LightBlinkL(CHWRMLight::ECustomTarget2, 30, 1, 1, KHWRMDefaultIntensity)); break;
    }
#endif
}

void showSystemNotificationPopup(QString title, QString message)
{
#if defined(Q_OS_SYMBIAN) && defined(SYMBIAN3_READY)
    static TUid symbianUid = {SYMBIAN_UID};
    TPtrC16 sTitle(reinterpret_cast<const TUint16*>(title.utf16()));
    TPtrC16 sMessage(reinterpret_cast<const TUint16*>(message.utf16()));
    //TODO: icon?
    TRAP_IGNORE(CAknDiscreetPopup::ShowGlobalPopupL(sTitle, sMessage, KAknsIIDNone, KNullDesC, 0, 0, KAknDiscreetPopupDurationLong, 0, NULL, symbianUid));
#elif !defined(Q_OS_SYMBIAN)
    QSystemTrayIcon icon;
    icon.show();
    icon.setToolTip("Kutegram");
    icon.showMessage(title, message);
    icon.hide();
#endif
}

void showNotification(QString title, QString message)
{
    showChatIcon();
    showSystemNotificationPopup(title, message);
    //TODO: vibrate
    //TODO: sound
    //TODO: blink
}

void openUrl(const QUrl &url)
{
#ifdef Q_OS_SYMBIAN
    static TUid KUidBrowser = {0x10008D39};
    _LIT(KBrowserPrefix, "4 " );

    // convert url to encoded version of QString
    QString encUrl(QString::fromUtf8(url.toEncoded()));
    // using qt_QString2TPtrC() based on
    // <http://qt.gitorious.org/qt/qt/blobs/4.7/src/corelib/kernel/qcore_symbian_p.h#line102>
    TPtrC tUrl(TPtrC16(static_cast<const TUint16*>(encUrl.utf16()), encUrl.length()));

    // Following code based on
    // <http://www.developer.nokia.com/Community/Wiki/Launch_default_web_browser_using_Symbian_C%2B%2B>

    // create a session with apparc server
    RApaLsSession appArcSession;
    User::LeaveIfError(appArcSession.Connect());
    CleanupClosePushL<RApaLsSession>(appArcSession);

    // get the default application uid for application/x-web-browse
    TDataType mimeDatatype(_L8("application/x-web-browse"));
    TUid handlerUID;
    appArcSession.AppForDataType(mimeDatatype, handlerUID);

    // if UiD not found, use the native browser
    if (handlerUID.iUid == 0 || handlerUID.iUid == -1)
        handlerUID = KUidBrowser;

    // Following code based on
    // <http://qt.gitorious.org/qt/qt/blobs/4.7/src/gui/util/qdesktopservices_s60.cpp#line213>

    HBufC* buf16 = HBufC::NewLC(tUrl.Length() + KBrowserPrefix.iTypeLength);
    buf16->Des().Copy(KBrowserPrefix); // Prefix used to launch correct browser view
    buf16->Des().Append(tUrl);

    TApaTaskList taskList(CCoeEnv::Static()->WsSession());
    TApaTask task = taskList.FindApp(handlerUID);
    if (task.Exists()) {
        // Switch to existing browser instance
        task.BringToForeground();
        HBufC8* param8 = HBufC8::NewLC(buf16->Length());
        param8->Des().Append(buf16->Des());
        task.SendMessage(TUid::Uid( 0 ), *param8); // Uid is not used
        CleanupStack::PopAndDestroy(param8);
    } else {
        // Start a new browser instance
        TThreadId id;
        appArcSession.StartDocument(*buf16, handlerUID, id);
    }

    CleanupStack::PopAndDestroy(buf16);
    CleanupStack::PopAndDestroy(&appArcSession);
#else
    QDesktopServices::openUrl(url);
#endif
}

void writeX11OrientationAngleProperty(QWidget* widget, ScreenOrientationX11 orientation)
{
#ifdef Q_WS_X11
    if (widget) {
        WId id = widget->winId();
        Display *display = QX11Info::display();
        if (!display) return;
        Atom orientationAngleAtom = XInternAtom(display, "_MEEGOTOUCH_ORIENTATION_ANGLE", False);
        XChangeProperty(display, id, orientationAngleAtom, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&orientation, 1);
    }
#endif
}
