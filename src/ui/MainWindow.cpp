#include "MainWindow.h"
#include "LoginWidget.h"
#include "SettingsMenu.h"
#include "CameraSelector.h"
#include "TrainingWindow.h"
#include "../internal/GlobalVariables.h"
#include "../internal/TemperatureMonitor.h"
#include <QApplication>


#include "../internal/modules/ObjectDetectionSubmodule/TestObjectDetectionSubmodule.h"
#include "../internal/modules/ObjectPositionCheckerSubmodule/TestObjectPositionCheckerSubmodule.h"
#include "InspectionSetting.h"


#ifdef Q_OS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

extern "C" MY_EXPORT int create(int c, char *v[])
{
    QApplication a(c, v);
    QCoreApplication::setOrganizationName("NeuronAware Inc.");
    QCoreApplication::setApplicationName("NVision");
    QCoreApplication::setApplicationVersion(APP_VERSION);
    MainWindow *w = new MainWindow();
    w->show();
    return a.exec();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
#ifndef GPU
    qDebug() << "GPU is not detected. Program terminating.";
    exit(1);
#endif

    InspectionSetting* s = new InspectionSetting;
    s->exec();

    QString settingFile = QApplication::applicationDirPath() + "/config.ini";
    QSettings config(settingFile, QSettings::IniFormat);
    config.setIniCodec("UTF-8");

    customerID = config.value("customerID", "").toString();
    license = config.value("license", "").toString();
    GlobalVariables::customerID = customerID;
    GlobalVariables::license = license;
    GlobalVariables::exePath = QApplication::applicationDirPath() + "/bin/";
    GlobalVariables::rootPath = QApplication::applicationDirPath() + "/";
    GlobalVariables::dataDir = QApplication::applicationDirPath() + "/data/";
    GlobalVariables::imageDir = QApplication::applicationDirPath() + "/images/";
    GlobalVariables::csvDir = QApplication::applicationDirPath() + "/csv/";
    setPaths();



    //    TestObjectDetectionSubmodule test;
//    test = new TestObjectPositionCheckerSubmodule;

    tempMonitor = new TemperatureMonitor;
    createStatusBar();

    createActions();
    createMenus();

    QWidget* mainArea = new QWidget(this);
    mdiArea = new QMdiArea(this);

    QVBoxLayout* layout = new QVBoxLayout(mainArea);
    layout->addWidget(mdiArea);
    setCentralWidget(mainArea);

    resize(854, 480);
    show();
    openLogin();
}

MainWindow::~MainWindow()
{

}

void MainWindow::setPaths()
{
    QDir pathRunner(QApplication::applicationDirPath());
    GlobalVariables::exePath = pathRunner.path();

    pathRunner.cdUp();
    GlobalVariables::rootPath = pathRunner.path();

    if (!pathRunner.cd("data")) {
        pathRunner.mkdir("data");
        pathRunner.cd("data");
    }
    GlobalVariables::dataDir = pathRunner.path();
    pathRunner.cdUp();

    if (!pathRunner.cd("images")) {
        pathRunner.mkdir("images");
        pathRunner.cd("images");
    }
    GlobalVariables::imageDir = pathRunner.path();
    pathRunner.cdUp();

    if (!pathRunner.cd("csv")) {
        pathRunner.mkdir("csv");
        pathRunner.cd("csv");
    }
    GlobalVariables::csvDir = pathRunner.path();
    pathRunner.cdUp();
}

void MainWindow::createStatusBar()
{
    tempLabel = new QLabel(this);
    tempLabel->setStyleSheet("QLabel { background-color : blue; color : white; border: 1px solid white}");
    statusBar()->addPermanentWidget(tempLabel);
    connect(tempMonitor, SIGNAL(temperatureChanged(unsigned int, unsigned int)), this, SLOT(handleTemperatureChanged(unsigned int, unsigned int)));
    tempMonitor->start();
}

