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

#include <QtTest>
#include <QDebug>
#include <QString>
#include <QStringList>
#include "../src/recontextfile.h"

// add necessary includes here

class testReconTextFile : public QObject
{
	Q_OBJECT

public:
	explicit testReconTextFile(QObject *parent = nullptr)
		: QObject(parent),
		  mReconTextFile(this)
	{
		QVERIFY(mReconTextFile.importFile("test_data.txt"));
	}

private:
	ReconTextFile mReconTextFile;

private slots:
	void test_readCommaSeparatedLine()
	{
		QString testLine = "Ud, N403, 490.WINREC, \"Time, s\", \"Vol\"\"tage, V\", 0, 130, -700, 700";
		QStringList data = mReconTextFile.readCommaSeparatedLine(testLine);

		qDebug() << data.count();
		qDebug() << data;

		QVERIFY(data.count() == 9);
		QVERIFY(data[0] == "Ud");
		QVERIFY(data[1] == "N403");
		QVERIFY(data[2] == "490.WINREC");
		QVERIFY(data[3] == "Time, s");
		QVERIFY(data[4] == "Vol\"tage, V");
		QVERIFY(data[5] == "0");
		QVERIFY(data[6] == "130");
		QVERIFY(data[7] == "-700");
		QVERIFY(data[8] == "700");
	}

	void test_read()
	{
		QVERIFY(mReconTextFile.analogSignalsCount() == 4);
		QVERIFY(mReconTextFile.analogSignal(0)->name() == "Ud (UZ1)");
		QVERIFY(mReconTextFile.analogSignal(0)->unit() == "V");
		QVERIFY(mReconTextFile.analogSignal(0)->scale() == 1.0);
		QVERIFY(mReconTextFile.analogSignal(0)->smooth() == 100);
	}

	void test_read_data()
	{
		QVERIFY(mReconTextFile.analogSignal(0)->data()->count() == 10);
		QVERIFY(mReconTextFile.analogSignal(0)->data()->at(0) == -2.314);
		QVERIFY(mReconTextFile.analogSignal(0)->minY() == -2.314);
		QVERIFY(mReconTextFile.analogSignal(0)->maxY() == 2.314);
	}
};

QTEST_APPLESS_MAIN(testReconTextFile)

#include "tst_testrecontextfile.moc"
