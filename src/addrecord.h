/**
 * "UltimateDailyWallpaper" Copyright (C) 2022 Patrice Coni
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


#ifndef ADDRECORD_H
#define ADDRECORD_H

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QObject>

class addrecord : public QObject
{
    Q_OBJECT

public:
    addrecord();

public slots:
    void _add_record(QString _description, QString _copyright, QString _provider,
                     QString _title_headline, QString _filename,
                     QString _browser_url, QString _thumb_filename,
                     int _size_height, int _size_width, int pageid,
                     bool tempdatabase, QString potd_date);

private:
    QString _databaseFilePath;
    QString _tempdatabaseFilePath;

    bool _connected;
};

#endif // ADDRECORD_H
