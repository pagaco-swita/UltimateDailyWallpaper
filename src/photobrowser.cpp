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

#include "photobrowser.h"
#include "ui_photobrowser.h"
#include "itemdelegate.h"

#include <QSize>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QPixmap>
#include <QImage>
#include <QIcon>
#include <QStandardItem>
#include <QObject>
#include <QtConcurrent/QtConcurrent>
#include <QThreadPool>
#include <QCloseEvent>
#include <QFileInfoList>
#include <QSettings>
#include <QDesktopServices>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDesktopWidget>
#include <QDebug>

PhotoBrowser::PhotoBrowser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PhotoBrowser)
{
    ui->setupUi(this);

    _read_settings();

    _scaled_picture_width = 258;
    _scaled_picture_height = 145;

    QObject::connect(ui->pushButton_4, &QPushButton::clicked, [this]()
    {
        ui->calendarWidget->setSelectedDate(QDate::currentDate());
    });

    QObject::connect(ui->pushButton_3, &QPushButton::clicked, this, &PhotoBrowser::show);
    QObject::connect(ui->pushButton_2, &QPushButton::clicked, this, &PhotoBrowser::download_and_set);

    _tempdirpath=QDir::homePath()+"/.UltimateDailyWallpaper/temp";

    QObject::connect(this, &PhotoBrowser::go_setWallpaper, [=](QString _wallpaperfile){
        _setwall._set_wallpaper(_wallpaperfile, _Parameter);
    });

    ui->widget_2->setVisible(false);
    ui->widget_3->setVisible(false);
    ui->frame_4->setVisible(false);

    _get_specific_values=false;
}

void PhotoBrowser::closeEvent(QCloseEvent * event)
{
    deltempdir();

    event->ignore();
    this->hide();
}

void PhotoBrowser::reject()
{
    this->hide();
}

PhotoBrowser::~PhotoBrowser()
{
    delete ui;
}

