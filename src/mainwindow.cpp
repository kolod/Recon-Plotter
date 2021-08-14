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
#include <QPointer>
#include <QSettings>
#include <QFileDialog>
#include <QElapsedTimer>
#include <QStandardPaths>
#include <QMdiSubWindow>
#include <QAction>
#include "doublelineedit.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "utils.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	mProgress = new QProgressBar(ui->statusbar);
	ui->statusbar->addPermanentWidget(mProgress, 1);

	connect(ui->menuWindow, &QMenu::aboutToShow, this, &MainWindow::updateWindowMenu);
	connect(ui->menuPlot, &QMenu::aboutToShow, this, &MainWindow::updatePlotMenu);
	connect(ui->menuFile, &QMenu::aboutToShow, this, &MainWindow::updateFileMenu);

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

	connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::actionOpen);
	connect(ui->actionSave, &QAction::triggered, this, &MainWindow::actionSave);
	connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::actionSaveAs);
	connect(ui->actionImport, &QAction::triggered, this, &MainWindow::actionImport);
	connect(ui->actionRefresh, &QAction::triggered, this, &MainWindow::actionRefresh);
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

	auto files = settings.value("Opened Files").toStringList();
	foreach (auto filename, files) {
		openFile(filename);
	}
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	QSettings settings;
	QStringList files;

	if (ui->mdiArea->currentSubWindow() != nullptr) {
		foreach (auto *child, ui->mdiArea->subWindowList()) {
			QPointer<ChartWindow> chart = qobject_cast<ChartWindow*>(child);
			QPointer<DataFile> datafile = chart ? chart->dataFile() : nullptr;
			if (datafile) files.append(datafile->fileName());
			if (chart) chart->close();
		}
	}

	if (ui->mdiArea->currentSubWindow() == nullptr) {
		saveSession();
		event->accept();
	} else {
		event->ignore();
	}

	settings.setValue("Opened Files", files);
}

void MainWindow::actionOpen()
{
	QSettings settings;
	QFileDialog *dialog = new QFileDialog(this);

	dialog->setNameFilter("Plot Data File (*.plot)");
	dialog->setFileMode(QFileDialog::ExistingFiles);
	dialog->setAcceptMode(QFileDialog::AcceptOpen);
	dialog->setWindowTitle(tr("Import Recon Data File"));
	dialog->setDirectory(settings.value(
		"LastDir", QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
	).toString());

	connect(dialog, &QFileDialog::accepted, this, [this, dialog]() {
		bool ok = false;
		foreach (auto filename, dialog->selectedFiles()) {
			if (openFile(filename)) {
				addToRecent(filename);
				ok = true;
			}
		}
		if (ok) QSettings().setValue("LastDir", dialog->directory().path());
	});

	dialog->open();
}

void MainWindow::actionSave()
{
	auto *child = activeMdiChild();
	if (child == nullptr) return;

	auto *datafile = child->dataFile();
	if (datafile == nullptr) return;

	if (datafile->isRenameNeeded()) {
		actionSaveAs();
	} else {
		if (datafile->save()) {
			addToRecent(datafile->fileName());
		}
	}
}

void MainWindow::actionSaveAs()
{
	auto *child = activeMdiChild();
	if (child == nullptr) return;

	auto *datafile = child->dataFile();
	if (datafile == nullptr) return;

	QFileDialog *dialog = new QFileDialog(this);

	dialog->setNameFilter("Plot Data File (*.plot)");
	dialog->setAcceptMode(QFileDialog::AcceptSave);
	dialog->setWindowTitle(tr("Save Plot Data File"));
	dialog->selectFile(fixFileSuffix(datafile->fileName(), "plot"));

	connect(dialog, &QFileDialog::accepted, this, [this, datafile, dialog]() {
		auto files = dialog->selectedFiles();
		if (!files.isEmpty())
			if (datafile->saveAs(files.first()))
				addToRecent(files.first());
	});

	dialog->open();
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
		"LastImportDir", QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
	).toString());

	connect(dialog, &QFileDialog::accepted, this, [this, dialog]() {
		bool ok = false;
		foreach (auto filename, dialog->selectedFiles()) {
			ok |= importReconTextFile(filename);
		}
		if (ok) {
			QSettings settings;
			settings.setValue("LastImportDir", dialog->directory().path());
		}
	});

	dialog->open();
}

