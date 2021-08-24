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
#include "analogsignal.h"

class DataFile : public QObject
{
	Q_OBJECT

public:
	explicit DataFile(QObject *parent = nullptr);

    auto fileName()       const {return mFileName;}
    auto title()          const {return mTitle;}
    auto device()         const {return mDevice;}
	auto &time()                {return mTime;}
    auto minX()           const {return mMinX;}
    auto maxX()           const {return mMaxX;}
    auto minY()           const {return mMinY;}
    auto maxY()           const {return mMaxY;}
    auto left()           const {return mLeft;}
    auto right()          const {return mRight;}
    auto bottom()         const {return mBottom;}
    auto top()            const {return mTop;}
    bool isRenameNeeded() const {return !mFileName.endsWith(".plot");}
	bool isModified()     const {return mModified;}

	auto analogSignalsCount() {return mAnalogSignals.count();}
	auto *analogSignal(int channel) {return mAnalogSignals[channel];}

	bool save();
	bool saveAs(QString filename);
	bool open(QString filename);
	void calculateLimits();
	void resetWindow();

    void setModified(bool modified=true) {
        mModified = modified;
        emit modifiedChanged(modified);
	}

    void setSelected(qsizetype channel, bool selected) {
        if (channel < mAnalogSignals.count()) {
            mAnalogSignals.at(channel)->setSelected(selected);
            emit selectedChanged(channel, selected);
        }
    }

    void setColor(qsizetype channel, QColor color) {
        if (channel < mAnalogSignals.count()) {
            mAnalogSignals.at(channel)->setColor(color);
            emit colorChanged(channel, color);
        }
    }

public slots:
	void cansel() {mCansel = true;}

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
	QVector<double> mTime;
    bool mModified;
	bool mCansel;

signals:
	void updateProgressShow(bool state);
	void updateProgressValue(int progress);
	void updateProgressRange(int min, int max);
	void dataLoaded();
	void modifiedChanged(bool modified);
    void selectedChanged(qsizetype channel, bool state);
    void colorChanged(qsizetype channel, QColor color);
};
