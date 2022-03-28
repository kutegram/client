TRANSLATIONS += rc/translations/kutegram_en.ts \
    rc/translations/kutegram_es.ts \
    rc/translations/kutegram_ru.ts \
    rc/translations/kutegram_uk.ts \
    rc/translations/kutegram_pt.ts

isEmpty(QMAKE_LUPDATE) {
	win32:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]\lupdate.exe
	else:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate
        !exists($$QMAKE_LUPDATE) { QMAKE_LUPDATE = lupdate }
}
isEmpty(QMAKE_LRELEASE) {
	win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
        else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
        !exists($$QMAKE_LRELEASE) { QMAKE_LRELEASE = lrelease }
}

system($$QMAKE_LUPDATE ..\client.pro)
system($$QMAKE_LRELEASE ..\client.pro)

#updatets.commands = $$QMAKE_LUPDATE -locations none -noobsolete client.pro
#QMAKE_EXTRA_TARGETS += updatets

#updateqm.depends = updatets
#updateqm.input = TRANSLATIONS
#updateqm.output = rc/translations/${QMAKE_FILE_BASE}.qm
#updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN}
#updateqm.CONFIG += no_link
#QMAKE_EXTRA_COMPILERS += updateqm
