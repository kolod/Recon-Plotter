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

#include <QDebug>
#include <QQueue>
#include "analogsignal.h"

AnalogSignal::AnalogSignal(QObject *parent)
	: QObject(parent)
	, mName("")
	, mUnit("")
	, mFactor(1.0)
	, mScale(1.0)
	, mSmooth(1)
	, mSelected(false)
	, mTime(nullptr)
{}

QString AnalogSignal::name(bool legend) const
{
	return legend ? QString("%1, %2 × %3").arg(mName, mUnit).arg(mScale)  : mName;
}

void AnalogSignal::clear()
{
	mName.clear();
	mUnit.clear();
	mScale = 1.0;
	mSmooth = 1;
	mSelected = false;
}

void AnalogSignal::invert()
{
	for (qsizetype i = 0; i < mData.count(); i++) mData[i] *= -1.0;
}

void AnalogSignal::calculateLimits()
{
	mMaxY = -qInf();
	mMinY = qInf();

	foreach (qreal val, mData) {
		if (val > mMaxY) mMaxY = val;
		if (val < mMinY) mMinY = val;
	}
}

QList<qreal> *AnalogSignal::smoothed()
{
	if (mSmooth > 1) {

		QList<qreal> *result = new QList<qreal>();
			QQueue<qreal> buffer;
			qreal value = 0.0;

			for (qsizetype i = 0; i < qMin(mData.count(), mTime->count()); i++) {
				value += mData.at(i);
				buffer.enqueue(mData.at(i));
			if (buffer.count() > static_cast<int>(mSmooth)) value -= buffer.dequeue();
			result->append(value / mSmooth);
				}

		return result;
			}

	return &mData;
}

QLineSeries *AnalogSignal::lineSeries() {
	QLineSeries *lineSeries = new QLineSeries();
	lineSeries->setName(mName);
	lineSeries->setColor(QColor(mColor));
	lineSeries->setUseOpenGL(true);

	if (mTime && (mData.count() == mTime->count())) {
		QList<qreal> *data = smoothed();
		for (qsizetype i = 0; i < data->count(); i++) {
			lineSeries->append(mTime->at(i), data->at(i) * mScale);
		}
	}

	return lineSeries;
}

QString AnalogSignal::toString()
{
	return QString("Name:\t%1\nUnit:\t%2\nScale:\t%3\nSmoth:\t%4")
		.arg(mName).arg(mUnit, mScale).arg(mSmooth);
}

bool AnalogSignal::saveToStream(QDataStream &stream) const
{
	stream
		<< mName
		<< mUnit
		<< mSelected
		<< mFactor
		<< mScale
		<< mSmooth
		<< mMinY
		<< mMaxY
		<< mColor.name()
		<< mData.count();

	for (qreal value : qAsConst(mData)) {
		stream << value;
	};

	return true;
}
bool AnalogSignal::loadFromStream(QDataStream &stream)
{
	QString colorname;
	qreal value;
	int count;

	stream
		>> mName
		>> mUnit
		>> mSelected
		>> mFactor
		>> mScale
		>> mSmooth
		>> mMinY
		>> mMaxY
		>> colorname
		>> count;

	mColor = QColor(colorname);

	mData.clear();
	mData.reserve(count);
	for (int i = count; i; i--) {
		stream >> value;
		mData.append(value);
	}

	return true;
}
