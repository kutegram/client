git clone --branch Symbian1Qt473 https://github.com/kutegram/compilers.git Symbian1Qt473
call Symbian1Qt473\patch.qmake.paths.bat
cd ..\..\..
cd Symbian1Qt473
call bin\qtenvS1.bat
cd ..
lrelease client.pro
qmake.exe client.pro -r -spec symbian-abld "CONFIG+=release" -after "OBJECTS_DIR=obj" "MOC_DIR=moc" "UI_DIR=ui" "RCC_DIR=rcc"
make.exe release-gcce -w in .
make.exe sis .
make.exe clean -w in .
