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

VERSION     = 0.0.1
DEFINES     += PLUGIN_VERSION=\\\"$$VERSION\\\"
DESTDIR     = ../../../../plugins
QT          += widgets sql network xml concurrent
CONFIG      += c++17 plugin
TARGET      = wikimedia_commons_potd_$$VERSION
TEMPLATE    = lib
DEFINES     += WIKIMEDIA_COMMONS_POTD_LIBRARY

INCLUDEPATH += ../../../../src

FILES = wikimedia_commons_potd.json

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    wikimedia_commons_potd.cpp

HEADERS += \
    wikimedia_commons_potd.h
