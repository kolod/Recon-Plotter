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

#include <QLocale>
#include "doublelineedit.h"
#include "utils.h"

DoubleValidator::DoubleValidator(QObject *parent)
	: QDoubleValidator(parent)
{}

DoubleValidator::State DoubleValidator::validate(QString &str, int &pos) const
{
	str.replace(QChar('.'), QLocale().decimalPoint());
	str.replace(QChar(','), QLocale().decimalPoint());
	return QDoubleValidator::validate(str, pos);
}

DoubleLineEdit::DoubleLineEdit(QWidget *parent)
	: QLineEdit(parent)
{
	setAlignment(Qt::AlignCenter);
	setValidator(&mValidator);

	connect(this, &DoubleLineEdit::textChanged, this, [this](QString t) {
		qreal value = str2qreal(t.replace(QLocale().decimalPoint(), QChar('.')));
		emit valueChanged(value);
	});
}

double DoubleLineEdit::value() const
{
	auto t = text().replace(QLocale().decimalPoint(), QChar('.'));
	return str2qreal(t, 0.0);
}
