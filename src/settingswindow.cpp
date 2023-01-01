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

#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "mainwindow.h"

#include <QFile>
#include <QSettings>
#include <QApplication>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QCloseEvent>
#include <QProcess>
#include <QThread>
#include <QTextStream>

SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->pushButton, &QPushButton::clicked, this, &SettingsWindow::selectWallpaperDir);
    QObject::connect(ui->pushButton_5, &QPushButton::clicked, this, &SettingsWindow::reset);
    QObject::connect(ui->pushButton_4, &QPushButton::clicked, this, &SettingsWindow::close);
    QObject::connect(ui->pushButton_3, &QPushButton::clicked, this, &SettingsWindow::save_changes);

    set_values();
}

void SettingsWindow::closeEvent(QCloseEvent * event)
{
    event->ignore();
    this->hide();
}

void SettingsWindow::reject()
{
    this->hide();
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::set_values()
{
    _iniFilePath = QDir::homePath()+"/.UltimateDailyWallpaper/settings.conf";

    QSettings settings(_iniFilePath, QSettings::NativeFormat);

    settings.beginGroup("SETTINGS");
    _WallpaperDir = settings.value("picturedir","").toString();
    ui->checkBox_2->setCheckState((Qt::CheckState)settings.value("autorun", 0).toInt());
    _delete_older_than = settings.value("store_days","").toInt();
    settings.endGroup();

    settings.beginGroup("SETWALLPAPER");
    ui->checkBox_3->setCheckState((Qt::CheckState)settings.value("autochange", 0).toInt());
    int _Parameter = settings.value("used_desktop","").toInt();
    int _time_hours = settings.value("time_hours","").toInt();
    int _time_minutes = settings.value("time_minutes","").toInt();
    settings.endGroup();

    ui->lineEdit->setReadOnly(true);
    ui->label_3->setEnabled(false);
    ui->comboBox->setEnabled(false);
    ui->lineEdit->setText(_WallpaperDir);
    ui->spinBox->setValue(_delete_older_than);

    if (ui->checkBox_3->isChecked() == true)
    {
        ui->checkBox_3->setChecked(true);
        ui->label_3->setEnabled(true);
        ui->comboBox->setEnabled(true);

        QTime time(_time_hours, _time_minutes);
        ui->timeEdit->setTime(time);
    }
    else
    {
        ui->timeEdit->setDisabled(true);
    }

    ui->comboBox->addItem("Budgie");
    ui->comboBox->addItem("Cinnamon");
    ui->comboBox->addItem("XFCE");
    ui->comboBox->addItem("LXDE");
    ui->comboBox->addItem("MATE");
    ui->comboBox->addItem("Trinity Desktop Environment (TDE)");
    ui->comboBox->addItem("KDE Plasma 5");
    ui->comboBox->addItem("LXQt");
    ui->comboBox->addItem("GNOME 3");

    ui->comboBox->setCurrentIndex(_Parameter);
}

void SettingsWindow::write_settings()
{
    QString combobox_index;
    combobox_index.sprintf("%d", ui->comboBox->currentIndex());

    _WallpaperDir = ui->lineEdit->text();

    _delete_older_than = ui->spinBox->value();

    QSettings settings(_iniFilePath, QSettings::IniFormat);

    settings.beginGroup("SETTINGS");
    settings.setValue("picturedir", _WallpaperDir);
    settings.setValue("autorun", (int)ui->checkBox_2->checkState());
    settings.setValue("store_days", _delete_older_than);
    settings.endGroup();
    settings.sync();

    settings.beginGroup("SETWALLPAPER");
    settings.setValue("autochange", (int)ui->checkBox_3->checkState());
    settings.setValue("used_desktop", combobox_index);
    settings.setValue("time_hours", ui->timeEdit->time().hour());
    settings.setValue("time_minutes", ui->timeEdit->time().minute());
    settings.endGroup();
    settings.sync();
}

void SettingsWindow::close()
{
    this->hide();
}

void SettingsWindow::save_changes()
{
    write_settings();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Save changes", "The application will be restart for "
                                                 "the changes to take effect. Do you want save the changes?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QProcess::startDetached(QCoreApplication::applicationFilePath());
        exit(-1);
    }

    this->hide();
}

void SettingsWindow::selectWallpaperDir()
{
    select_dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
                                                 QDir::homePath(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    ui->lineEdit->setText(select_dir);
}

void SettingsWindow::on_checkBox_3_clicked()
{
    if(ui->checkBox_3->isChecked() == true) {
        ui->label_3->setEnabled(true);
        ui->label_5->setEnabled(true);
        ui->comboBox->setEnabled(true);
        ui->timeEdit->setEnabled(true);
    } else {
        ui->label_3->setDisabled(true);
        ui->label_5->setDisabled(true);
        ui->comboBox->setDisabled(true);
        ui->timeEdit->setDisabled(true);
    }
}

void SettingsWindow::reset()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Reset", "This will reset all settings to the default settings. "
                                                 "All existing wallpapers will be deleted. "
                                                 "Do you want to continue?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        QString _AppFileDir=QDir::homePath()+"/.UltimateDailyWallpaper";
        QString runscript("#!/bin/bash\n"
                   "homeDir="+_AppFileDir+"\n"
                   "\n"
                   "rm $homeDir\'/udw_database.sqlite\'\n"
                   "rm $homeDir\'/settings.conf\'\n"
                   "rm -r $homeDir");

        QFile scriptfile(_AppFileDir+"/"+"_genscript.sh");
        if(!scriptfile.exists(_AppFileDir+"/"+"_genscript.sh"))
        {
            if (scriptfile.open(QIODevice::Append))
            {
                QTextStream stream(&scriptfile);
                stream << runscript << Qt::endl;
            }
        }

        QProcess _goscript;
        _goscript.start("/bin/bash "+_AppFileDir+"/"+"_genscript.sh");
                if(_goscript.waitForFinished())
                {
                    QThread::msleep(100);
                    scriptfile.remove();
                    QProcess::startDetached(QCoreApplication::applicationFilePath());
                    exit(-1);
                }
    }

    this->hide();
}
