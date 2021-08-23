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

#include <QObject>
#include <QLineEdit>
#include <QDoubleValidator>

class DoubleValidator : public QDoubleValidator
{
	Q_OBJECT

public:
	explicit DoubleValidator(QObject *parent = nullptr);
	QValidator::State validate(QString &str, int &pos) const override;
};


class DoubleLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	DoubleLineEdit(QWidget *parent = nullptr);
    double value() const;

public slots:
	void setDecimals(int decimals) {mValidator.setDecimals(decimals);}
	void setRange(qreal bottom, qreal top) {mValidator.setBottom(bottom), mValidator.setTop(top);}

signals:
	void valueChanged(qreal value);

private:
	DoubleValidator mValidator;
};
