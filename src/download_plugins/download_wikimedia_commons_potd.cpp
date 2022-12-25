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

#include "download_wikimedia_commons_potd.h"

#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QtXml/QDomElement>
#include <QtXml/QDomDocument>
#include <QXmlStreamReader>
#include <QPixmap>
#include <QUrl>
#include <QProcess>

download_wikimedia_commons_potd::download_wikimedia_commons_potd()
{
    QObject::connect(this, &download_wikimedia_commons_potd::download_successful, [=](QString _description, QString _copyright, QString _provider,
                     QString _title_headline, QString _filename,
                     QString _browser_url, QString _thumb_filename,
                     int _size_height, int _size_width, int pageid,
                     bool tempdatabase, QString potd_date){
            add_record._add_record(_description, _copyright, _provider,
                                   _title_headline, _filename,
                                   _browser_url, _thumb_filename,
                                   _size_height, _size_width, pageid,
                                   tempdatabase, potd_date);
        });
}

download_wikimedia_commons_potd::~download_wikimedia_commons_potd()
{
    if (reply != 0)
    {
        delete reply;
    }
}

void download_wikimedia_commons_potd::get_wikimedia_commons_potd(bool downloadthumb, QString _picturedir, QString lang, int year, int month, int day)
{
    QString _baseurl;

    QString _year = QString::number(year);
    QString _month = QString("%1").arg(month, 2, 10, QChar('0'));
    QString _day = QString("%1").arg(day, 2, 10, QChar('0'));

    existing_pageid=0;

    if(!(year==0) && !(month==0) && !(day==0))
    {
        _baseurl="https://commons.wikimedia.org/w/api.php?action=expandtemplates&text={{Potd/"+_year+"-"+_month+"-"+_day+"}}&format=xml";
        _description_URL="https://commons.wikimedia.org/w/api.php?action=expandtemplates&text={{Potd/"+_year+"-"+_month+"-"+_day+" ("+lang+")}}&format=xml";
    } else
    {
        _baseurl="https://commons.wikimedia.org/w/api.php?action=expandtemplates&text={{Potd/{{CURRENTYEAR}}-{{CURRENTMONTH}}-{{CURRENTDAY2}}}}&format=xml";
        _description_URL="https://commons.wikimedia.org/w/api.php?action=expandtemplates&text={{Potd/{{CURRENTYEAR}}-{{CURRENTMONTH}}-{{CURRENTDAY2}} ("+lang+")}}&format=xml";
    }

    QDomDocument xmlfile;
    QDomElement element;

    if(request_download_xml(_baseurl)==true)
    {
        xmlfile.setContent(_reply);
        element = xmlfile.documentElement().firstChild().toElement();
        while(!element.isNull()) {
            if(element.tagName()=="expandtemplates")
            {
                picture_filename=element.firstChild().toText().data();
            }
            element = element.nextSibling().toElement();
        }
        picture_url="https://commons.wikimedia.org/w/api.php?titles=Image:"+picture_filename+"&action=query&prop=imageinfo&iiprop=url&iiurlwidth=200&format=xml";
        _imageinfo_url="https://commons.wikimedia.org/w/api.php?titles=Image:"+picture_filename+"&action=query&prop=imageinfo&iiprop=url&iiurlwidth=200&prop=imageinfo&iiprop=extmetadata&iimetadataversion=latest&format=xml";
        _reply.clear();
        element.clear();
        xmlfile.clear();
    }  

    if(downloadthumb==true)
    {
        _databaseFilePath=QDir::homePath()+"/.UltimateDailyWallpaper/temp/temp_udw_database.sqlite";

        get_values();

        _wpc_potd_description=_wpc_potd_description.trimmed();

        if(!(download_picture(_thumburl, _picturedir, picture_filename)==255))
        {
            emit download_successful(_wpc_potd_description, "", "1", "Wikimedia Commons: Picture of the day",
                                     picture_filename, _description_url,
                                     picture_filename, _picture_height, _picture_width, _pageid,
                                     true, _year+_month+_day);            
        }
    }
    else
    {
        if(!picture_filename.isEmpty())
        {            
            if(check_exists()==1)
            {
                _databaseFilePath=QDir::homePath()+"/.UltimateDailyWallpaper/udw_database.sqlite";

                get_values();

                _wpc_potd_description=_wpc_potd_description.trimmed();

                if(!(download_picture(picture_download_url, _picturedir, picture_filename)==255) && !(create_thumbfile(_picturedir, picture_filename)==255))
                {
                    if(thumbfilename.contains("\x5C"))
                    {
                        thumbfilename.remove("\x5C");
                    }

                    if(thumbfilename.contains("\'\\'\'"))
                    {
                        thumbfilename.replace("\'\\'\'", "\'");
                    }

                    emit download_successful(_wpc_potd_description, "", "1", "Wikimedia Commons: Picture of the day",
                                             picture_filename, _description_url,
                                             thumbfilename, _picture_height, _picture_width, _pageid,
                                             false, _year+_month+_day);
                }
            }
            else
            {
                qDebug() << "The most recent image is already stored.";
            }
        }
    }
}

