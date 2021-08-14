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

#include <QString>

double prettyFloor(double value, int places = 2);
double prettyCeil(double value, int places = 2);

bool str2bool(const QString str, bool byDefault = false);
qreal str2qreal(const QString str, qreal byDefault = 0.0);
int str2int(const QString str, int byDefault = 0);
unsigned int str2uint(const QString str, unsigned int byDefault = 0);

QString fixFileSuffix(QString filename, const QString suffix);
