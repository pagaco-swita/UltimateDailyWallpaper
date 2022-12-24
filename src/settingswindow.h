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

#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = 0);
    ~SettingsWindow();

private slots:
    void close();
    void save_changes();
    void on_checkBox_3_clicked();
    void reset();
    void selectWallpaperDir();

private:
    Ui::SettingsWindow *ui;
    QString _iniFilePath;
    QString select_dir;
    QString _WallpaperDir;

    bool _delete_automatically;

    int _delete_older_than;
    int _time_hours;
    int _time_minutes;

    void set_values();
    void write_settings();

    void closeEvent(QCloseEvent * event);
    void reject();
};

#endif // SETTINGSWINDOW_H