void download_wikimedia_commons_potd::get_values()
{
    QDomDocument xmlfile;
    QDomElement element;

    if(request_download_xml(picture_url)==true)
    {
        xmlfile.setContent(_reply);
        if (!(_reply.isEmpty())) {
            QXmlStreamReader xmlcontent_reader(_reply);
            while(!xmlcontent_reader.atEnd()) {
                xmlcontent_reader.readNext();
                if (xmlcontent_reader.isStartElement()) {
                    if (xmlcontent_reader.name() == "page")
                    {
                        foreach(const QXmlStreamAttribute &url_attr, xmlcontent_reader.attributes()) {
                            if (url_attr.name().toString() == QLatin1String("pageid")) {
                                _pageid = url_attr.value().toInt();
                            }
                        }
                    }
                    if (xmlcontent_reader.name() == "ii")
                    {
                        foreach(const QXmlStreamAttribute &url_attr, xmlcontent_reader.attributes()) {
                            if (url_attr.name().toString() == QLatin1String("url")) {
                                picture_download_url = url_attr.value().toString();
                            }
                            if (url_attr.name().toString() == QLatin1String("descriptionurl")) {
                                _description_url = url_attr.value().toString();
                            }
                            if (url_attr.name().toString() == QLatin1String("thumburl")) {
                                _thumburl = url_attr.value().toString();
                            }
                        }
                    }
                }
            }
        }
        _reply.clear();
        xmlfile.clear();
    }

    if(request_download_xml(_imageinfo_url)==true)
    {
        xmlfile.setContent(_reply);
        if (!(_reply.isEmpty())) {
            QXmlStreamReader xmlcontent_reader(_reply);
            while(!xmlcontent_reader.atEnd()) {
                xmlcontent_reader.readNext();
                if (xmlcontent_reader.isStartElement())
                {
                    if (xmlcontent_reader.name() == "Artist")
                    {
                        foreach(const QXmlStreamAttribute &url_attr, xmlcontent_reader.attributes()) {
                            if (url_attr.name().toString() == QLatin1String("value"))
                            {
                                _author=url_attr.value().toString();
                                _author.remove(QRegExp("<[^>]*>"));
                            }
                        }
                    }
                    if (xmlcontent_reader.name() == "ImageDescription")
                    {
                        foreach(const QXmlStreamAttribute &url_attr, xmlcontent_reader.attributes()) {
                            if (url_attr.name().toString() == QLatin1String("value"))
                            {
                                _wpc_potd_description_orig=url_attr.value().toString();
                                _wpc_potd_description_orig.remove(QRegExp("<[^>]*>"));
                                if(_wpc_potd_description_orig.contains("."))
                                {
                                    QStringList parts=_wpc_potd_description_orig.split(".");
                                    _wpc_potd_description=parts.at(0);

                                }
                                else
                                {
                                    _wpc_potd_description=_wpc_potd_description_orig;
                                }
                                _wpc_potd_description=_wpc_potd_description.simplified();
                            }
                        }
                    }
                }
            }
        }
        _reply.clear();
        xmlfile.clear();
    }

    if(!(_author.isEmpty()) && !(_author=="1"))
    {
        _wpc_potd_description.append(" - Picture by "+_author);
    }

}

