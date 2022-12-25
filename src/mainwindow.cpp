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

#include "mainwindow.h"

#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QMessageBox>
#include <QDebug>
#include <QDesktopServices>
#include <QProcess>
#include <QThread>
#include <QDate>
#include <QDesktopWidget>
#include <QString>
#include <string>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QObject::connect(this, &MainWindow::_setWallpaper, [=](QString _wallpaperfile, int _set_Parameter)
    {
            setwall._set_wallpaper(_wallpaperfile, _set_Parameter);
    });

    qApp->setAttribute(Qt::AA_DontShowIconsInMenus, false);

    _databaseFilePath = QDir::homePath()+"/.UltimateDailyWallpaper/udw_database.sqlite";
    _AppFileDir = QDir::homePath()+"/.UltimateDailyWallpaper";
    _confFile = _AppFileDir+"/"+"settings.conf";
    _thumbfiledir = QDir::homePath()+"/.UltimateDailyWallpaper/thumbnails";

    QDir _settDir(_AppFileDir);
    if(!_settDir.exists())
    {
        if(check_internet_connection()==true)
        {
            _settDir.mkpath(_AppFileDir);
        }
        else
        {
            QMessageBox MsgError;
            MsgError.setIcon(QMessageBox::Critical);
            MsgError.setText("No connection to the internet.\n"
                             "The application will be closed.");
            MsgError.setStandardButtons(QMessageBox::Ok);

            connect(MsgError.button(QMessageBox::Ok), &QPushButton::clicked, [this]
            {
                exit(0);
            });

            MsgError.exec();
        }
    }

    QFile confFile(_confFile);
    if(!confFile.exists(_confFile))
    {
        QString _defaultcontent =
                "[SETTINGS]\n"
                "picturedir="+_AppFileDir+"/picturefiles\n"
                "autorun=2\n"
                "store_days=17\n"
                "\n"
                "[SETWALLPAPER]\n"
                "autochange=2\n"
                "used_desktop=1\n"
                "auto_timebased_change=1\n"
                "time_hours=18\n"
                "time_minutes=0\n"
                "\n"
                "[PROVIDER_SETTINGS]\n"
                "selected_provider=1\n"
                "lang=en";

        if (confFile.open(QIODevice::Append))
        {
            QTextStream stream(&confFile);
            stream << _defaultcontent << Qt::endl;
        }
    }

    load_settings();

    /* read variable $XDG_CONFIG_HOME.
     * If the variable is empty then set default directory $HOME/.config
     * If the variable contains more than one directory (e.g. PATH1:PATH2...)
     * then split by ":" and select the first directory as autostartDir. */

    QString _confDirVariable = qgetenv("XDG_CONFIG_HOME");
    QString _autorun_location;
    QStringList _detected_autorun_dirs;

    if(_confDirVariable.contains(":")) {
        _detected_autorun_dirs = _confDirVariable.split(":");
        _autorun_location = _detected_autorun_dirs.at(0)+"/autostart";
    } else {
        if(_confDirVariable.contains("/")) {
            _autorun_location = _confDirVariable.toUtf8()+"/autostart";
        } else {
            _autorun_location = QDir::homePath()+"/.config/autostart";
        }
    }

    QFile autorun_file(_autorun_location+"/"+QApplication::applicationName()+".desktop");

    if(_autorun==2)
    {
        if(!autorun_file.exists())
        {
            QString _autorun_file_content =
                "[Desktop Entry]\n"
                "Type=Application\n"
                "Exec=/usr/bin/UltimateDailyWallpaper\n"
                "Hidden=false\n"
                "Name="+QApplication::applicationName()+"\n"
                "Terminal=false\n"
                "Icon=/usr/share/pixmaps/ultimatedesktopwallpaper_icon.png";

            if (autorun_file.open(QIODevice::Append))
            {
                QTextStream stream(&autorun_file);
                stream <<_autorun_file_content<<endl;
            }
        }
    }
    else
    {
        if(autorun_file.exists())
        {
            autorun_file.remove();
        }
    }

    if(createConnection()==false)
    {
        QMessageBox MsgError;
        MsgError.setIcon(QMessageBox::Critical);
        MsgError.setText("An error was detected when starting the application.\n"
                         "The application will be closed.");
        MsgError.setStandardButtons(QMessageBox::Ok);

        connect(MsgError.button(QMessageBox::Ok), &QPushButton::clicked, [this]
        {
            exit(0);
        });

        MsgError.exec();
    }

    mSystemTrayIcon = new QSystemTrayIcon(this);

    request_dl_wallpaper();
    get_last_record(_selected_provider);

    create_MenuHead(_db_rec_description, _db_rec_title, _db_rec_thumb_filename);
    create_Actions();
    create_Menu();

    if(_autochange==2)
    {
        no_autochange();
        set_autochange();
        load_wallpaper();
    }
    else
    {
        no_autochange();
    }

    if(check_internet_connection()==false)
    {
        _display_tooltip("No connection to the internet.", "Please check your internet connection.");
    }
    else
    {
        _display_tooltip(_db_rec_title, _db_rec_description);
    }

    set_SystemTrayIcon();

    connect(mSystemTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(slotActive(QSystemTrayIcon::ActivationReason)));

    check_content();
}

