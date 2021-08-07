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

#include "signalsmodel.h"
#include "analogsignal.h"

enum class SignalsModelColumn {
	Selected,
	Name,
	Unit,
	Inverted,
	Smooth,
	Scale,
	Minimum,
	Maximum,
	Color
};

SignalsModel::SignalsModel(QObject *parent)
	: QAbstractTableModel(parent)
	, mDataFile(nullptr)
{}

QVariant SignalsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal) {
			switch (static_cast<SignalsModelColumn>(section)) {
				case SignalsModelColumn::Selected: return tr("Selected");
				case SignalsModelColumn::Name:     return tr("Signal Name");
				case SignalsModelColumn::Unit:     return tr("Units");
				case SignalsModelColumn::Inverted: return tr("Inverted");
				case SignalsModelColumn::Smooth:   return tr("Smooth");
				case SignalsModelColumn::Scale:    return tr("Scale");
				case SignalsModelColumn::Minimum:  return tr("Minimum");
				case SignalsModelColumn::Maximum:  return tr("Maximum");
				case SignalsModelColumn::Color:    return tr("Color");
			}
		}
	}

	return SignalsModel::QAbstractTableModel::headerData(section, orientation, role);
}

QHeaderView::ResizeMode SignalsModel::columnResizeMode(const int section) const
{
	switch (static_cast<SignalsModelColumn>(section)) {
		case SignalsModelColumn::Selected: return QHeaderView::ResizeToContents;
		case SignalsModelColumn::Name:     return QHeaderView::Stretch;
		case SignalsModelColumn::Unit:     return QHeaderView::ResizeToContents;
		case SignalsModelColumn::Inverted: return QHeaderView::ResizeToContents;
		case SignalsModelColumn::Smooth:   return QHeaderView::ResizeToContents;
		case SignalsModelColumn::Scale:    return QHeaderView::ResizeToContents;
		case SignalsModelColumn::Minimum:  return QHeaderView::ResizeToContents;
		case SignalsModelColumn::Maximum:  return QHeaderView::ResizeToContents;
		case SignalsModelColumn::Color:    return QHeaderView::ResizeToContents;
	}

	return QHeaderView::Fixed;
}

int SignalsModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid()) return 0;
	if (mDataFile == nullptr) return 0;
	return mDataFile->analogSignalsCount();
}

int SignalsModel::columnCount(const QModelIndex &parent) const
{
	return parent.isValid() ? 0 : 9;
}

QVariant SignalsModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) return QVariant();
	if (mDataFile == nullptr) return QVariant();
	if (index.row()>= mDataFile->analogSignalsCount()) return QVariant();

	auto signal = mDataFile->analogSignal(index.row());

	if ((role == Qt::DisplayRole) || (role == Qt::EditRole)) {
		switch (static_cast<SignalsModelColumn>(index.column())) {
			case SignalsModelColumn::Selected: return signal->selected();
			case SignalsModelColumn::Name:     return signal->name();
			case SignalsModelColumn::Unit:     return signal->unit();
			case SignalsModelColumn::Inverted: return signal->inverted();
			case SignalsModelColumn::Scale:    return signal->scale();
			case SignalsModelColumn::Smooth:   return signal->smooth();
			case SignalsModelColumn::Minimum:  return signal->bottom();
			case SignalsModelColumn::Maximum:  return signal->top();
			case SignalsModelColumn::Color:    return signal->color();
		}
	}

	return QVariant();
}

bool SignalsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (data(index, role) != value) {
		auto signal = mDataFile->analogSignal(index.row());
		if ((role == Qt::DisplayRole) || (role == Qt::EditRole)) {
			switch (static_cast<SignalsModelColumn>(index.column())) {
				case SignalsModelColumn::Selected: signal->setSelected(value.toBool());         break;
				case SignalsModelColumn::Name:     signal->setName(value.toString());           break;
				case SignalsModelColumn::Unit:     signal->setUnit(value.toString());           break;
				case SignalsModelColumn::Inverted: signal->setInverted(value.toBool());         break;
				case SignalsModelColumn::Scale:    signal->setScale(value.toDouble());          break;
				case SignalsModelColumn::Smooth:   signal->setSmooth(value.toDouble());         break;
				case SignalsModelColumn::Color:    signal->setColor(QColor(value.toString()));  break;
				case SignalsModelColumn::Minimum:                                               break;
				case SignalsModelColumn::Maximum:                                               break;
			}
		}

		mDataFile->setModified();
		emit dataChanged(index, index, QVector<int>() << role);
		return true;
	}
	return false;
}

Qt::ItemFlags SignalsModel::flags(const QModelIndex &index) const
{
	if (!index.isValid()) return Qt::NoItemFlags;

	switch (static_cast<SignalsModelColumn>(index.column())) {
		case SignalsModelColumn::Selected:
		case SignalsModelColumn::Name:
		case SignalsModelColumn::Unit:
		case SignalsModelColumn::Inverted:
		case SignalsModelColumn::Scale:
		case SignalsModelColumn::Smooth:
		case SignalsModelColumn::Color:
			return Qt::ItemIsEditable | Qt::ItemIsEnabled;
		case SignalsModelColumn::Minimum:;
		case SignalsModelColumn::Maximum:;
			return Qt::ItemIsEnabled;
	}

	return Qt::ItemIsEditable;
}

void SignalsModel::setDataFile(DataFile *datafile) {
	beginResetModel();
	mDataFile = datafile;
	endResetModel();
}
