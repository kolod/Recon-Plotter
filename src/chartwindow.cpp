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
#include "analogsignal.h"

ChartWindow::ChartWindow(QWidget *parent, Qt::WindowFlags flags)
	: QMdiSubWindow(parent, flags)
	, mChart(nullptr)
    , mDataFile(nullptr)
{
	setAttribute(Qt::WA_DeleteOnClose, true);
	mChartView = new QChartView(this);
	mChartView->setRenderHint(QPainter::Antialiasing);
	setWidget(mChartView);

	mTimeAxis = new QValueAxis;
	mTimeAxis->setLabelFormat("%g");
	mTimeAxis->setTitleText(tr("Time, S"));

	mVoltageAxis = new QValueAxis;
	mVoltageAxis->setTitleText(tr("Voltage, V"));
	mVoltageAxis->setGridLineVisible();
	mVoltageAxis->setMinorTickCount(4);

	mCurrentAxis = new QValueAxis;
	mCurrentAxis->setTitleText(tr("Current, A"));
	mCurrentAxis->setGridLineVisible();
	mCurrentAxis->setMinorTickCount(4);mChart = new QChart;

	mChart->addAxis(mTimeAxis   , Qt::AlignBottom);
	mChart->addAxis(mCurrentAxis, Qt::AlignLeft);
	mChart->addAxis(mVoltageAxis, Qt::AlignRight);
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

void ChartWindow::refresh()
{
	mChart->removeAllSeries();

	for (qsizetype i = 0; i < mDataFile->analogSignalsCount(); i++) {
		auto *signal = mDataFile->analogSignal(i);
		if (signal->selected()) {
			auto line = signal->lineSeries();
			mChart->addSeries(line);
			if (mVoltageAxis != nullptr) line->attachAxis(mVoltageAxis);
			if (mTimeAxis != nullptr) line->attachAxis(mTimeAxis);
		}
	}

	mChart->setTitle(mDataFile->title());
	mChart->setAnimationOptions(QChart::SeriesAnimations);
	mChart->legend()->hide();
	mChart->createDefaultAxes();

	mChartView->setChart(mChart);
}
