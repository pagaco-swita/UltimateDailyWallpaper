#---------------------------------------------------
# "UltimateDailyWallpaper" Copyright (C) 2023 Patrice Coni
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#  
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#---------------------------------------------------

VERSION   = 1.0.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

QT       += core gui
QT       += network
QT       += xml
QT       += concurrent
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET          = UltimateDailyWallpaper
TEMPLATE        = app
CONFIG          += warn_on
DESTDIR         = ../bin
MOC_DIR         = ../build/moc
RCC_DIR         = ../build/rcc
UI_DIR          = ../build/ui
unix:OBJECTS_DIR = ../build/o/unix

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    about.cpp \
    addrecord.cpp \
    main.cpp \
    mainwindow.cpp \
    photobrowser.cpp \
    settingswindow.cpp \
    setwallpaper.cpp

HEADERS += \
    about.h \
    addrecord.h \
    connection.h \
    interfaces.h \
    itemdelegate.h \
    mainwindow.h \
    photobrowser.h \
    settingswindow.h \
    setwallpaper.h

FORMS += \
    about.ui \
    photobrowser.ui \
    settingswindow.ui

# Default rules for deployment.
# qnx: target.path = /tmp/$${TARGET}/bin
# else: unix:!android: target.path = /opt/$${TARGET}/bin
# !isEmpty(target.path): INSTALLS += target

RESOURCES += \
    udw_ressource.qrc
