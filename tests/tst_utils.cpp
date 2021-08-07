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
#include "../src/utils.h"

class testUtils : public QObject
{
	Q_OBJECT

public:
	testUtils() {;}

private slots:
	void test_str2bool() {
		QVERIFY(str2bool("N")     == false);
		QVERIFY(str2bool("n")     == false);
		QVERIFY(str2bool("0")     == false);
		QVERIFY(str2bool("No")    == false);
		QVERIFY(str2bool("NO")    == false);
		QVERIFY(str2bool("false") == false);
		QVERIFY(str2bool("False") == false);
		QVERIFY(str2bool("FALSE") == false);

		QVERIFY(str2bool("Y")     == true);
		QVERIFY(str2bool("y")     == true);
		QVERIFY(str2bool("1")     == true);
		QVERIFY(str2bool("Yes")   == true);
		QVERIFY(str2bool("YES")   == true);
		QVERIFY(str2bool("true")  == true);
		QVERIFY(str2bool("True")  == true);
		QVERIFY(str2bool("TRUE")  == true);

		QVERIFY(str2bool("TRrUE")        == false);
		QVERIFY(str2bool("TRrUE", false) == false);
		QVERIFY(str2bool("TRrUE", true)  == true);
	}

	void test_str2int() {
		QVERIFY(str2int("999")         == 999);
		QVERIFY(str2int("-999")        == -999);
		QVERIFY(str2int("99.9", 111)   == 111);
	}

	void test_str2uint() {
		QVERIFY(str2uint("999")         == 999);
		QVERIFY(str2uint("-999")        == 0);
		QVERIFY(str2uint("99.9", 111)   == 111);
	}

	void test_str2qreal() {
		QVERIFY(str2qreal("999")         == 999.0);
		QVERIFY(str2qreal("-999")        == -999.0);
		QVERIFY(str2qreal("99.9", 111)   == 99.9);
	}
};

QTEST_APPLESS_MAIN(testUtils)

#include "tst_utils.moc"