MainWindow::~MainWindow()
{

}

void MainWindow::slotActive(QSystemTrayIcon::ActivationReason r)
{
    if (r == QSystemTrayIcon::Trigger)
    {
        basemenu->exec(QCursor::pos());
    }
}

void MainWindow::set_autochange()
{
    if (_autochange==2 && _autoChangeTimer == NULL)
    {
        QTime scheduledTime(_time_hours, _time_minutes);

        // Default of midnight if time is invalid for some reason...
        if (!scheduledTime.isValid())
        {
            scheduledTime.setHMS(23,59,59);
        }

        int milliSecondsToGo = QTime::currentTime().msecsTo(scheduledTime);

        // If time is in the past, schedule it for tomorrow...
        if (milliSecondsToGo < 0)
        {
            // 0,0 is the start of the day whereas 23,59 + 1 min is tonight at midnight...
            int msUntilMidnight = QTime::currentTime().msecsTo(QTime(23,59)) + 60000;
            milliSecondsToGo = msUntilMidnight + (QTime(0,0).msecsTo(scheduledTime));
        }

        // Some padding for good measure to avoid firing more than once at the scheduled time
        milliSecondsToGo += 3000;

        _autoChangeTimer = new QTimer(QCoreApplication::instance());

        connect(_autoChangeTimer, &QTimer::timeout, this, &MainWindow::update_all);
        connect(_autoChangeTimer, &QTimer::timeout, this, &MainWindow::check_content);
        connect(_autoChangeTimer, &QTimer::timeout, [this]
        {
            // reset timer to avoid firing more than once.
            _autoChangeTimer->stop();
            _autoChangeTimer=NULL;

            QThread::msleep(100);

            set_autochange();
        });

        _autoChangeTimer->start(milliSecondsToGo);
    }
}

void MainWindow::check_content()
{
    if(get_date_list()==true)
    {
        delete_old_pictures();
    }
}

void MainWindow::no_autochange()
{
    if (_autoChangeTimer != NULL)
    {
        _autoChangeTimer->stop();
        disconnect(_autoChangeTimer, &QTimer::timeout, this, &MainWindow::update_all);
        disconnect(_autoChangeTimer, &QTimer::timeout, this, &MainWindow::check_content);
        delete _autoChangeTimer;
        _autoChangeTimer = NULL;
    }
}

