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

#ifndef PHOTOBROWSER_H
#define PHOTOBROWSER_H

#include "setwallpaper.h"
#include "interfaces.h"
#include "addrecord.h"

#include <QDialog>
#include <QModelIndex>
#include <QFileSystemModel>
#include <QStandardItemModel>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

namespace Ui {
class PhotoBrowser;
}

class PhotoBrowser : public QDialog
{
    Q_OBJECT

public:
    explicit PhotoBrowser(QWidget *parent = 0);
    ~PhotoBrowser();

    void init(int mode);

    bool wallchanged;

    QString _photobrowser_specific_wallpaperfilename;
    QString _pb_copyright_description_photo;
    QString _pb_headline;
    QString _pb_copyright_link;
    QString _thumb_filename;
    QString _pluginfile;

signals:
    void UpdateItem(int, QImage);
    void go_setWallpaper(QString _wallpaperfile);

private slots:
    void on_pushButton_clicked();
    void setWallpaperFile();
    void on_listView_clicked();
    void _getmoreInformationsButton_clicked();

private:
    Ui::PhotoBrowser *ui;
    setWallpaper _setwall;
    addrecord add_record;
    BasicInterface * basicinterface;
    QVBoxLayout *_layout_details;
    QLabel *_label_Details;
    QLabel *_label_headline;
    QLabel *_label_headline1;
    QLabel *_label_description_and_copyright1;
    QLabel *_label_description_and_copyright;
    QLabel *_label_image_size1;
    QLabel *_label_image_size;
    QLabel *_preview_text;
    QLabel *_previewImageLabel;
    QPushButton *_getmoreInformations;

    QLabel _description;
    QLabel _headline;

    QString _WallpaperDir;
    QString _wallpaperfile;
    QString _resolution;
    QString filename_new;
    QString _thumbfiledir;
    QString _thumbfilename;
    QString _databaseFilePath;
    QString _lang;
    QString _tempdirpath;
    QString _output_value;
    QString _pluginsdir;
    QString _selected_plugin;

    QStringList _filenamelist;
    QStringList _filenamelist_with_fullpath;
    QStringList _thumbfilelist;
    QStringList _idlist;

    int _scaled_picture_width;
    int _scaled_picture_height;
    int _size_height;
    int _size_width;
    int _Parameter;
    int _mode;

    bool running;
    bool _get_specific_values;
    bool _first_run;

    bool loadPlugin(QString _pluginfilename);
    bool create_temp_database();
    bool select_single_value(QString desired_column, QString db_filepath);
    void refresh();
    void refresh_listview();
    void download_and_set();
    void download_thumb(int days);    
    void show();
    void openURL();
    void _init_ImageList();
    void _read_settings();
    void removeWallpaperFile();
    void _readResolution();
    void _get_values();
    void _display_details();
    void _select_first_item(const QModelIndex & index);
    void deltempdir();
    void loadPlugin();
    void closeEvent(QCloseEvent * event);
    void reject();
};

#endif // PHOTOBROWSER_H