void MainWindow::createActions()
{
    loginAct = new QAction(tr("Login"), this);
    connect(loginAct, SIGNAL(triggered()), this, SLOT(openLogin()));

    settingsAct = new QAction(tr("Settings"), this);
    connect(settingsAct, SIGNAL(triggered()), this, SLOT(openSettings()));

    exitAct = new QAction(tr("Exit"), this);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(closeApplication()));

    basicAct = new QAction(tr("Basic Inspection"), this);
    basicAct->setEnabled(false);

    integratedAct = new QAction(tr("Integrated Inspection"), this);
    integratedAct->setEnabled(false);

    trainingAct = new QAction(tr("Training Window"), this);
    connect(trainingAct, SIGNAL(triggered()), this, SLOT(openCameraSelector()));
    trainingAct->setEnabled(false);
}

void MainWindow::createMenus()
{
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    QMenu* fileMenu = menuBar->addMenu("File");
    QMenu* inspectionMenu = menuBar->addMenu("Inspection");

    fileMenu->addAction(loginAct);
    fileMenu->addAction(settingsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    inspectionMenu->addAction(basicAct);
    inspectionMenu->addAction(integratedAct);
    inspectionMenu->addSeparator();
    inspectionMenu->addAction(trainingAct);
}

void MainWindow::login()
{
    loginWindow->close();
    loginAct->setEnabled(false);
    trainingAct->setEnabled(true);
    // enable future menubar options here
}

void MainWindow::openLogin()
{
    LoginWidget* loginWidget = new LoginWidget(this);
    loginWindow = mdiArea->addSubWindow(loginWidget);
    connect(loginWidget, SIGNAL(loginSuccess()), this, SLOT(login()));
    connect(loginWidget, SIGNAL(rejected()), loginWindow, SLOT(close()));

    loginWindow->move(mdiArea->geometry().x() + (mdiArea->geometry().width() - loginWindow->geometry().width()) / 2,
                      mdiArea->geometry().y() + (mdiArea->geometry().height() - loginWindow->geometry().height()) / 2);
    loginWidget->show();
}

void MainWindow::openSettings()
{
    SettingsMenu menu(this);
    menu.exec();
}

void MainWindow::closeApplication()
{
    QApplication::quit();
}

void MainWindow::openCameraSelector()
{
    CameraSelector* selector = new CameraSelector;
    connect(selector, SIGNAL(openTrainingWindow(const QString)), this, SLOT(openTrainingWindow(const QString)));
    selector->exec();
}

void MainWindow::openTrainingWindow(const QString ip)
{
    QList<QMdiSubWindow*> windowList = mdiArea->subWindowList();
    int windowCount = 0;
    for (int i = 0; i < windowList.size(); i++) {
        if (windowList.at(i)->windowTitle().startsWith("Training Window"))
            windowCount++;
    }
    if (windowCount < GlobalVariables::maximumTrainingWindows) {
        TrainingWindow* trainingWindow = new TrainingWindow(ip);
        QMdiSubWindow* training = mdiArea->addSubWindow(trainingWindow);
        training->show();
        training->resize(500, 300);
    }
    else {
        QMessageBox msg(QMessageBox::Warning, "Warning", "Maximum number of Training Windows(" + QString::number(GlobalVariables::maximumTrainingWindows) + ") reached.");
        msg.exec();
    }
}

void MainWindow::handleTemperatureChanged(unsigned int tempCurrent, unsigned int tempMax)
{
    auto tempString = QString(tr("%1°C, Max: %2°C")).arg(tempCurrent).arg(tempMax);

    if (tempCurrent <= 40)
        tempLabel->setStyleSheet("QLabel { background-color : blue; color : white; border: 1px solid white}");
    else if (tempCurrent > 40 && tempCurrent <= 70)
        tempLabel->setStyleSheet("QLabel { background-color : green; color : white; border: 1px solid white}");
    else
        tempLabel->setStyleSheet("QLabel { background-color : red; color : white; border: 1px solid white}");

    tempLabel->setText(tempString);
}
