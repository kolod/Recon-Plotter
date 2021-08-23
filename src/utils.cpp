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


#include <cmath>
#include <array>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QSettings>
//#include <QApplication>
#include <QRegularExpression>
#include "utils.h"

double prettyFloor(double value, int places) {
    if (qIsNull(value) || !qIsFinite(value)) return value;
	double f = std::pow(10, std::round(std::log10(std::fabs(value))) - places + 1);
	double result = std::floor(value / f) * f;
	return result;
}

double prettyCeil(double value, int places) {
    if (qIsNull(value) || !qIsFinite(value)) return value;
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

QString fixFileSuffix(QString filename, const QString suffix)
{
	QFileInfo fi(filename);
	if (fi.suffix().toLower() != suffix) {
		filename = fi.absoluteDir().dirName() + "/" + fi.completeBaseName() + "." + suffix;
	}

	return filename;
}

void addToRecent(QString filename)
{
	QSettings settings;
	auto recent = settings.value("Recent").toStringList();
	recent.removeAll(filename);
	recent.prepend(filename);
	settings.setValue("Recent", recent);
}

void multyply(QVector<double> &data, const double multiplier)
{
	//TODO: add multythread
	if (multiplier != 1.0) for (qsizetype i = 0; i < data.count(); i++) {
		data[i] = data.at(i) * multiplier;
	}
}

QString str2key(QString value)
{
	return value.toLower().replace(QRegularExpression("\\s+"), "_");
}

#ifdef WIN32
void registerFileAsossiation(const QString suffix)
{
	QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\CLASSES", QSettings::NativeFormat);

	QString path = QDir::toNativeSeparators(qApp->applicationFilePath());
	QString name = QString("%1.%2.v%3").arg(
		str2key(qApp->organizationName()),
		str2key(qApp->applicationName()),
		str2key(qApp->applicationVersion()
	));

	qDebug()
		<< "Register File Asossiation:" << Qt::endl
		<< "  Name = " << name << Qt::endl
		<< "  Path = " << path << Qt::endl;

	settings.setValue("." + suffix + "/.", name);
	settings.setValue("." + suffix + "/DefaultIcon/.", path);
	settings.setValue(name + "/shell/open/command/.", path + " %1");
}
#endif