void MainWindow::load_settings()
{
    QSettings settings(_confFile, QSettings::NativeFormat);

    settings.beginGroup("SETTINGS");
    _picturedir = settings.value("picturedir","").toString();
    _autorun = settings.value("autorun","").toInt();
    _store_days = settings.value("store_days","").toInt();
    settings.endGroup();

    settings.beginGroup("SETWALLPAPER");
    _autochange = settings.value("autochange","").toInt();
    _used_desktop = settings.value("used_desktop","").toInt();
    _time_hours = settings.value("time_hours","").toInt();
    _time_minutes = settings.value("time_minutes","").toInt();
    settings.endGroup();

    settings.beginGroup("PROVIDER_SETTINGS");
    _selected_provider = settings.value("selected_provider","").toInt();
    _lang = settings.value("lang","").toString();
    settings.endGroup();
}

bool MainWindow::check_internet_connection()
{
    QNetworkReply *reply;
    QNetworkAccessManager dl_manager;

    bool _error=false;
    QString _check_url = "https://www.wikimedia.org";
    QEventLoop loop;
    QObject::connect(&dl_manager,&QNetworkAccessManager::finished,&loop,&QEventLoop::quit);
    reply = dl_manager.get(QNetworkRequest(_check_url));
    loop.exec();

    if (!reply->bytesAvailable())
    {
        _error=true;
    }
    else
    {
        _error=false;
    }

    reply->close();
    loop.exit();

    if(_error==true)
    {
        return false;
    }

    return true;
}

void MainWindow::request_dl_wallpaper()
{
    if(check_internet_connection()==true)
    {
        switch (_selected_provider)
        {
        case 1:
            _wikimedia_commons_potd.get_wikimedia_commons_potd(false, _picturedir, _lang, 0, 0, 0);
            break;
        }
    }
    else
    {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [this]
        {
            if(check_internet_connection()==true)
            {
                update_all();
                timer->stop();
            }
        });
        timer->start(5000);
    }
}

