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

#include <QObject>
#include <QFile>
#include <QList>
#include <QtCharts/QLineSeries>
#include "analogsignal.h"

#if QT_VERSION_MAJOR < 6
using namespace QtCharts;
#endif

class DataFile : public QObject
{
	Q_OBJECT

public:
	explicit DataFile(QObject *parent = nullptr);
	int analogSignalsCount() {return mAnalogSignals.count();}
	AnalogSignal *analogSignal(int channel) {return mAnalogSignals[channel];}
	QString fileName() const {return mFileName;}
	QString title() const {return mTitle;}
	QString device() const {return mDevice;}

	// Signal limits
	double minX()   {return mMinX;}
	double maxX()   {return mMaxX;}
	double minY()   {return mMinY;}
	double maxY()   {return mMaxY;}

	// Plot window limits
	double left()   {return mLeft;}
	double right()  {return mRight;}
	double bottom() {return mBottom;}
	double top()    {return mTop;}

	bool save();
	bool saveAs(QString filename);
	bool open(QString filename);
	bool isModified() const {return mModified;}
	void setModified() {
		mModified = true;
		emit modifiedChanged(true);
	}

	bool isRenameNeeded() const {return mRenameNeeded;}

protected:
	QString mFileName;
	QString mTitle;
	QString mDevice;
	QString mOriginalFileName;
	QString mLabelX;
	QString mLabelY;
	double mLeft;
	double mRight;
	double mBottom;
	double mTop;
	double mMinX;
	double mMaxX;
	double mMinY;
	double mMaxY;
	QList<AnalogSignal*> mAnalogSignals;
	QList<qreal> mTime;
	bool mModified;
	bool mRenameNeeded;

signals:
	void updateProgressShow(bool state);
	void updateProgressValue(int progress);
	void updateProgressRange(int min, int max);
	void dataLoaded();
	void modifiedChanged(bool modified);
};
