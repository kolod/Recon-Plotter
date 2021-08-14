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
#include <QDataStream>
#include <QColor>
#include <QtCharts/QLineSeries>

#if QT_VERSION_MAJOR < 6
using namespace QtCharts;
#endif

class AnalogSignal : public QObject
{
	Q_OBJECT

public:
	AnalogSignal(QObject *parent = nullptr);

	QString name(bool legend = false) const;

	QString unit()  const {return mUnit;}
	QColor color()  const {return mColor;}
	bool selected() const {return mSelected;}
	double factor() const {return mFactor;}
	double scale()  const {return mScale;}
	double minY()   const {return mMinY;}
	double maxY()   const {return mMaxY;}

	QList<qreal> *data() {return &mData;}
	size_t dataCount() const {return mData.count();}
	unsigned int smooth() const {return mSmooth;}
	qreal time(int index) {return (mTime && (index < mTime->count()))? mTime->at(index) : qSNaN();}

	QLineSeries *lineSeries();
	QString toString();

	bool saveToStream(QDataStream &stream) const;
	bool loadFromStream(QDataStream &stream);

public slots:
	void setTime(QList<qreal> *time)          { mTime = time;}
	void setName(const QString name)          { mName = name;}
	void setUnit(const QString unit)          { mUnit = unit;}
	void setFactor(const qreal factor)        { mFactor = factor;}
	void setScale(const qreal scale)          { mScale = scale;}
	void setSelected(const bool selected)     { mSelected = selected;}
	void setColor(const QColor color)         { mColor = color;}
	void setSmooth(const unsigned int smooth) { if (smooth > 0) mSmooth = smooth;}

	void clear();
	void invert();
	void calculateLimits();
	QList<qreal> *smoothed();

private:
	QString mName;
	QString mUnit;
	QColor mColor;
	double mFactor;         // ADC factor
	double mScale;          // Scale for plot
	unsigned int mSmooth;
	bool mSelected;
	QList<qreal> mData;
	QList<qreal> *mTime;
	qreal mMinY;
	qreal mMaxY;
};