void MainWindow::create_MenuHead(QString description, QString title, QString thumbfile)
{
    _descWidget = new QWidget();
    dL = new QVBoxLayout();
    _imageLabel = new QLabel();
    _labelTitle = new QLabel(title);
    _labelDescription = new QLabel(description);

    _imageLabel->setAlignment(Qt::AlignCenter);
    _labelTitle->setAlignment(Qt::AlignCenter);
    _labelDescription->setAlignment(Qt::AlignCenter);

    _headImage.load(_thumbfiledir+"/"+thumbfile);
    _descImage = _headImage.scaled(280,170, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    _imageLabel->setPixmap(QPixmap::fromImage(_descImage));

    _labelDescription->setWordWrap(true);
    _labelDescription->setStyleSheet("font: 8pt; font-style: italic; text-align:center");
    _labelTitle->setStyleSheet("font: 8pt; font-weight: bold; text-align:center;");

    dL->addWidget(_labelTitle);
    dL->addWidget(_imageLabel);
    dL->addWidget(_labelDescription);
    _descWidget->setLayout(dL);
    _descWidget->show();
}

void MainWindow::create_Menu()
{
    basemenu = new QMenu(this);
    provider = new QMenu(this);

    _widgetaction = new QWidgetAction(basemenu);
    _widgetaction->setDefaultWidget(_descWidget);
    basemenu->addAction(_widgetaction);
    basemenu->addSeparator();

    // basemenu->addAction(getnewpicture);

    basemenu->addAction(moreinformation);

    switch(_selected_provider)
    {
    case 1:
        basemenu->addAction(wmc_potd_morepictures);
        break;
    }

    basemenu->addSeparator();
    basemenu->addAction(loadexistingpicture);
    basemenu->addSeparator();

    provider = basemenu->addMenu("&Provider");
    provider->addAction(provider_wikimedia_commons_potd);

    basemenu->addAction(settings);
    basemenu->addAction(aboutapp);
    basemenu->addSeparator();
    basemenu->addAction(quitapp);
}

void MainWindow::create_Actions()
{
    /*
    getnewpicture = new QAction(tr("&Get new picture"), this);
    connect(getnewpicture, &QAction::triggered, this, &MainWindow::mnu_getnewpicture);
    */

    moreinformation = new QAction(tr("&About this picture and license"), this);
    connect(moreinformation, &QAction::triggered, this, &MainWindow::mnu_moreinformation);

    switch (_selected_provider)
    {
    case 1:
        wmc_potd_morepictures = new QAction(tr("&Download past pictures"), this);
        connect(wmc_potd_morepictures, &QAction::triggered, this, &MainWindow::wikimedia_commons_more_pictures);
        break;
    }

    provider_wikimedia_commons_potd = new QAction(tr("&Wikimedia Commons - Picture of the day"), this);
    connect(provider_wikimedia_commons_potd, &QAction::triggered, this, &MainWindow::providermnu_wikipedia_commons_potd);

    loadexistingpicture = new QAction(tr("&Load existing picture"), this);
    connect(loadexistingpicture, &QAction::triggered, this, &MainWindow::basemnu_loadexistingpicture);

    settings = new QAction(tr("&Settings"), this);
    connect(settings, &QAction::triggered, this, &MainWindow::basemnu_settings);

    aboutapp = new QAction(tr("&About"), this);
    connect(aboutapp, &QAction::triggered, this, &MainWindow::basemnu_aboutapp);

    quitapp = new QAction(tr("&Quit"), this);
    connect(quitapp, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWindow::wikimedia_commons_more_pictures()
{
    if(check_internet_connection()==true)
    {
        show_photobrowser(1);
    }
    else
    {
        QApplication::setQuitOnLastWindowClosed(false);
        QMessageBox MsgError;
        MsgError.setIcon(QMessageBox::Warning);
        MsgError.setText("No connection to the internet.");
        MsgError.exec();
    }
}

void MainWindow::mnu_getnewpicture()
{
    request_dl_wallpaper();
    get_last_record(_selected_provider);
    update_menu();
}

void MainWindow::mnu_moreinformation()
{
    QDesktopServices::openUrl(_db_rec_url);
}

void MainWindow::basemnu_loadexistingpicture()
{
    if(get_date_list()==true)
    {
        delete_old_pictures();
    }

    show_photobrowser(0);
}

void MainWindow::basemnu_settings()
{
    SettingsWindow _sett_win;
    _sett_win.setFixedSize(_sett_win.size());
    _sett_win.adjustSize();
    _sett_win.move(QApplication::desktop()->screen()->rect().center() - _sett_win.rect().center());
    _sett_win.setModal(true);
    _sett_win.exec();
}

void MainWindow::basemnu_aboutapp()
{
    About _about_win;
    _about_win.adjustSize();
    _about_win.move(QApplication::desktop()->screen()->rect().center() - _about_win.rect().center());
    _about_win.setFixedSize(_about_win.size());
    _about_win.setModal(true);
    _about_win.exec();
}

void MainWindow::providermnu_wikipedia_commons_potd()
{
    change_provider(1);
}

void MainWindow::set_SystemTrayIcon()
{
    mSystemTrayIcon->setIcon(QIcon(":/ultimatedesktopwallpaper_icon.png"));
    mSystemTrayIcon->show();
    mSystemTrayIcon->setVisible(true);
}

void MainWindow::_display_tooltip(QString _tooltip_title, QString _tooltip_message)
{
    if(!_tooltip_message.isEmpty())
    {
        QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);
        mSystemTrayIcon->showMessage(_tooltip_title, _tooltip_message, icon, 5000);
        mSystemTrayIcon->setToolTip("Description: \n"+_tooltip_message);
    }
}

void MainWindow::change_provider(int _selected_provider)
{
    QSettings _select_provider_settings(_confFile, QSettings::IniFormat);

    _select_provider_settings.beginGroup("PROVIDER_SETTINGS");
    _select_provider_settings.setValue("selected_provider", _selected_provider);
    _select_provider_settings.endGroup();
    _select_provider_settings.sync();

    update_menu();
}

void MainWindow::get_last_record(int provider)
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
    udw_db.open();

    udw_query.prepare("SELECT description FROM udw_history WHERE provider=\""+QString::number(provider)+"\"");
    udw_query.exec();
    while(udw_query.next()){
        if(udw_query.last()) {
            _db_rec_description = udw_query.value(0).toString();
        }
    }
    udw_query.prepare("SELECT title FROM udw_history WHERE provider=\""+QString::number(provider)+"\"");
    udw_query.exec();
    while(udw_query.next()){
        if(udw_query.last()) {
            _db_rec_title = udw_query.value(0).toString();
        }
    }
    udw_query.prepare("SELECT provider FROM udw_history WHERE provider=\""+QString::number(provider)+"\"");
    udw_query.exec();
    while(udw_query.next()){
        if(udw_query.last()) {
            _db_rec_provider = udw_query.value(0).toString();
        }
    }
    udw_query.prepare("SELECT filename FROM udw_history WHERE provider=\""+QString::number(provider)+"\"");
    udw_query.exec();
    while(udw_query.next()){
        if(udw_query.last()) {
            _db_rec_filename=udw_query.value(0).toString();
        }
    }
    udw_query.prepare("SELECT thumb_filename FROM udw_history WHERE provider=\""+QString::number(provider)+"\"");
    udw_query.exec();
    while(udw_query.next()){
        if(udw_query.last()) {
            _db_rec_thumb_filename=udw_query.value(0).toString();
        }
    }
    udw_query.prepare("SELECT size_width FROM udw_history WHERE provider=\""+QString::number(provider)+"\"");
    udw_query.exec();
    while(udw_query.next()){
        if(udw_query.last()) {
            _db_rec_size_width = udw_query.value(0).toInt();
        }
    }
    udw_query.prepare("SELECT size_height FROM udw_history WHERE provider=\""+QString::number(provider)+"\"");
    udw_query.exec();
    while(udw_query.next()){
        if(udw_query.last()) {
            _db_rec_size_height = udw_query.value(0).toInt();
        }
    }
    udw_query.prepare("SELECT browser_url FROM udw_history WHERE provider=\""+QString::number(provider)+"\"");
    udw_query.exec();
    while(udw_query.next()){
        if(udw_query.last()) {
            _db_rec_url = udw_query.value(0).toString();
        }
    }
    udw_query.finish();
    udw_query.clear();
}

void MainWindow::runscript(QString content)
{
    QString _scriptfilepath = _AppFileDir+"/"+"_genscript.sh";

    QFile scriptfile(_scriptfilepath);
    if(!scriptfile.exists(_scriptfilepath))
    {
        if (scriptfile.open(QIODevice::Append))
        {
            QTextStream stream(&scriptfile);
            stream << content << Qt::endl;
        }
    }

    QProcess proc;
    proc.startDetached("/bin/bash "+_scriptfilepath);
    if(proc.Running)
    {
        QThread::msleep(1000);
        scriptfile.remove();
        proc.close();
    }
}

void MainWindow::load_wallpaper()
{
    if(_autochange==2)
    {
        emit _setWallpaper(_picturedir+"/"+_db_rec_filename, _used_desktop);
    }
}

bool MainWindow::get_date_list()
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
    if (!udw_db.open())
    {
        return false;
    }

    udw_query.exec("SELECT date FROM udw_history");
    QSqlRecord record_date = udw_query.record();
    while (udw_query.next())
    {
        datelist.append(udw_query.value(record_date.indexOf("date")).toString());
    }
    udw_query.finish();
    udw_query.clear();

    return true;
}