void PhotoBrowser::init(int mode)
{
    deltempdir();

    ui->listView->setResizeMode(QListView::Adjust);
    ui->listView->setFlow(QListView::LeftToRight);
    ui->listView->setSpacing(15);
    ui->listView->setWrapping(true);
    ui->listView->setUniformItemSizes(true);
    ui->listView->setIconSize(QSize (_scaled_picture_width, _scaled_picture_height));
    ui->listView->setViewMode(QListView::IconMode);
    ui->listView->setMovement(QListView::Static);
    ui->listView->setItemDelegate(new ItemDelegate(this));


    /* *************************************************
     * mode variants
     * *************************************************
     *
     *  0: select stored picture
     *
     *  1: show Wikimedia Commons picture of the day
     *     of past days within a given time range
     *
     * *************************************************
     */

    _mode=mode;

    switch(mode)
    {
    case 0:
        _thumbfiledir = QDir::homePath()+"/.UltimateDailyWallpaper/thumbnails";
        _databaseFilePath=QDir::homePath()+"/.UltimateDailyWallpaper/udw_database.sqlite";

        ui->widget_2->setVisible(true);

        break;
    case 1:
        _thumbfiledir = QDir::homePath()+"/.UltimateDailyWallpaper/temp/thumbnails";
        _databaseFilePath=QDir::homePath()+"/.UltimateDailyWallpaper/temp/temp_udw_database.sqlite";

        if(!loadPlugin(_pluginsdir+"/"+_selected_plugin))
        {
            QMessageBox::critical(this, "Error", "Error while loading plugin.");
            deltempdir();
            this->hide();
        }

        this->setWindowTitle("Past pictures of the day (Wikimedia Commons)");

        QDir _tempthumbfiledir(_thumbfiledir);
        if(!_tempthumbfiledir.exists())
        {
            _tempthumbfiledir.mkpath(_thumbfiledir);
        }

        ui->frame_4->setVisible(true);
        ui->widget_3->setVisible(true);
        ui->spinBox->setValue(1);

        if(create_temp_database()==true)
        {
            download_thumb(ui->spinBox->value());
        }

        ui->spinBox->setValue(3);

        break;
    }

    _get_values();

    QStandardItemModel *model = new QStandardItemModel(_thumbfilelist.size(), 1, this);

    for(int i=0;i<_thumbfilelist.size(); i++)
    {
        _thumbfilename = _thumbfilelist.at(i).toUtf8();
        QPixmap thumb_orig;
        thumb_orig.load(_thumbfiledir+"/"+_thumbfilename);
        QPixmap thumb=thumb_orig.scaled(QSize(_scaled_picture_width, _scaled_picture_height),  Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QIcon icon = thumb;
        QStandardItem *item = new QStandardItem(icon, _thumbfilelist.at(i).toUtf8());
        model->setItem(i, item);
    }
    ui->listView->setModel(model);

    wallchanged = false;

    _label_Details = new QLabel(this);
    _label_headline = new QLabel(this);
    _label_headline1 = new QLabel(this);
    _label_description_and_copyright1 = new QLabel(this);
    _label_description_and_copyright = new QLabel(this);
    _preview_text = new QLabel(this);
    _previewImageLabel = new QLabel(this);
    _getmoreInformations = new QPushButton(this);
    _getmoreInformations->setText("Get more information");
    _getmoreInformations->setVisible(false);
    connect(_getmoreInformations, &QPushButton::released, this, &PhotoBrowser::_getmoreInformationsButton_clicked);

    _label_image_size1 = new QLabel(this);
    _label_image_size = new QLabel(this);

    _layout_details = new QVBoxLayout();
    ui->frame_2->setLayout(_layout_details);

    _layout_details->addWidget(_label_Details);
    _layout_details->addSpacerItem(new QSpacerItem(0,20, QSizePolicy::Minimum,QSizePolicy::Minimum));

    _layout_details->addWidget(_label_headline);
    _layout_details->addWidget(_label_headline1);
    _layout_details->addSpacerItem(new QSpacerItem(0,20, QSizePolicy::Minimum,QSizePolicy::Minimum));

    _layout_details->addWidget(_label_description_and_copyright1);
    _layout_details->addWidget(_label_description_and_copyright);
    _layout_details->addSpacerItem(new QSpacerItem(0,20, QSizePolicy::Minimum,QSizePolicy::Minimum));

    _layout_details->addWidget(_label_image_size1);
    _layout_details->addWidget(_label_image_size);
    _layout_details->addSpacerItem(new QSpacerItem(0,20, QSizePolicy::Minimum,QSizePolicy::Minimum));

    _layout_details->addWidget(_preview_text);
    _layout_details->addWidget(_previewImageLabel);
    _layout_details->setAlignment(_previewImageLabel, Qt::AlignHCenter);
    _layout_details->addSpacerItem(new QSpacerItem(0,20, QSizePolicy::Minimum,QSizePolicy::Minimum));
    _layout_details->addWidget(_getmoreInformations);
    _layout_details->setAlignment(_getmoreInformations, Qt::AlignHCenter);
    _layout_details->addStretch();

    _first_run=true;
    _display_details();
}

void PhotoBrowser::_read_settings()
{
    QString _confFilePath = QDir::homePath()+"/.UltimateDailyWallpaper/settings.conf";

    QSettings settings(_confFilePath, QSettings::NativeFormat);

    settings.beginGroup("SETWALLPAPER");
    _Parameter = settings.value("used_desktop","").toInt();
    settings.endGroup();

    settings.beginGroup("SETTINGS");
    _pluginsdir = settings.value("pluginsdir","").toString();
    _WallpaperDir = settings.value("picturedir","").toString();
    settings.endGroup();

    settings.beginGroup("PROVIDER_SETTINGS");
    _lang = settings.value("lang","").toString();
    _selected_plugin = settings.value("selected_plugin","").toString();
    settings.endGroup();
}

void PhotoBrowser::on_pushButton_clicked()
{
    _get_specific_values=true;
    wallchanged = true;
    setWallpaperFile();
    _get_values();
    this->hide();
}

void PhotoBrowser::setWallpaperFile()
{
    if(select_single_value("filename", _databaseFilePath)==true)
    {
        QModelIndex index = ui->listView->currentIndex();
        _thumb_filename = index.data(Qt::DisplayRole).toString();
        emit go_setWallpaper(_WallpaperDir+"/"+_output_value);
        _photobrowser_specific_wallpaperfilename = _output_value;
    }
}

void PhotoBrowser::_get_values()
{
    QSqlDatabase udw_db;
    QSqlQuery udw_query(udw_db);

    if(QSqlDatabase::contains("qt_sql_default_connection"))
    {
      udw_db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
      udw_db = QSqlDatabase::addDatabase("QSQLITE");
    }

    udw_db.setDatabaseName(_databaseFilePath);

    if(_get_specific_values==false)
    {
        udw_query.prepare("SELECT thumb_filename FROM udw_history");
        udw_query.exec();
        while(udw_query.next())
        {
            if(!_thumbfilelist.contains(udw_query.value(0).toString().toUtf8()))
            {
                if(!(udw_query.value(0).toString().isEmpty()))
                {
                    _thumbfilelist.append(udw_query.value(0).toString().toUtf8());
                }
            }
        }
    }
    else
    {
        QModelIndex index = ui->listView->currentIndex();

        _thumb_filename = index.data(Qt::DisplayRole).toString();

        if(_thumb_filename.contains("\x27"))
        {
            _thumb_filename.replace("\x27", "\x27\x27");
        }

        udw_query.prepare("SELECT description FROM udw_history WHERE thumb_filename = \x27"+_thumb_filename+"\x27");
        if(udw_query.exec() && udw_query.next())
        {
            _pb_copyright_description_photo = udw_query.value(0).toString();
        }
        udw_query.prepare("SELECT copyright FROM udw_history WHERE thumb_filename = \x27"+_thumb_filename+"\x27");
        if(udw_query.exec() && udw_query.next())
        {
            _pb_copyright = udw_query.value(0).toString();
        }
        udw_query.prepare("SELECT title FROM udw_history WHERE thumb_filename = \x27"+_thumb_filename+"\x27");
        if(udw_query.exec() && udw_query.next())
        {
            _pb_headline = udw_query.value(0).toString();
        }
        udw_query.prepare("SELECT browser_url FROM udw_history WHERE thumb_filename = \x27"+_thumb_filename+"\x27");
        if(udw_query.exec() && udw_query.next())
        {
            _pb_copyright_link = udw_query.value(0).toString();
        }
        udw_query.prepare("SELECT size_width FROM udw_history WHERE thumb_filename = \x27"+_thumb_filename+"\x27");
        if(udw_query.exec() && udw_query.next())
        {
            _size_width = udw_query.value(0).toInt();
        }
        udw_query.prepare("SELECT size_height FROM udw_history WHERE thumb_filename = \x27"+_thumb_filename+"\x27");
        if(udw_query.exec() && udw_query.next())
        {
            _size_height = udw_query.value(0).toInt();
        }
        udw_query.prepare("SELECT filename FROM udw_history WHERE thumb_filename = \x27"+_thumb_filename+"\x27");
        if(udw_query.exec() && udw_query.next())
        {
            _wallpaperfile = udw_query.value(0).toString();
        }
        _get_specific_values=false;

        if(_thumb_filename.contains("\x27\x27"))
        {
            _thumb_filename.replace("\x27\x27", "\x27");
        }
    }
    udw_query.finish();
    udw_query.clear();

    QThread::msleep(100);
    if (_wallpaperfile.isEmpty())
    {
        // Set a content in the qstring to avoid a crash
        _wallpaperfile = "NULL";
    }
}

void PhotoBrowser::on_listView_clicked()
{
    _display_details();
}

void PhotoBrowser::_display_details()
{
    _label_Details->setText("Details");
    _label_Details->setStyleSheet("font: 14pt; font-weight: bold;");

    _label_headline->setStyleSheet("font-style: italic;");
    _label_headline->setText("Please select a picture.");

    if(_first_run==false)
    {
        _get_specific_values=true;
        _get_values();

        _label_headline->setStyleSheet("font-weight: bold;");
        _label_headline->setText("Title: ");

        _label_headline1->setText(_pb_headline);
        _label_headline1->setWordWrap(true);
        _label_headline1->setMaximumWidth(275);

        _label_description_and_copyright1->setStyleSheet("font-weight: bold;");
        _label_description_and_copyright1->setText("Description: ");

        _label_description_and_copyright->setText(_pb_copyright_description_photo);
        _label_description_and_copyright->setWordWrap(true);
        _label_description_and_copyright->setMaximumWidth(275);

        _label_image_size1->setText("Picture size:");
        _label_image_size1->setStyleSheet("font-weight: bold;");
        _label_image_size->setText(QString::number(_size_width)+" x "+QString::number(_size_height));

        if(_mode==1)
        {
            _label_image_size1->setHidden(true);
            _label_image_size->setHidden(true);
        }

        _preview_text->setText("Preview:");
        _preview_text->setStyleSheet("font-weight: bold;");

        QImage _loadPrevImage;
        QImage _displayImage;

        if(_mode==0)
        {            
            _loadPrevImage.load(_WallpaperDir+"/"+_wallpaperfile);
        }
        else
        {
            _loadPrevImage.load(_thumbfiledir+"/"+_wallpaperfile);
        }

        _displayImage = _loadPrevImage.scaled(240,150, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        _previewImageLabel->setPixmap(QPixmap::fromImage(_displayImage));

        QPixmap _gotoBing(":icons/Info.png");
        _getmoreInformations->setIcon(_gotoBing);
        _getmoreInformations->setVisible(true);
        _getmoreInformations->setMinimumWidth(240);
    }
    _first_run=false;
}

void PhotoBrowser::_getmoreInformationsButton_clicked()
{
    openURL();
}

void PhotoBrowser::openURL()
{
    QDesktopServices::openUrl(_pb_copyright_link);
}

void PhotoBrowser::download_thumb(int days)
{
    QProgressDialog progressdialog("Download in progress...", "Cancel", 0, days, this);
    progressdialog.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    progressdialog.setCancelButton(0);
    progressdialog.setWindowModality(Qt::WindowModal);
    progressdialog.adjustSize();
    progressdialog.move(QApplication::desktop()->screen()->rect().center() - progressdialog.rect().center());
    progressdialog.show();

    QDate selected_date=ui->calendarWidget->selectedDate();

    QString year;
    QString month;
    QString day;

    _thumbfilelist.clear();

    for(int i=0; i<days; i++)
    {
        progressdialog.setValue(i);

        year=selected_date.toString("yyyy");
        month=selected_date.toString("MM");
        day=selected_date.toString("dd");

        basicinterface->get_picture(true, _thumbfiledir, _lang, year.toInt(), month.toInt(), day.toInt());

        selected_date=selected_date.addDays(-1);

        refresh();
    }

    refresh();
    progressdialog.setValue(days);
}

void PhotoBrowser::show()
{
    if(QDate::currentDate()<ui->calendarWidget->selectedDate())
    {
        QMessageBox MsgError;
        MsgError.setIcon(QMessageBox::Critical);
        MsgError.setText("The entered date must not be in the future.");
        MsgError.exec();
    }

    if(QDate::currentDate()>=ui->calendarWidget->selectedDate())
    {
        if(QFile::exists(_databaseFilePath))
        {
            QFile::remove(_databaseFilePath);
        }

        QDir thumbfiledir(_thumbfiledir);
        if(thumbfiledir.exists())
        {
            thumbfiledir.removeRecursively();
            thumbfiledir.mkpath(_thumbfiledir);
        }

        if(create_temp_database()==true)
        {
            download_thumb(ui->spinBox->value());
        }
    }
}

bool PhotoBrowser::create_temp_database()
{
    QSqlDatabase udw_temp_db;

    if(QSqlDatabase::contains("qt_sql_default_connection"))
    {
        udw_temp_db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        udw_temp_db = QSqlDatabase::addDatabase("QSQLITE");
    }

    udw_temp_db.setDatabaseName(_databaseFilePath);

    if (!udw_temp_db.open())
    {
        return false;
    }

    QSqlQuery udw_temp_query(udw_temp_db);
    if(!udw_temp_db.tables().contains(QLatin1String("udw_history")))
    {
        udw_temp_query.exec("create table udw_history (id int primary key, "
                   "date varchar(30), description varchar(900), copyright varchar(500),"
                   "title varchar(500), provider varchar(100),"
                   "filename varchar(150), browser_url varchar(500), size_width int, size_height int,"
                   "thumb_filename varchar(150), pageid int, potd_date varchar(500))");

        udw_temp_query.finish();
        udw_temp_query.clear();
    }

    return true;
}

void PhotoBrowser::refresh()
{
    _get_values();

    QStandardItemModel *model = new QStandardItemModel(_thumbfilelist.size(), 1, this);

    for(int i=0;i<_thumbfilelist.size(); i++)
    {
        _thumbfilename = _thumbfilelist.at(i).toUtf8();
        QPixmap thumb_orig;
        thumb_orig.load(_thumbfiledir+"/"+_thumbfilename);
        QPixmap thumb=thumb_orig.scaled(QSize(_scaled_picture_width, _scaled_picture_height),  Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QIcon icon = thumb;
        QStandardItem *item = new QStandardItem(icon, _thumbfilelist.at(i).toUtf8());
        model->setItem(i, item);
    }
    ui->listView->setModel(model);
}

void PhotoBrowser::download_and_set()
{
    QProgressDialog progressdialog("Working in progress...", "Cancel", 0, 100, this);
    progressdialog.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    progressdialog.setCancelButton(0);
    progressdialog.adjustSize();
    progressdialog.move(QApplication::desktop()->screen()->rect().center() - progressdialog.rect().center());
    progressdialog.setWindowModality(Qt::WindowModal);
    progressdialog.show();

    wallchanged = true;

    QString year;
    QString month;
    QString day;

    if(select_single_value("potd_date", _databaseFilePath)==true)
    {
        progressdialog.setValue(50);
        QDate date=QDate::fromString(_output_value, "yyyyMMdd");

        year=date.toString("yyyy");
        month=date.toString("MM");
        day=date.toString("dd");

        basicinterface->get_picture(false, _WallpaperDir, _lang, year.toInt(), month.toInt(), day.toInt());

        // (temp)thumb_filename is the real filename.
        QModelIndex index = ui->listView->currentIndex();
        _thumb_filename = index.data(Qt::DisplayRole).toString();
        emit go_setWallpaper(_WallpaperDir+"/"+_thumb_filename);

        // Provide QString for menu head
        _thumb_filename = "thumb_"+index.data(Qt::DisplayRole).toString();

        if (progressdialog.wasCanceled())
        {
            progressdialog.cancel();
        }
        progressdialog.setValue(100);
    }

    deltempdir();

    this->hide();
}

bool PhotoBrowser::select_single_value(QString desired_column, QString db_filepath)
{
    QSqlDatabase udw_temp_db;
    QSqlQuery udw_query(udw_temp_db);

    if(QSqlDatabase::contains("qt_sql_default_connection"))
    {
        udw_temp_db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        udw_temp_db = QSqlDatabase::addDatabase("QSQLITE");
    }

    udw_temp_db.setDatabaseName(db_filepath);

    if (!udw_temp_db.open())
    {
        return false;
    }

    if(_thumb_filename.contains("\x27"))
    {
        _thumb_filename.replace("\x27", "\x27\x27");
    }

    udw_query.prepare("SELECT "+desired_column+" FROM udw_history WHERE thumb_filename = \x27"+_thumb_filename+"\x27");
    if(udw_query.exec() && udw_query.next())
    {
        _output_value = udw_query.value(0).toString();
    }
    udw_query.finish();
    udw_query.clear();

    return true;
}

void PhotoBrowser::deltempdir()
{
    QDir tempdir(_tempdirpath);
    if(tempdir.exists())
    {
        tempdir.removeRecursively();
    }
}

bool PhotoBrowser::loadPlugin(QString _pluginfile)
{
    QPluginLoader pluginLoader;

    pluginLoader.setFileName(_pluginfile);

    QObject *plugin = pluginLoader.instance();
    if (plugin)
    {
        basicinterface = qobject_cast<BasicInterface *>(plugin);

        if (basicinterface)
        {
            connect(plugin, SIGNAL(download_successful(QString, QString, QString,
                                                      QString, QString,
                                                      QString, QString,
                                                      int, int, int,
                                                      bool, QString)),
                    this, SLOT(_add_record(QString, QString, QString,
                                     QString, QString,
                                     QString, QString,
                                     int, int, int,
                                     bool, QString)));

            return true;
        }
        pluginLoader.unload();
    }
    return false;
}

void PhotoBrowser::_add_record(QString _description,
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
                             QString potd_date)
{
    QSqlDatabase udw_db;

    QString _tempdatabaseFilePath=QDir::homePath()+"/.UltimateDailyWallpaper/temp/temp_udw_database.sqlite";

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
