![Icon](https://github.com/pagaco-swita/ultimatedailywallpaper/blob/main/src/ultimatedesktopwallpaper_icon.png)

# UltimateDailyWallpaper (for linux based operating systems)
UltimateDailyWallpaper is a utility to set the Wikimedia Commons daily picture as wallpaper. It saves images in a high quality. Further information about each picture can be obtained from the internet with a single click. It integrates with most X11 desktop environments to provide automatically changing wallpaper.
<br>
#### Features of the application: <br />
 * Allows to download the daily picture of Wikimedia Commons (POTD)
 * Allows to download "pictures of the day" from past days
 * Supports the most of X11 desktop environments to provide automatically changing wallpaper

#### Requierment: <br />
* Qt 5.14 (or higher) - Download Qt here: https://www.qt.io/download
* imgp - https://github.com/jarun/imgp

#### Get the latest build:<br />
<a href="hhttps://github.com/pagaco-swita/ultimatedailywallpaper/releases">https://github.com/pagaco-swita/ultimatedailywallpaper/releases</a>

#### How to build the Application:

```
$ sudo apt-get install libkf5filemetadata-dev locales qt5-qmake-bin libkf5coreaddons-dev libkf5config-dev qtbase5-dev tzdata qtchooser qt5-qmake libqt5opengl5-dev baloo-kf5-dev qtbase5-dev-tools sensible-utils wget imgp libqt5gui5 libqt5widgets5 libqt5core5a

$ cd ./src
$ qmake (or qmake -qt5)
$ make -f Makefile
``` 

#### Screenshot: <br />
![Screenshot](https://github.com/pagaco-swita/ultimatedailywallpaper/blob/main/screenshots/example_desktop.png)

![Screenshot](https://github.com/pagaco-swita/ultimatedailywallpaper/blob/main/screenshots/download_picture.png)

![Screenshot](https://github.com/pagaco-swita/ultimatedailywallpaper/blob/main/screenshots/main_menu.png)
