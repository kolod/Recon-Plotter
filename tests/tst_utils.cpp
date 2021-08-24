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
#include <QColor>
#include "../src/utils.h"

class testUtils : public QObject
{
	Q_OBJECT

public:
	explicit testUtils(QObject *parent=nullptr) : QObject(parent) { ; }

private slots:
	void test_str2bool()
	{
		QCOMPARE(str2bool("N"), false);
		QCOMPARE(str2bool("n"), false);
		QCOMPARE(str2bool("0"), false);
		QCOMPARE(str2bool("No"), false);
		QCOMPARE(str2bool("NO"), false);
		QCOMPARE(str2bool("false"), false);
		QCOMPARE(str2bool("False"), false);
		QCOMPARE(str2bool("FALSE"), false);

		QCOMPARE(str2bool("Y"), true);
		QCOMPARE(str2bool("y"), true);
		QCOMPARE(str2bool("1"), true);
		QCOMPARE(str2bool("Yes"), true);
		QCOMPARE(str2bool("YES"), true);
		QCOMPARE(str2bool("true"), true);
		QCOMPARE(str2bool("True"), true);
		QCOMPARE(str2bool("TRUE"), true);

		QCOMPARE(str2bool("TRrUE"), false);
		QCOMPARE(str2bool("TRrUE", false), false);
		QCOMPARE(str2bool("TRrUE", true), true);
	}

	void test_str2int()
	{
		QCOMPARE(str2int("999"), 999);
		QCOMPARE(str2int("-999"), -999);
		QCOMPARE(str2int("99.9", 111), 111);
	}

	void test_str2uint()
	{
		QCOMPARE(str2uint("999"), 999);
		QCOMPARE(str2uint("-999"), 0);
		QCOMPARE(str2uint("99.9", 111), 111);
	}

	void test_str2qreal()
	{
		QCOMPARE(str2qreal("999"), 999.0);
		QCOMPARE(str2qreal("-999"), -999.0);
		QCOMPARE(str2qreal("99.9", 111), 99.9);
	}

	void test_prettyFloor(){
		QCOMPARE(prettyFloor(qInf()), qInf());
		QCOMPARE(prettyFloor(-qInf()), -qInf());
		QCOMPARE(prettyFloor(0.000), 0.000);

		QCOMPARE(prettyFloor(1.000), 1.000);
		QCOMPARE(prettyFloor(1.010), 1.000);
		QCOMPARE(prettyFloor(1.011), 1.000);
		QCOMPARE(prettyFloor(2.099), 2.000);
		QCOMPARE(prettyFloor(2.199), 2.100);

		QCOMPARE(prettyFloor(1.000, 1), 1.000);
		QCOMPARE(prettyFloor(1.010, 1), 1.000);
		QCOMPARE(prettyFloor(1.011, 1), 1.000);
		QCOMPARE(prettyFloor(2.099, 1), 2.000);
		QCOMPARE(prettyFloor(2.199, 1), 2.000);

		QCOMPARE(prettyFloor(100.0, 1), 100.0);
		QCOMPARE(prettyFloor(101.0, 1), 100.0);
		QCOMPARE(prettyFloor(101.1, 1), 100.0);
		QCOMPARE(prettyFloor(209.9, 1), 200.0);
		QCOMPARE(prettyFloor(219.9, 1), 200.0);

		QCOMPARE(prettyFloor(1.000, 3), 1.000);
		QCOMPARE(prettyFloor(1.010, 3), 1.010);
		QCOMPARE(prettyFloor(1.011, 3), 1.010);
		QCOMPARE(prettyFloor(2.099, 3), 2.090);
		QCOMPARE(prettyFloor(2.199, 3), 2.190);

		QCOMPARE(prettyFloor(10.00, 3), 10.00);
		QCOMPARE(prettyFloor(10.30, 3), 10.30);
		QCOMPARE(prettyFloor(10.11, 3), 10.10);
		QCOMPARE(prettyFloor(20.99, 3), 20.90);
		QCOMPARE(prettyFloor(21.99, 3), 21.90);
	}

	void test_prettyCeil(){
		QCOMPARE(prettyCeil(qInf()), qInf());
		QCOMPARE(prettyCeil(-qInf()), -qInf());
		QCOMPARE(prettyCeil(0.000), 0.000);
	}
};

QTEST_APPLESS_MAIN(testUtils)

#include "tst_utils.moc"
