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

#include "about.h"
#include "ui_about.h"

#include <QDesktopServices>
#include <QCloseEvent>
#include <QDate>
#include <QWidget>

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex(0);

    ui->label_2->setText(QApplication::applicationName());
    ui->label_2->setStyleSheet("font: 18pt");

    QString _version=APP_VERSION;
    ui->label_3->clear();
    ui->label_3->setText("Version: "+_version);
    ui->label_3->setStyleSheet("font: 11pt");

    QDate current_year;
    current_year=QDate::currentDate();

    QString _author="Patrice Coni";

    ui->label_5->clear();
    ui->label_5->setText("Copyright 2022 - "+current_year.toString("yyyy")+" "+_author);
}

About::~About()
{
    delete ui;
}

void About::set_plugin_text(QString plugininfo)
{
    ui->label_7->setText(plugininfo);
}

void About::closeEvent(QCloseEvent * event)
{
    event->ignore();
    this->hide();
}

void About::reject() {
    this->hide();
}

void About::on_pushButton_clicked()
{
    this->hide();
}

void About::on_pushButton_3_clicked()
{
    openURL();
    this->hide();
}

void About::openURL()
{
    QString _DefinitiveURL="https://github.com/pagaco-swita/ultimatedailywallpaper";
    QDesktopServices::openUrl(_DefinitiveURL);
}
