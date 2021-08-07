#include "test.h"

test::test(QObject *parent)
  : QAbstractTableModel(parent)
{
}

QVariant test::headerData(int section, Qt::Orientation orientation, int role) const
{
  // FIXME: Implement me!
}

bool test::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  if (value != headerData(section, orientation, role)) {
	// FIXME: Implement me!
	emit headerDataChanged(orientation, section, section);
	return true;
  }
  return false;
}


int test::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid())
	return 0;

  // FIXME: Implement me!
}

int test::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid())
	return 0;

  // FIXME: Implement me!
}

QVariant test::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
	return QVariant();

  // FIXME: Implement me!
  return QVariant();
}

bool test::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (data(index, role) != value) {
	// FIXME: Implement me!
	emit dataChanged(index, index, QVector<int>() << role);
	return true;
  }
  return false;
}

Qt::ItemFlags test::flags(const QModelIndex &index) const
{
  if (!index.isValid())
	return Qt::NoItemFlags;

  return Qt::ItemIsEditable; // FIXME: Implement me!
}