bool MainWindow::create_filenamelist()
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
    if (!udw_db.open())
    {
        return false;
    }

    for(int i = 0; i < selected_datelist.size(); i++)
    {
        udw_query.prepare("SELECT filename FROM udw_history WHERE date=\""+selected_datelist.at(i).toUtf8()+"\"");
        if(udw_query.exec() && udw_query.next())
        {
           if(!filenamelist.contains(udw_query.value(0).toString().toUtf8()))
           {
               filenamelist.append(udw_query.value(0).toString().toUtf8());
           }
        }
        udw_query.prepare("SELECT thumb_filename FROM udw_history WHERE date=\""+selected_datelist.at(i).toUtf8()+"\"");
        if(udw_query.exec() && udw_query.next())
        {
           if(!_oldthumbfiles.contains(udw_query.value(0).toString().toUtf8()))
           {
               _oldthumbfiles.append(udw_query.value(0).toString().toUtf8());
           }
        }
    }

    //Check if more entries with the same date and add it.
    while(udw_query.next()){
        if(!filenamelist.contains(udw_query.value(0).toString().toUtf8()))
        {
            filenamelist.append(udw_query.value(0).toString().toUtf8());
        }
    }
    while(udw_query.next()){
        if(!_oldthumbfiles.contains(udw_query.value(0).toString().toUtf8()))
        {
            _oldthumbfiles.append(udw_query.value(0).toString().toUtf8());
        }
    }
    udw_query.finish();
    udw_query.clear();

    return true;
}

