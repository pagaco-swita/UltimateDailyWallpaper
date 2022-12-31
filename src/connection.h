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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDir>

//! [0]
static bool createConnection()
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

    udw_db.setDatabaseName(QDir::homePath()+"/.UltimateDailyWallpaper/udw_database.sqlite");
    if (!udw_db.open())
    {
        return false;
    }

    QSqlQuery udw_query(udw_db);
    if(!udw_db.tables().contains(QLatin1String("udw_history")))
    {
        udw_query.exec("create table udw_history (id int primary key, "
                   "date varchar(30), description varchar(900), copyright varchar(500),"
                   "title varchar(500), provider varchar(100),"
                   "filename varchar(150), browser_url varchar(500), size_width int, size_height int,"
                   "thumb_filename varchar(150), pageid int)");

        udw_query.finish();
        udw_query.clear();
        udw_db.close();
    }

    return true;
}
//! [0]

#endif // CONNECTION_H
