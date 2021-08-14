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

#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include "analogsignal.h"
#include "datafile.h"

#define MAGIC        (quint32) 0x504C4F54
#define VERSION      (quint32) 1

DataFile::DataFile(QObject *parent)
	: QObject(parent)
	, mFileName("")
	, mTime()
	, mModified(false)
	, mRenameNeeded(false)
{}

bool DataFile::save()
{
	return saveAs(mFileName);
}

bool DataFile::saveAs(QString filename)
{
	QFile datafile(filename);
	if (!datafile.open(QIODevice::WriteOnly)) {
		qDebug() << "Unable to open: " << filename;
		return false;
	}

	QByteArray data;

	QDataStream datastream(&data, QIODevice::WriteOnly);
	datastream.setFloatingPointPrecision(QDataStream::DoublePrecision);

	datastream
		<< MAGIC
		<< VERSION;

	datastream.setVersion(QDataStream::Qt_5_0);

	datastream
		<< mTitle
		<< mDevice
		<< mOriginalFileName
		<< mLabelX
		<< mLabelY
		<< mLeft
		<< mRight
		<< mBottom
		<< mTop
		<< mMinX
		<< mMaxX
		<< mMinY
		<< mMaxY
		<< mTime.count();

	for (qreal value : qAsConst(mTime)) {
		datastream << value;
	}

	datastream << mAnalogSignals.count();
	foreach (auto *signal, mAnalogSignals) {
		signal->saveToStream(datastream);
	}

	if (datafile.write(qCompress(data)) > 0) {
		mFileName = filename;
		mRenameNeeded = mModified = false;
		emit modifiedChanged(false);
		return true;
	}

	return false;
}

bool DataFile::open(QString filename)
{
	QFile datafile(filename);
	if (!datafile.open(QIODevice::ReadOnly)) {
		qDebug() << "Unable to open: " << filename;
		return false;
	}

	QByteArray data = qUncompress(datafile.readAll());
	QDataStream datastream(&data, QIODevice::ReadOnly);
	datastream.setFloatingPointPrecision(QDataStream::DoublePrecision);

	quint32 magic;
	quint32 version;
	int count;
	qreal value;

	datastream >> magic;
	qDebug() << "Magic value: 0x" << Qt::hex << magic;
	if (magic != MAGIC) return false;

	datastream >> version;
	qDebug() << "Version: " << version;
	if (version != VERSION) return false;

	datastream.setVersion(QDataStream::Qt_5_0);

	datastream
		>> mTitle
		>> mDevice
		>> mOriginalFileName
		>> mLabelX
		>> mLabelY
		>> mLeft
		>> mRight
		>> mBottom
		>> mTop
		>> mMinX
		>> mMaxX
		>> mMinY
		>> mMaxY
		>> count;

	mTime.clear();
	mTime.reserve(count);
	for (int i = count; i; i--) {
		datastream >> value;
		mTime.append(value);
	}

	foreach (auto signal, mAnalogSignals) {
		if (signal != nullptr) signal->deleteLater();
		mAnalogSignals.removeOne(signal);
	}

	datastream >> count;
	for (int i = count; i; i--) {
		AnalogSignal *signal = new AnalogSignal();
		signal->loadFromStream(datastream);
		signal->setTime(&mTime);
		mAnalogSignals.append(signal);
	}

	mFileName = filename;
	mModified = false;
	emit modifiedChanged(false);

	return true;
}
