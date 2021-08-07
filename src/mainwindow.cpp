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

#include <QDebug>
#include <QtGlobal>
#include <QSettings>
#include <QFileDialog>
#include <QElapsedTimer>
#include <QStandardPaths>
#include <QMdiSubWindow>
#include <QAction>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	mProgress = new QProgressBar(ui->statusbar);
	ui->statusbar->addPermanentWidget(mProgress, 1);

	connect(ui->menuWindow, &QMenu::aboutToShow, this, &MainWindow::updateWindowMenu);

	mSignalsModel = new SignalsModel();
	ui->tableSignals->setModel(mSignalsModel);
	ui->tableSignals->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	for (int i = 0; i < ui->tableSignals->horizontalHeader()->count(); i++) {
		ui->tableSignals->horizontalHeader()->setSectionResizeMode(i, mSignalsModel->columnResizeMode(i));
	}

	connect(ui->mdiArea, &QMdiArea::subWindowActivated, this, [this](QMdiSubWindow *window) {
		auto activeChartWindow = qobject_cast<ChartWindow*>(window);
		if (activeChartWindow != nullptr) {
			mSignalsModel->setDataFile(activeChartWindow->dataFile());
		} else {
			mSignalsModel->setDataFile(nullptr);
		}
	});

	connect(ui->actionImport, &QAction::triggered, this, &MainWindow::actionImport);
	connect(ui->actionAboutQt, &QAction::triggered, this, [](){qApp->aboutQt();});
}

MainWindow::~MainWindow()
{
	delete ui;
	mSignalsModel->deleteLater();
}

void MainWindow::saveSession()
{
	QSettings settings;

	settings.setValue("geometry"     , saveGeometry());
	settings.setValue("state"        , saveState());
}

void MainWindow::restoreSession()
{
	QSettings settings;

	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("state").toByteArray());

}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (ui->mdiArea->currentSubWindow() != nullptr) {
		ui->mdiArea->closeAllSubWindows();
	}
	if (ui->mdiArea->currentSubWindow() == nullptr) {
		saveSession();
		event->accept();
	} else {
		event->ignore();
	}
}

bool MainWindow::importReconTextFile(QString filename) {

	DataFile *datafile = new ReconTextFile(filename, this);
	connect(datafile, &DataFile::updateProgressShow , mProgress, &QProgressBar::setVisible);
	connect(datafile, &DataFile::updateProgressRange, mProgress, &QProgressBar::setRange);
	connect(datafile, &DataFile::updateProgressValue, mProgress, &QProgressBar::setValue);

	if (datafile->read()) {
		ChartWindow *newChartWindow = new ChartWindow(this);
		newChartWindow->setDataFile(datafile);
		ui->mdiArea->addSubWindow(newChartWindow);
		newChartWindow->showMaximized();
		updateWindowMenu();
		return true;
	}

	return false;
}

void MainWindow::actionImport()
{
	QSettings settings;
	QFileDialog *dialog = new QFileDialog(this);

	dialog->setNameFilter("Recon Data Text File (*.txt)");
	dialog->setFileMode(QFileDialog::ExistingFiles);
	dialog->setAcceptMode(QFileDialog::AcceptOpen);
	dialog->setLabelText(QFileDialog::Accept, tr("Import"));
	dialog->setWindowTitle(tr("Import Recon Data File"));
	dialog->setDirectory(settings.value(
		"LastImportDir",
		QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
	).toString());

	connect(dialog, &QFileDialog::accepted, this, [this, dialog]() {
		bool ok = false;
		foreach (QString filename, dialog->selectedFiles()) {
			ok |= importReconTextFile(filename);
		}
		if (ok) {
			QSettings settings;
			settings.setValue("LastImportDir", dialog->directory().path());
		}
	});

	dialog->open();
}

ChartWindow *MainWindow::activeMdiChild() const {
	return qobject_cast<ChartWindow *>(ui->mdiArea->activeSubWindow());
}

void MainWindow::updateWindowMenu() {

	// Clear items
	auto actions = ui->menuWindow->actions();
	for (qsizetype i =0; i < actions.count(); i++) {
		QAction *action = actions[i];
		if (action->isCheckable()) {
			ui->menuWindow->removeAction(action);
			action->deleteLater();
		}
	}

	// Add items
	QList<QMdiSubWindow *> windows = ui->mdiArea->subWindowList();
	for (qsizetype i =0; i < windows.count(); i++) {
		auto *mdiSubWindow = qobject_cast<ChartWindow*>(windows.at(i));
		auto text = QString((i<=9)?"&%1 %2":"%1 %2").arg(i+1).arg(mdiSubWindow->userFriendlyCurrentFile());
		QAction *action = ui->menuWindow->addAction(text, mdiSubWindow, [this, mdiSubWindow](){
			ui->mdiArea->setActiveSubWindow(mdiSubWindow);
		});
		action->setCheckable(true);
		action->setChecked(mdiSubWindow == activeMdiChild());
	}
}
