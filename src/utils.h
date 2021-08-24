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
#include <QRect>
#include <QRectF>
#include <QColor>
#include <QPalette>
#include <QApplication>

double prettyFloor(double value, int places = 2);
double prettyCeil(double value, int places = 2);

bool str2bool(const QString str, bool byDefault = false);
qreal str2qreal(const QString str, qreal byDefault = 0.0);
int str2int(const QString str, int byDefault = 0);
unsigned int str2uint(const QString str, unsigned int byDefault = 0);

QString fixFileSuffix(QString filename, const QString suffix);
void addToRecent(QString filename);

void multyply(QVector<double> &data, const double multiplier);
QString str2key(QString value);

double luminance(const QColor color);
bool isLight(const QColor color);
bool isLighter(const QColor color1, const QColor color2);
bool isLightPalette(const QPalette palette = QApplication::palette());
QPalette::ColorRole prettyTextColorRole(QColor background);
QColor prettyTextColor(const QColor background, const QPalette palette = QApplication::palette());

#ifdef WIN32
void registerFileAsossiation(const QString suffix);
#else
#define registerFileAsossiation(x)
#endif
