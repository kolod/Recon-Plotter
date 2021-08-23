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
#include <QElapsedTimer>
#include <QSharedPointer>
#include <QCoreApplication>
#include "utils.h"
#include "recontextfile.h"

ReconTextFile::ReconTextFile(QObject *parent)
	: DataFile(parent)
{}

bool ReconTextFile::importFile(QString filename)
{
	mCansel = false;

	// Clear data
	foreach (auto item, mAnalogSignals) if (item != nullptr) delete item;
	mAnalogSignals.clear();
	mTime.clear();

	// Open file
	QFile datafile(filename);

	//TODO: Fix conversion from cp1251

	if (datafile.open(QIODevice::ReadOnly)) {

		// Send progress information
		emit updateProgressRange(0, datafile.size());
		emit updateProgressShow(true);
		emit updateProgressValue(0);
		qApp->processEvents();

		// Read header
		QString line = QString::fromLocal8Bit(datafile.readLine());
		if (!line.isEmpty()) {
			auto data = readCommaSeparatedLine(line);
			if (data.count() >= 1) mTitle            = data[0];
			if (data.count() >= 2) mDevice           = data[1];
			if (data.count() >= 3) mOriginalFileName = data[2];
			if (data.count() >= 4) mLabelX           = data[3];
			if (data.count() >= 5) mLabelY           = data[4];
			if (data.count() >= 6) mLeft             = str2qreal(data[5]);
			if (data.count() >= 7) mRight            = str2qreal(data[6]);
			if (data.count() >= 8) mBottom           = str2qreal(data[7]);
			if (data.count() >= 9) mTop              = str2qreal(data[8]);
		}

		// Skip empty line
		datafile.readLine();

		// Read channels
		for (;;) {
			line = QString::fromLocal8Bit(datafile.readLine()).simplified();
			if (line.isEmpty()) break;

			auto data = readCommaSeparatedLine(line);
			if (data.count() >= 1) {
				if (data[0] == '1') {
					line = QString::fromLocal8Bit(datafile.readLine().simplified());
					continue;
				} else if (data[0] == 'N') {
					break;
				}
			}

			auto analogSignal = new AnalogSignal(this);
			analogSignal->setTime(&mTime);
			mAnalogSignals.append(analogSignal);

			analogSignal->setScale(1.0);
			analogSignal->setFactor(1.0);

			if (data.count() >= 3)
				analogSignal->setName(data[2]);
			if ((data.count() >= 4) && (!data[3].isEmpty()))
				analogSignal->setSelected(str2bool(data[3]));
			if (data.count() >= 5)
				analogSignal->setFactor(str2qreal(data[4]));
			if (data.count() >= 6)
				analogSignal->setSmooth(str2int(data[5]));
		}

		// Skip lines
		datafile.readLine();
		datafile.readLine();

		// Update progress
		emit updateProgressValue(static_cast<int>(datafile.pos()));
		qApp->processEvents();
		QElapsedTimer timer;
		timer.start();

		// Read data
		for (;;) {
			// Cansel
			if (mCansel) break;

			line = QString::fromLocal8Bit(datafile.readLine()).simplified();
			QStringList values = line.split(',');
			if (values.count() != (mAnalogSignals.count() + 3)) break;  //TODO: Fix for discrete signals

			// Get units
			if (values[0].simplified().isEmpty())
				for (int i = 0; i < mAnalogSignals.count(); i++)
					mAnalogSignals[i]->setUnit(values[i+2].simplified());

			// Get data
			else if (values.count() >= 3) {
				mTime.append(str2qreal(values[1].simplified()));
				for (int i = 0; i < mAnalogSignals.count(); i++)
					mAnalogSignals[i]->data()->append(str2qreal(values[i+2].simplified()));
			}

			// Update progress avery 1000 milliseconds
			if (timer.hasExpired(100)) {
				emit updateProgressValue(datafile.pos());
				qApp->processEvents();
				timer.restart();
			}
		}

		if (!mCansel) {
			datafile.close();

			calculateLimits();
			resetWindow();

            mFileName = filename;
            setModified(false);

            emit updateProgressShow(false);
			emit dataLoaded();
			return true;
		}
	}

	return false;
}

QStringList ReconTextFile::readCommaSeparatedLine(QString line)
{
	QStringList result;
	QString value = "";
	bool isStringStarted = false;

	for (int i = 0; i < line.count(); i++) {
		if (line.at(i) == '"') {
			if (isStringStarted) {
				if ((i > 1) && (line.at(i-1) == '"')) value += '"';
			} else {
				isStringStarted = true;
			}
		} else if (line.at(i) == ',') {
			if (((i > 2) && (line.at(i-1) == '"') && (line.at(i-2) == '"')) ||
				(isStringStarted && (line.at(i-1) != '"'))
			) {
				value += line.at(i);
			} else {
				result.append(value.simplified());
				isStringStarted = false;
				value = "";
			}
		} else {
			value += line.at(i);
		}
	}

	if (value.count()) result.append(value.simplified());

	return result;
}
