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

class AnalogSignal : public QObject
{
	Q_OBJECT

public:
	AnalogSignal(QObject *parent = nullptr);

	QString name(const bool legend = false) const;
	QString toString();
	bool saveToStream(QDataStream &stream) const;
	bool loadFromStream(QDataStream &stream);

	auto unit()      const {return mUnit;}
	auto color()     const {return mColor;}
	auto selected()  const {return mSelected;}
	auto factor()    const {return mFactor;}
	auto scale()     const {return mScale;}
	auto minY()      const {return mMinY * mFactor;}
	auto maxY()      const {return mMaxY * mFactor;}
	auto *data()           {return &mData;}
	auto dataCount() const {return mData.count();}
	auto smooth()    const {return mSmooth;}

public slots:
	void setTime(QVector<double> *time)   { mTime = time;}
	void setName(const QString name)      { mName = name;}
	void setUnit(const QString unit)      { mUnit = unit;}
	void setFactor(const qreal factor)    { mFactor = factor;}
	void setScale(const qreal scale)      { mScale = scale;}
	void setSelected(const bool selected) { mSelected = selected;}
	void setColor(const QColor color)     { mColor = color;}
	void setSmooth(const quint64 smooth)  { if (smooth > 0) mSmooth = smooth;}

	void clear();
	void invert();
	void calculateLimits();
	QVector<double> &smoothed();

private:
	QString mName;
	QString mUnit;
	QColor mColor;
	double mFactor;         // ADC factor
	double mScale;          // Scale for plot
	quint64 mSmooth;
	bool mSelected;
	QVector<double> mData;
	QVector<double> *mTime;
	double mMinY;
	double mMaxY;

	double mMutyplierCache;
	unsigned int mSmoothCache;
	QVector<double> mDataCache;
};