void MainWindow::actionRefresh()
{
	activeMdiChild()->refresh();
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

	// Disable actions
	bool hasChildrens = !ui->mdiArea->subWindowList().isEmpty();
	bool hasMultypleChildrens = ui->mdiArea->subWindowList().count() > 1;
	bool hasActiveChildren = activeMdiChild() != nullptr;

	ui->actionTile->setEnabled(hasChildrens);
	ui->actionCascade->setEnabled(hasChildrens);
	ui->actionCloseAll->setEnabled(hasChildrens);
	ui->actionClose->setEnabled(hasActiveChildren);
	ui->actionNext->setEnabled(hasMultypleChildrens);
	ui->actionPrevious->setEnabled(hasMultypleChildrens);
}

void MainWindow::updateFileMenu()
{
	// Clear old items
	foreach (auto *action, ui->menuOpenRecent->actions()) {
		if (action != nullptr) {
			ui->menuOpenRecent->removeAction(action);
			action->deleteLater();
		}
	}

	// Add items
	QSettings settings;
	auto recent = settings.value("Recent").toStringList();
	foreach (auto filename, recent) {
		auto *action = new QAction(filename);
		action->setEnabled(QFileInfo(filename).isReadable());
		connect(action, &QAction::triggered, this, [this, filename](){openFile(filename);});
		ui->menuOpenRecent->addAction(action);
	}

	// Disable menu if no recent files found
	ui->menuOpenRecent->setEnabled(ui->menuOpenRecent->actions().count() > 0);

	// Disable save menu if no active file
	QPointer<ChartWindow> child = activeMdiChild();
	QPointer<DataFile> datafile = child ? child->dataFile() : nullptr;
	ui->actionSave->setEnabled(datafile && datafile->isModified());
	ui->actionSaveAs->setEnabled(datafile);
}

void MainWindow::updatePlotMenu()
{
	// Disable save menu if no active file
	auto child = activeMdiChild();
	bool canRefresh = ((child != nullptr) && (child->dataFile() != nullptr));
	ui->actionRefresh->setEnabled(canRefresh);
}

bool MainWindow::openFile(const QString filename) {
	DataFile *datafile = new DataFile(this);
	connect(datafile, &DataFile::updateProgressShow , mProgress, &QProgressBar::setVisible);
	connect(datafile, &DataFile::updateProgressRange, mProgress, &QProgressBar::setRange);
	connect(datafile, &DataFile::updateProgressValue, mProgress, &QProgressBar::setValue);

	if (datafile->open(filename)) {
		ChartWindow *newChartWindow = new ChartWindow(this);
		newChartWindow->setDataFile(datafile);
		ui->mdiArea->addSubWindow(newChartWindow);
		newChartWindow->showMaximized();
		newChartWindow->refresh();
		updateWindowMenu();
		return true;
	}

	return false;
}

bool MainWindow::importReconTextFile(QString filename) {

	auto *datafile = new ReconTextFile(this);
	connect(datafile, &DataFile::updateProgressShow , mProgress, &QProgressBar::setVisible);
	connect(datafile, &DataFile::updateProgressRange, mProgress, &QProgressBar::setRange);
	connect(datafile, &DataFile::updateProgressValue, mProgress, &QProgressBar::setValue);

	if (datafile->importFile(filename)) {
		ChartWindow *newChartWindow = new ChartWindow(this);
		newChartWindow->setDataFile(datafile);
		ui->mdiArea->addSubWindow(newChartWindow);
		newChartWindow->showMaximized();
		newChartWindow->refresh();
		updateWindowMenu();
		return true;
	}

	return false;
}

ChartWindow *MainWindow::activeMdiChild() const {
	return qobject_cast<ChartWindow *>(ui->mdiArea->activeSubWindow());
}

void MainWindow::addToRecent(QString filename)
{
	QSettings settings;
	auto recent = settings.value("Recent").toStringList();
	recent.removeAll(filename);
	recent.prepend(filename);
	settings.setValue("Recent", recent);
}
