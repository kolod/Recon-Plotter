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

#include <QFileInfo>
#include <QMessageBox>
#include <QMdiSubWindow>
#include "chartwindow.h"

ChartWindow::ChartWindow(QWidget *parent, Qt::WindowFlags flags)
	: QMdiSubWindow(parent, flags)
    , mDataFile(nullptr)
{
	setAttribute(Qt::WA_DeleteOnClose, true);
	mChartView = new QChartView(this);
	setWidget(mChartView);
}

void ChartWindow::closeEvent(QCloseEvent *event)
{
	if (maybeSave()) {
		QMdiSubWindow::closeEvent(event);
		event->accept();
	} else {
		event->ignore();
	}
}

bool ChartWindow::loadFile(QString filename)
{
	return false;
}

void ChartWindow::setDataFile(DataFile *datafile)
{
	if (mDataFile != nullptr)
		delete mDataFile;

	mDataFile = datafile;

	if (mDataFile != nullptr) {
		connect(mDataFile, &DataFile::modifiedChanged, this, &QMdiSubWindow::setWindowModified);
		setWindowTitle(mDataFile->fileName() + "[*]");
	}
}

QString ChartWindow::userFriendlyCurrentFile(){
	return QFileInfo(mDataFile->fileName()).fileName();
};

bool ChartWindow::maybeSave()
{
	if (mDataFile->isModified()) {
		switch (QMessageBox::warning(this,
			tr("Recon Plotter"),
			tr("'%1' has been modified.\nDo you want to save your changes?").arg(mDataFile->fileName()),
			QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
		)) {
			case QMessageBox::Save: return mDataFile->save();
			case QMessageBox::Cancel: return false;
			default: break;
		}
	}
	return true;
}
