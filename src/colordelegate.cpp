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

#include "colordelegate.h"

ColorDelegate::ColorDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{}

QWidget *ColorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return new QColorDialog(parent);
}

void ColorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QPointer<QColorDialog> dialog = qobject_cast<QColorDialog*>(editor);
	if (dialog) {
		auto color = index.data().toString();
		dialog->setCurrentColor(QColor(color));
	} else {
		QStyledItemDelegate::setEditorData(editor, index);
	}
}

void ColorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
	QPointer<QColorDialog> dialog = qobject_cast<QColorDialog*>(editor);
	if (dialog) {
		auto color = dialog->currentColor().name();
		model->setData(index, color);
	} else {
		QStyledItemDelegate::setModelData(editor, model, index);
	}
}
