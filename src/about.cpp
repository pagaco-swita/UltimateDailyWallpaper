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

#include "about.h"
#include "ui_about.h"

#include <QDesktopServices>
#include <QCloseEvent>

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    ui->label_2->setText("Application name: "+QApplication::applicationName());

    QString version="0.0.1";
    ui->label_3->clear();
    ui->label_3->setText("Version: "+version);
}

About::~About()
{
    delete ui;
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
