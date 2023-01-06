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

#ifndef WIKIMEDIA_COMMONS_POTD_H
#define WIKIMEDIA_COMMONS_POTD_H

#include <QObject>
#include <QtPlugin>
#include <QStringList>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "interfaces.h"

class Wikimedia_Commons_potd : public QObject,
                               public BasicInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "UltimateDailyWallpaper.BasicInterface" FILE "wikimedia_commons_potd.json")
    Q_INTERFACES(BasicInterface)

public:
    Wikimedia_Commons_potd();
    ~Wikimedia_Commons_potd();

    bool SetExtendedFunctionInterface() override;
    bool SetAdditionalFunctionInterface() override;
    bool SetMenuInterface() override;
    bool SetSubMenuInterface() override;

    void get_picture(bool downloadthumb,
                     QString _picturedir,
                     QString lang,
                     int year,
                     int month,
                     int day) override;

    QString provider() override;
    QString pluginname() override;
    QString plugininfo() override;

    void get_wikimedia_commons_potd(bool downloadthumb,
                                    QString _picturedir,
                                    QString lang,
                                    int year,
                                    int month,
                                    int day);

    int existing_pageid;

signals:
    void download_successful(QString _description,
                             QString _copyright,
                             QString _provider,
                             QString _title_headline,
                             QString _filename,
                            QString _browser_url,
                             QString _thumb_filename,
                             int _size_height,
                             int _size_width,
                             int pageid,
                             bool tempdatabase,
                             QString potd_date) override;

private:
    QNetworkAccessManager dl_manager;
    QNetworkReply *reply;

    QString _reply;
    QString picture_filename;
    QString thumbfilename;
    QString _picturedlpath;
    QString _description_url;
    QString _thumburl;
    QString _databaseFilePath;
    QString picture_url;
    QString picture_download_url;
    QString _wpc_potd_description_orig;
    QString _wpc_potd_description;
    QString _description_URL;
    QString _imageinfo_url;
    QString _author;

    int _picture_width;
    int _picture_height;
    int _pageid;

    int check_exists();
    int create_thumbfile(QString picturedir, QString filename);
    int download_picture(QString _dlurl, QString dlpath, QString filename);
    int doProcess(QString command, QStringList arguments);
    void get_picture_url(QString base_URL);
    void get_picture_base_url();
    void get_description(QString _lang);
    void get_values();
    bool request_download_xml(QString _targeturl);
};

#endif // WIKIMEDIA_COMMONS_POTD_H
