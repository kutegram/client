<p align="center">
  <a href="https://github.com/kutegram/client">
    <img src="/rc/icons/hicolor/80x80/apps/kutegram.png" width="80" height="80">
  </a>
  <h1>Kutegram</h1>
</p>

## Supported platforms
* Windows XP and higher (maybe even lower?)
* Linux
* Symbian 9.2 and higher
* Maemo 5 Fremantle
* MeeGo Harmattan

## Current features
- [x] MTProto 2.0 support
- [x] Code authorization
- [ ] 2FA authorization
- [ ] QR-code login flow
- [x] Dialogs list
- [x] Basic messages history
- [x] Basic messages sending
- [ ] Messages entities formatting
- [ ] Media uploading/downloading

## How to build
### Symbian 9.2 and higher
*Windows only*
* Clone this repository with submodules.
```
git clone --recursive https://github.com/kutegram/client.git
cd client
```
* Go to `library` folder, rename `apivalues.default.h` file to `apivalues.h`, and fill needed fields.
* Run build script.
```
buildSymbian.bat
```