int download_wikimedia_commons_potd::check_exists()
{
    QString content;
    QString _databaseFilePath=QDir::homePath()+"/.UltimateDailyWallpaper/udw_database.sqlite";

    QSqlDatabase udw_db;
    if(QSqlDatabase::contains("qt_sql_default_connection"))
    {
        udw_db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        udw_db = QSqlDatabase::addDatabase("QSQLITE");
    }

    udw_db.setDatabaseName(_databaseFilePath);
    udw_db.open();

    QSqlQuery uwp_query(udw_db);
    uwp_query.prepare("SELECT filename FROM udw_history WHERE filename=\'"+picture_filename+"\'");
    uwp_query.exec();    
    while(uwp_query.next())
    {
        if(uwp_query.last())
        {
            content = uwp_query.value(0).toString();
        }
    }

    if(!content.isEmpty())
    {
        if(picture_filename==content)
        {
            uwp_query.prepare("SELECT pageid FROM udw_history WHERE filename=\'"+picture_filename+"\'");
            uwp_query.exec();
            while(uwp_query.next())
            {
                if(uwp_query.last())
                {
                    existing_pageid = uwp_query.value(0).toInt();
                }
            }

            return 255;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 1;
    }

    return 100;
}

bool download_wikimedia_commons_potd::request_download_xml(QString _targeturl)
{
    QEventLoop loop;
    QObject::connect(&dl_manager,&QNetworkAccessManager::finished,&loop,&QEventLoop::quit);
    QUrl targeturl=_targeturl;
    reply = dl_manager.get(QNetworkRequest(targeturl));
    loop.exec();
    if (reply->bytesAvailable())
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            _reply = reply->readAll();
            delete reply;
            reply=0;
        }
        else
        {
            qDebug() << reply->errorString();
            return false;
        }
    }

    loop.quit();
    loop.exit();

    return true;
}

int download_wikimedia_commons_potd::download_picture(QString _dlurl, QString dlpath, QString filename)
{
    QDir _picturedir(dlpath);
    if(!_picturedir.exists())
    {
        _picturedir.mkpath(dlpath);
    }

    QStringList arguments;
    arguments << "-O" << dlpath+"/"+filename << _dlurl;

    if(!(doProcess("wget", arguments)==255))
    {
        QImage img(dlpath+"/"+filename);

        _picture_height=img.size().height();
        _picture_width=img.size().width();
    }
    else
    {
        _picture_height=0;
        _picture_width=0;

        return 255;
    }

    return 0;
}

int download_wikimedia_commons_potd::create_thumbfile(QString picturedir, QString filename)
{
    QString thumbfiledir=QDir::homePath()+"/.UltimateDailyWallpaper/thumbnails";
    QString scriptfile=QDir::homePath()+"/.UltimateDailyWallpaper/temp_script.sh";

    if(filename.contains("\""))
    {
        filename.replace("\"", "\x5C\x22");
    }

    if(filename.contains("\'"))
    {
        filename.replace("\'", "\'\\'\'");
    }

    thumbfilename="thumb_"+filename;

    QDir _thumbfile_dir(thumbfiledir);
    if(!_thumbfile_dir.exists())
    {
        _thumbfile_dir.mkpath(thumbfiledir);
    }

    QFile ct_script(scriptfile);
    if(!ct_script.exists(scriptfile))
    {
        QString content = "#!/bin/bash\n"
                          "\n"
                          "imgp -x 500x290 \""+picturedir+"/"+filename+"\" \n"
                          "newpicture=$(ls "+picturedir+"/"+"*_IMGP*) \n"
                          "mv \"$newpicture\" "+"\""+thumbfiledir+"/"+thumbfilename+"\"";

        if (ct_script.open(QIODevice::Append))
        {
            QTextStream stream(&ct_script);
            stream <<content<<endl;
        }
    }

    QStringList arguments;

    if(doProcess("/bin/bash "+scriptfile, arguments)==255)
    {
        ct_script.remove();
        return 255;
    } else
    {
        ct_script.remove();
        return 1;
    }
}

int download_wikimedia_commons_potd::doProcess(QString command, QStringList arguments)
{
    QProcess proc;

    if(!arguments.isEmpty())
    {
        proc.start(command, arguments);
    }
    else
    {
        proc.start(command);
    }

    proc.waitForFinished();
    proc.close();

    return proc.exitCode();
}
