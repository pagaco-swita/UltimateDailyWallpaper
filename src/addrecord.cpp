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

#include "addrecord.h"

#include <QDir>
#include <QDate>
#include <QDateTime>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

addrecord::addrecord()
{
    _databaseFilePath=QDir::homePath()+"/.UltimateDailyWallpaper/udw_database.sqlite";
    _tempdatabaseFilePath=QDir::homePath()+"/.UltimateDailyWallpaper/temp/temp_udw_database.sqlite";
}

void addrecord::_add_record(QString _description, QString _copyright, QString _provider,
                                 QString _title_headline, QString _filename,
                                 QString _browser_url, QString _thumb_filename,
                                 int _size_height, int _size_width, int pageid, bool tempdatabase,
                                 QString potd_date)
{
    QSqlDatabase udw_db;

    if(QSqlDatabase::contains("qt_sql_default_connection"))
    {
        udw_db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        udw_db = QSqlDatabase::addDatabase("QSQLITE");
    }

    if(tempdatabase==true)
    {
        udw_db.setDatabaseName(_tempdatabaseFilePath);
    }
    else
    {
        udw_db.setDatabaseName(_databaseFilePath);
    }

    udw_db.open();

    QSqlQuery udw_query(udw_db);

    if(tempdatabase==true)
    {
        udw_query.prepare("INSERT INTO udw_history (id, date, description, copyright, title, provider, filename, browser_url, size_width, size_height, thumb_filename, pageid, potd_date)"
                                            "VALUES (:id, :date, :description, :copyright, :title, :provider, :filename, :browser_url, :size_width, :size_height, :thumb_filename, :pageid, :potd_date)");
    } else
    {
        udw_query.prepare("INSERT INTO udw_history (id, date, description, copyright, title, provider, filename, browser_url, size_width, size_height, thumb_filename, pageid)"
                                            "VALUES (:id, :date, :description, :copyright, :title, :provider, :filename, :browser_url, :size_width, :size_height, :thumb_filename, :pageid)");
    }
    udw_query.bindValue(":id", QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz"));
    udw_query.bindValue(":date", QDate::currentDate().toString("yyyyMMdd"));
    udw_query.bindValue(":description", _description);
    udw_query.bindValue(":copyright", _copyright);
    udw_query.bindValue(":title", _title_headline);
    udw_query.bindValue(":provider", _provider);
    udw_query.bindValue(":filename", _filename);
    udw_query.bindValue(":browser_url", _browser_url);
    udw_query.bindValue(":size_width", _size_width);
    udw_query.bindValue(":size_height", _size_height);
    udw_query.bindValue(":thumb_filename", _thumb_filename);
    udw_query.bindValue(":pageid", pageid);

    if(tempdatabase==true)
    {
        udw_query.bindValue(":potd_date", potd_date);
    }

    if(!udw_query.exec())
    {
        qDebug() << udw_query.lastError();
    }

    udw_query.finish();
    udw_query.clear();
    udw_db.close();
}
