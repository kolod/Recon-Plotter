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
#include <QPainter>
#include <QtGlobal>
#include <QPointer>
#include <QSettings>
#include <QFileDialog>
#include <QPrintDialog>
#include <QProgressDialog>
#include <QPrintPreviewDialog>
#include <QProgressDialog>
#include <QElapsedTimer>
#include <QStandardPaths>
#include <QMdiSubWindow>
#include <QAction>
#include <QLocalSocket>
#include "doublelineedit.h"
#include "mainwindow.h"
#include "utils.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, mSignalsModel(nullptr)
	, mServer(nullptr)
	, mColorDelegate(nullptr)
{
	ui->setupUi(this);

#ifndef WIN32
	setWindowIcon(QIcon(":/icons/appicon.svg"));
#endif

	mColorDelegate = new ColorDelegate(this);
	ui->tableSignals->setItemDelegateForColumn(7, mColorDelegate);

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

	connect(ui->actionOpen,           &QAction::triggered, this, &MainWindow::open);
	connect(ui->actionImport,         &QAction::triggered, this, &MainWindow::import);
	connect(ui->actionFullscreenMode, &QAction::toggled,   this, &MainWindow::fullScreen);
	connect(ui->actionAboutQt,        &QAction::triggered, this, [](){qApp->aboutQt();});

	connect(ui->actionPrint,          &QAction::triggered, this, [this](){
		QPointer<ChartWindow> child = activeMdiChild();
		if (child) child->print();
	});

	connect(ui->actionRefresh,        &QAction::triggered, this, [this](){
		QPointer<ChartWindow> child = activeMdiChild();
		if (child) child->refresh();
	});

	connect(ui->actionSave,           &QAction::triggered, this, [this](){
		QPointer<ChartWindow> child = activeMdiChild();
		if (child) child->save();
	});

	connect(ui->actionSaveAs,         &QAction::triggered, this, [this](){
		QPointer<ChartWindow> child = activeMdiChild();
		if (child) child->saveAs();
	});

	mServer = new QLocalServer(this);
	connect(mServer, &QLocalServer::newConnection, this, [this](){
		for (;;) {
			QPointer<QLocalSocket> localSocket = mServer->nextPendingConnection();
			if (!localSocket) break;

			raise();
			activateWindow();

			connect(localSocket, &QLocalSocket::disconnected, localSocket, &QLocalSocket::deleteLater);
			connect(localSocket, &QLocalSocket::readyRead, this, [this, localSocket](){
				while (localSocket->canReadLine()) {
					auto line = QString::fromUtf8(localSocket->readLine());
					qDebug() << "Read from local connection:" << line;
					openFile(line.simplified());
				}
			});
		}
	});

	mServer->listen(str2key(qApp->applicationName()));

	qDebug() << "Opened local socket:" << mServer->fullServerName();
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

	foreach (auto filename, filesFromSettings("OpenedFiles")) openFile(filename);
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

	settings.setValue("OpenedFiles", files);
}

void MainWindow::open()
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

void MainWindow::import()
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

void MainWindow::fullScreen(bool state)
{
	static QByteArray previousGeometry;
	static QByteArray previousState;
	static bool previousIsMaximized = false;

	if (state) {
		previousIsMaximized = isMaximized();
		previousGeometry = saveGeometry();
		previousState = saveState();
		ui->dockSignals->hide();
		ui->dockPlotSettings->hide();
		ui->statusbar->hide();
		showFullScreen();
	} else {
		previousIsMaximized ? showMaximized() : showNormal();
		restoreState(previousState);
		restoreGeometry(previousGeometry);
	}
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
	foreach (auto filename, filesFromSettings("Recent")) {
		auto *action = new QAction(filename);
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
	ui->actionPrint->setEnabled(datafile);
}

void MainWindow::updatePlotMenu()
{
	// Disable save menu if no active file
	auto child = activeMdiChild();
	bool canRefresh = ((child != nullptr) && (child->dataFile() != nullptr));
	ui->actionRefresh->setEnabled(canRefresh);
}

bool MainWindow::openFile(const QString filename) {
	if (isFileAlreadyOpen(filename)) return false;  //TODO: Add message

	DataFile *datafile = new DataFile(this);

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
	if (isFileAlreadyOpen(filename)) return false;  //TODO: Add message

	auto *datafile = new ReconTextFile(this);

	QProgressDialog dialog;
	dialog.setLabelText(tr("Importing %1").arg(filename));

	connect(&dialog, &QProgressDialog::canceled, datafile, &DataFile::cansel);
	connect(datafile, &DataFile::updateProgressRange, &dialog, &QProgressDialog::setRange);
	connect(datafile, &DataFile::updateProgressValue, &dialog, &QProgressDialog::setValue);
	dialog.open();

	if (datafile->importFile(filename)) {
		ChartWindow *newChartWindow = new ChartWindow(this);
		newChartWindow->setDataFile(datafile);
		ui->mdiArea->addSubWindow(newChartWindow);
		newChartWindow->showMaximized();
		newChartWindow->refresh();
		updateWindowMenu();
		dialog.close();
		return true;
	}

	dialog.close();
	return false;
}

ChartWindow *MainWindow::activeMdiChild() const {
	return qobject_cast<ChartWindow *>(ui->mdiArea->activeSubWindow());
}

bool MainWindow::isFileAlreadyOpen(const QString filename) {
	foreach (auto *child, ui->mdiArea->subWindowList()) {
		QPointer<ChartWindow> chartwindow = qobject_cast<ChartWindow*>(child);
		if (chartwindow != nullptr) {
			QPointer<DataFile> datafile = chartwindow->dataFile();
			if (datafile) {
				if (filename == datafile->fileName()) {
					qDebug() << "File already opened:" << filename;
					chartwindow->activateWindow();
					return true;
				}
			}
		}
	}

	return false;
}

QStringList MainWindow::filesFromSettings(QString option) {
	QSettings settings;
	auto recentfiles = settings.value(option).toStringList();
	auto count = recentfiles.count();

	recentfiles.removeDuplicates();
	foreach (auto filename, recentfiles) {
		QFileInfo fi(filename);
		if (!fi.exists()) recentfiles.removeOne(filename);
	}

	if (count != recentfiles.count())
		settings.setValue(option, recentfiles);

	return recentfiles;
}
