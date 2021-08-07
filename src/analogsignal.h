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
#include <QString>
#include <QColor>
#include <QtCharts/QLineSeries>

#if QT_VERSION_MAJOR < 6
using namespace QtCharts;
#endif

class AnalogSignal : public QObject
{
	Q_OBJECT

public:
	AnalogSignal(QObject *parent = nullptr, QString name = "", QString unit = "", qreal scale = 1.0, int smooth = 1);

	QString name() const {return mName;}
	QString unit() const {return mUnit;}
	QColor color() const {return mColor;}
	bool selected() const {return mSelected;}
	bool inverted() const {return mInverted;}
	qreal scale() const {return mScale;}
	QList<qreal> *data() {return &mData;}
	size_t dataCount() const {return mData.count();}
	unsigned int smooth() const {return mSmooth;}

	qreal time(int index) {
		return (mTime && (index < mTime->count()))? mTime->at(index) : qSNaN();
	}

	qreal left() const {return mLeft;}
	qreal right() const {return mRight;}
	qreal top() const {return mTop;}
	qreal bottom() const {return mBottom;}

	QLineSeries *lineSeries();
	QString toString();

public slots:
	void setTime(QList<qreal> *time) {
		mTime = time;
	}

	void setName(const QString name) {
		mName = name;
	}

	void setUnit(const QString unit) {
		mUnit = unit;
	}

	void setSmooth(const unsigned int smooth) {
		if (smooth > 0) mSmooth = smooth;
	}

	void setScale(const qreal scale) {
		mScale = scale;
	}

	void setSelected(const bool selected) {
		mSelected = selected;
	}

	void setInverted(const bool inverted) {
		mInverted = inverted;
	}

	void setColor(const QColor color) {
		mColor = color;
	}

	void clear();
	void calculateLimits();

private:
	QString mName;
	QString mUnit;
	QColor mColor;
	qreal mScale;
	unsigned int mSmooth;
	bool mSelected;
	bool mInverted;
	QList<qreal> mData;
	QList<qreal> *mTime;
	qreal mLeft;
	qreal mRight;
	qreal mBottom;
	qreal mTop;
};
