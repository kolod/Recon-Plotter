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

#include "chartwindow.h"

#include <QDebug>
#include <QFileInfo>
#include <QLayout>
#include <QMargins>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QPointer>
#include <QPrintPreviewDialog>

#include "analogsignal.h"
#include "utils.h"

ChartWindow::ChartWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMdiSubWindow(parent, flags), mDataFile(nullptr) {
    setAttribute(Qt::WA_DeleteOnClose, true);

    mChartView.setInteractions(QCP::iRangeDrag | QCP::iRangeZoom |
                               QCP::iSelectPlottables | QCP::iMultiSelect);
    mChartView.xAxis->setLabel(tr("Time, s"));
    mChartView.yAxis->setLabel(tr("Voltage, V"));

    setWidget(&mChartView);
}

void ChartWindow::closeEvent(QCloseEvent *event) {
    if (maybeSave()) {
        QMdiSubWindow::closeEvent(event);
        event->accept();
    } else {
        event->ignore();
    }
}

void ChartWindow::setDataFile(DataFile *datafile) {
    if (mDataFile != nullptr) delete mDataFile;

    mDataFile = datafile;

    if (mDataFile != nullptr) {
        connect(mDataFile, &DataFile::modifiedChanged, this,
                &QMdiSubWindow::setWindowModified);
        setWindowTitle(mDataFile->fileName() + "[*]");
    }
}

QString ChartWindow::userFriendlyCurrentFile() {
    return QFileInfo(mDataFile->fileName()).fileName();
};

bool ChartWindow::maybeSave() {
    if (mDataFile->isModified()) {
        switch (QMessageBox::warning(
            this, tr("Recon Plotter"),
            tr("'%1' has been modified.\nDo you want to save your changes?")
                .arg(mDataFile->fileName()),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel)) {
            case QMessageBox::Save:
                save();
                return true;
            case QMessageBox::Cancel:
                return false;
            default:
                break;
        }
    }
    return true;
}

void ChartWindow::save() {
    if (mDataFile != nullptr) {
        if (mDataFile->isRenameNeeded()) {
            saveAs();
        } else {
            if (mDataFile->save()) {
                addToRecent(mDataFile->fileName());
            }
        }
    }
}

void ChartWindow::saveAs() {
    if (mDataFile != nullptr) {
        QFileDialog *dialog = new QFileDialog(this);

        dialog->setNameFilter("Plot Data File (*.plot)");
        dialog->setAcceptMode(QFileDialog::AcceptSave);
        dialog->setWindowTitle(tr("Save Plot Data File"));
        dialog->selectFile(fixFileSuffix(mDataFile->fileName(), "plot"));

        connect(dialog, &QFileDialog::accepted, this, [this, dialog]() {
            auto files = dialog->selectedFiles();
            if (!files.isEmpty())
                if (mDataFile->saveAs(files.first()))
                    addToRecent(files.first());
        });

        dialog->open();
    }
}

void ChartWindow::refresh() {
    mChartView.clearGraphs();
    mChartView.xAxis->setRange(mDataFile->left(), mDataFile->right());
    mChartView.yAxis->setRange(mDataFile->bottom(), mDataFile->top());
    mChartView.legend->setVisible(true);

    for (qsizetype i = 0; i < mDataFile->analogSignalsCount(); i++) {
        auto *signal = mDataFile->analogSignal(i);
        if (signal->selected()) {
            auto *graph = mChartView.addGraph();
            graph->setData(mDataFile->time(), signal->smoothed(), true);
            graph->setName(signal->name(true));
            graph->setPen(QPen(signal->color()));
            graph->setVisible(true);
        }
    }

    mChartView.replot();
}

void ChartWindow::print() {
    auto *dialog = new QPrintPreviewDialog(
        this, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    if (dialog != nullptr) {
        QPrinter *printer = dialog->printer();
        if (printer != nullptr) {
            printer->setPageOrientation(QPageLayout::Landscape);
            printer->setResolution(600);
        }

        connect(dialog, &QPrintPreviewDialog::paintRequested, this,
                [this](QPrinter *printer) {
                    QCPPainter painter;
                    painter.begin(printer);
                    painter.setCompositionMode(
                        QPainter::CompositionMode_SourceAtop);

                    auto rect = printer->pageLayout().paintRectPixels(
                        printer->resolution());
                    // auto rect = printer->pageLayout().paintRectPoints();

                    double factor = 1.00;

                    painter.save();
                    painter.scale(factor, factor);

                    qDebug()
                        << "Painter window            :" << painter.window()
                        << Qt::endl
                        << "Painter viewport          :" << painter.viewport()
                        << Qt::endl
                        << "Printer page size         :"
                        << printer->pageLayout().pageSize() << Qt::endl
                        << "Printer paint rect        :"
                        << printer->pageLayout().paintRect() << Qt::endl
                        << "Printer paint rect pixels :"
                        << printer->pageLayout().paintRectPixels(
                               printer->resolution())
                        << Qt::endl
                        << "Printer paint rect points :"
                        << printer->pageLayout().paintRectPoints() << Qt::endl;

                    mChartView.toPainter(&painter, rect.width(), rect.height());
                    painter.restore();

                    painter.end();
                });

        dialog->open();
    }
}
