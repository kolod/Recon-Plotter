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

#include "utils.h"

#include <cmath>
#include <array>

#include <QStringList>

double prettyFloor(double value, int places) {
	double f = std::pow(10, std::round(std::log10(std::fabs(value))) - places + 1);
	double result = std::floor(value / f) * f;
	return result;
}

double prettyCeil(double value, int places) {
	double f = std::pow(10, std::round(std::log10(std::fabs(value))) - places + 1);
	double result = std::ceil(value / f) * f;
	return result;
}

bool str2bool(const QString str, bool byDefault) {
	const QStringList trueList  = {"true" , "yes", "y", "1"};
	const QStringList falseList = {"false", "no" , "n", "0"};
	if ( trueList.contains(str, Qt::CaseSensitivity::CaseInsensitive)) return true;
	if (falseList.contains(str, Qt::CaseSensitivity::CaseInsensitive)) return false;
	return byDefault;
}

qreal str2qreal(const QString str, qreal byDefault) {
	bool ok;
	qreal result = str.toDouble(&ok);
	if (ok) return result;
	return byDefault;
}

int str2int(const QString str, int byDefault) {
	bool ok;
	int result = str.toInt(&ok);
	if (ok) return result;
	return byDefault;
}

unsigned int str2uint(const QString str, unsigned int byDefault) {
	bool ok;
	unsigned int result = str.toUInt(&ok);
	if (ok) return result;
	return byDefault;
}
