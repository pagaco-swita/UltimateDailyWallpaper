/**
 * "UltimateDailyWallpaper" Copyright (C) 2023 Patrice Coni
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SETWALLPAPER_H
#define SETWALLPAPER_H

#include <QObject>

class setWallpaper: public QObject
{
    Q_OBJECT

public:
    setWallpaper();

signals:
    void wallpaper_changed(QString _scriptfile);

public slots:
    void _set_wallpaper(QString _wallpaperfile, int _Parameter);

private:
    QString out;
    QStringList _detected_monitors;

    int doProcess(QString command);
    int _xfce4_detect_monitors();
    void _set_wallpaper_script(QString _wallpaperfile, QString _scriptfile);
};

#endif // SETWALLPAPER_H