void MainWindow::delete_old_records()
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

    for(int i = 0; i < selected_datelist.size(); i++)
    {
        udw_query.prepare("DELETE FROM udw_history WHERE date = \""+selected_datelist.at(i).toUtf8()+"\"");
        if(!udw_query.exec())
        {
            qDebug() << udw_query.lastError();
        }
    }
    udw_query.finish();
    udw_query.clear();
}

void MainWindow::delete_old_pictures()
{
    for(int i = 0; i < datelist.size(); i++)
    {
        QDate current_date = QDate::currentDate();
        QDate selected_date = QDate::fromString(datelist.at(i), "yyyyMMdd");
        if(selected_date.daysTo(current_date)>_store_days) {
            selected_datelist.append(datelist.at(i));
        }
    }

    if(create_filenamelist()==true)
    {
        for(int j = 0; j < filenamelist.size(); j++)
        {
            QString oldwallfile = _picturedir+"/"+filenamelist.at(j);
            QFile old_wallp(oldwallfile);
            old_wallp.remove();
            qDebug() << "Picture " << oldwallfile << " deleted.";
            oldwallfile.clear();

            QString oldthumbfile = _thumbfiledir+"/"+"thumb_"+filenamelist.at(j);
            QFile old_thumbf(oldthumbfile);
            old_wallp.remove();
            qDebug() << "Thumbnail-Picture " << oldthumbfile << " deleted.";
            oldthumbfile.clear();
        }
        delete_old_records();
    }
}

void MainWindow::update_menu()
{
    basemenu->clear();
    create_MenuHead(_db_rec_description, _db_rec_title, _db_rec_thumb_filename);
    create_Actions();
    create_Menu();
}

void MainWindow::update_all()
{
    request_dl_wallpaper();
    get_last_record(_selected_provider);
    update_menu();
    if(_autochange==2)
    {
        load_wallpaper();
    }
    _display_tooltip(_db_rec_title, _db_rec_description);
}

void MainWindow::show_photobrowser(int mode)
{
    /* *****************************************************
     * mode variants
     * *****************************************************
     *
     *  0: Show stored pictures
     *
     *  1: Show Wikimedia Commons pictures of the day
     *     of past days within a given time range
     *
     * *****************************************************
     */

    PhotoBrowser _photobrowser;
    _photobrowser.adjustSize();
    _photobrowser.move(QApplication::desktop()->screen()->rect().center() - _photobrowser.rect().center());
    _photobrowser.init(mode);
    _photobrowser.setModal(true);
    _photobrowser.exec();

    mSystemTrayIcon->setToolTip("");

    if (_photobrowser.wallchanged==true)
    {
       _photobrowser.wallchanged=false;

       _db_rec_description=_photobrowser._pb_copyright_description_photo;
       _db_rec_title=_photobrowser._pb_headline;
       _db_rec_thumb_filename=_photobrowser._thumb_filename;
       _db_rec_url=_photobrowser._pb_copyright_link;

       update_menu();
    }
}
