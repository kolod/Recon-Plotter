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
#include "utils.h"
#include "analogsignal.h"

AnalogSignal::AnalogSignal(QObject *parent)
	: QObject(parent), mName(""), mUnit(""), mFactor(1.0), mScale(1.0), mSmooth(1), mSelected(false), mTime(nullptr)
{
}

QString AnalogSignal::name(const bool legend) const
{
	if (legend) {
		if (mScale == 1.0) {
			return QString("%1, %2").arg(mName, mUnit);
		} else {
			return QString("%1, %2 × %3").arg(mName, mUnit).arg(mScale);
		}
	} else {
		return mName;
	}
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

void AnalogSignal::calculateLimits() {
	mMinY = qInf();
	mMaxY = -qInf();

	foreach (qreal val, mData) {
		mMinY = qMin(mMinY, val);
		mMaxY = qMax(mMaxY, val);
	}
}

QVector<double> &AnalogSignal::smoothed() {
	double multiplier = mFactor * mScale;

	if ((mDataCache.count() != mData.count()) ||
		(multiplier != mMutyplierCache) ||
		(mSmooth != mSmoothCache)
	){
		mDataCache.clear();
		mDataCache.reserve(mData.count());

		if (mSmooth > 1) {
			qreal value = 0.0;
			QQueue<qreal> buffer;

			for (qsizetype i = 0; i < qMin(mData.count(), mTime->count()); i++) {
				value += mData.at(i);
				buffer.enqueue(mData.at(i));
				if (buffer.count() > static_cast<int>(mSmooth)) value -= buffer.dequeue();
				mDataCache.append(value / buffer.count());
			}
		} else {
			mDataCache = mData;
		}

		multyply(mDataCache, multiplier);
	}

	return mDataCache;
}

QString AnalogSignal::toString()
{
	return QString("Name:\t%1\nUnit:\t%2\nScale:\t%3\nSmoth:\t%4").arg(mName).arg(mUnit, mScale).arg(mSmooth);
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
		<< static_cast<quint64>(mData.count());

	for (qreal value : qAsConst(mData)) stream << value;
	return true;
}

bool AnalogSignal::loadFromStream(QDataStream &stream)
{
	QString colorname;
	qreal value;
	quint64 count;

	stream >> mName >> mUnit >> mSelected >> mFactor >> mScale >> mSmooth >> mMinY >> mMaxY >> colorname >> count;

	mColor = QColor(colorname);

	mData.clear();
	mData.reserve(count);
	for (int i = count; i; i--) {
		stream >> value;
		mData.append(value);
	}

	return true;
}
