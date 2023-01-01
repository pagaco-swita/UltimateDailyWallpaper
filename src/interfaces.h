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

#ifndef INTERFACES_H
#define INTERFACES_H

#include <QObject>
#include <QString>
#include <QStringList>

//! [0]
class BasicInterface
{
public:
    virtual ~BasicInterface()=default;
    virtual void get_picture(bool downloadthumb,
                             QString _picturedir,
                             QString lang,
                             int year,
                             int month,
                             int day)=0;
    virtual QString provider()=0;
    virtual QString pluginname()=0;
    virtual QString plugininfo()=0;

    virtual bool ExtendedFunctionInterface()=0;
    virtual bool AdditionalFunctionInterface()=0;
    virtual bool MenuInterface()=0;
    virtual bool SubMenuInterface()=0;

signals:
    virtual void download_successful(QString _description,
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
                                     QString potd_date)=0;
};

class ExtendedFunctionInterface
{
public:
    virtual void ExtendedFunction1()=0;
    virtual void ExtendedFunction2()=0;
};

class AdditionalFunctionInterface
{
public:
    virtual void AdditionalFunction1()=0;
    virtual void AdditionalFunction2()=0;
    virtual void AdditionalFunction3()=0;
    virtual void AdditionalFunction4()=0;
    virtual void AdditionalFunction5()=0;
};

class MenuInterface
{
public:
    virtual QStringList MenuTriggers()=0;
};

class SubMenuInterface
{
public:
    virtual QString SubMenuTitle()=0;
    virtual QStringList SubMenuTriggers()=0;
    virtual QStringList SubMenuEmitStrings()=0;

public slots:
    virtual void SubMenuFunction(QString anything);
};

QT_BEGIN_NAMESPACE

#define BasicInterface_iid "UltimateDailyWallpaper.BasicInterface"
Q_DECLARE_INTERFACE(BasicInterface, BasicInterface_iid)

#define ExtendedFunctionInterface_iid "UltimateDailyWallpaper.ExtendedFunctionInterface"
Q_DECLARE_INTERFACE(ExtendedFunctionInterface, ExtendedFunctionInterface_iid)

#define AdditionalFunctionInterface_iid "UltimateDailyWallpaper.AdditionalFunctionInterface"
Q_DECLARE_INTERFACE(AdditionalFunctionInterface, AdditionalFunctionInterface_iid)

#define MenuInterface_iid "UltimateDailyWallpaper.MenuInterface"
Q_DECLARE_INTERFACE(MenuInterface, MenuInterface_iid)

#define SubMenuInterface_iid "UltimateDailyWallpaper.SubMenuInterface"
Q_DECLARE_INTERFACE(SubMenuInterface, SubMenuInterface_iid)

QT_END_NAMESPACE

//! [0]

#endif // INTERFACES_H
