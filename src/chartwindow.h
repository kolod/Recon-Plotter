//    Recon Plotter
//    Copyright (C) 2021  Oleksandr Kolodkin <alexandr.kolodkin@gmail.com>
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <QMdiSubWindow>
#include <QPointer>
#include <QPrinter>
#include "datafile.h"
#include "qcustomplot.h"

class ChartWindow : public QMdiSubWindow {
    Q_OBJECT

   public:
    explicit ChartWindow(QWidget *parent = nullptr,
                         Qt::WindowFlags flags = Qt::WindowFlags());

    DataFile *dataFile() const { return mDataFile; }
    QString userFriendlyCurrentFile();
    void setDataFile(DataFile *datafile = nullptr);

   public slots:
    void save();
    void saveAs();
    void print();
    void refresh();

   protected:
    void closeEvent(QCloseEvent *event) override;

   private:
    DataFile *mDataFile;
    QCustomPlot mCustomPlot;

    QCPGraph *findGraph(QString name);

    bool maybeSave();
};
