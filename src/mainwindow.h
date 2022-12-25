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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "download_plugins/download_wikimedia_commons_potd.h"
#include "connection.h"
#include "about.h"
#include "settingswindow.h"
#include "setwallpaper.h"
#include "photobrowser.h"

#include <QMainWindow>
#include <QAction>
#include <QSql>
#include <QSqlRelationalTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QVBoxLayout>
#include <QWidgetAction>
#include <QLabel>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QSystemTrayIcon;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


signals:
    void _setWallpaper(QString _wallpaperfile, int _set_Parameter);

private slots:
    void _display_tooltip(QString _tooltip_title, QString _tooltip_message);
    void slotActive(QSystemTrayIcon::ActivationReason r);
    void check_content();

    // basemenu
    void wikimedia_commons_more_pictures();
    void mnu_getnewpicture();
    void mnu_moreinformation();
    void basemnu_loadexistingpicture();
    void basemnu_settings();
    void basemnu_aboutapp();

    // menu provider
    void providermnu_wikipedia_commons_potd();

private:        
    setWallpaper setwall;
    download_wikimedia_commons_potd _wikimedia_commons_potd;
    QSystemTrayIcon * mSystemTrayIcon;
    QTimer * _autoChangeTimer = NULL;
    QTimer * timer;

    QStringList datelist;
    QStringList selected_datelist;
    QStringList filenamelist;
    QStringList _oldthumbfiles;

    QString _AppFileDir;
    QString _confFile;
    QString _picturedir;
    QString _lang;
    QString _databaseFilePath;
    QString _db_rec_description;
    QString _db_rec_title;
    QString _db_rec_provider;
    QString _db_rec_filename;
    QString _db_rec_thumb_filename;
    QString _db_rec_url;
    QString _thumbfiledir;

    int _db_rec_size_width;
    int _db_rec_size_height;
    int _store_days;
    int _autochange;
    int _selected_provider;
    int _used_desktop;
    int _time_hours;
    int _time_minutes;
    int _autorun;

    void set_autochange();
    void no_autochange();
    void runscript(QString content);
    void get_last_record(int provider);
    void load_settings();
    void request_dl_wallpaper();
    void set_SystemTrayIcon();
    void set_ContextMenu();
    void create_MenuHead(QString description, QString title, QString thumbfile);
    void create_Menu();
    void create_Actions();
    void load_wallpaper();
    void change_provider(int _selected_provider);
    void delete_old_pictures();
    void delete_old_records();
    void update_menu();
    void update_all();
    void show_photobrowser(int mode);
    bool check_internet_connection();
    bool get_date_list();
    bool create_filenamelist();

    // head of the basemenu
    QImage _headImage;
    QImage _descImage;
    QWidget *_descWidget;
    QVBoxLayout *dL;
    QWidgetAction *_widgetaction;
    QLabel *_imageLabel;
    QLabel *_labelTitle;
    QLabel *_labelDescription;

    // basemenu
    QMenu *basemenu;
    QMenu *provider;

    QAction *getnewpicture;
    QAction *moreinformation;
    QAction *loadexistingpicture;
    QAction *settings;
    QAction *aboutapp;
    QAction *quitapp;

    // QActions for menu provider
    QAction *wmc_potd_morepictures;
    QAction *provider_wikimedia_commons_potd;
};

#endif // MAINWINDOW_H
