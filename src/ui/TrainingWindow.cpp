#include "TrainingWindow.h"
#include "VideoDisplay.h"
#include "ModelSelector.h"
#include "../internal/GenericHolder.h"
#include "../internal/PylonCamera.h"
#include "../internal/FrameGrabber.h"
#include "../internal/DarknetDispatcher.h"
#include "../internal/DarknetLoader.h"
#include "../internal/DetectionPainter.h"
#include "../internal/VideoPlayback.h"
#include "../internal/ModelInformation.h"
#include "../internal/GlobalVariables.h"


using namespace Pylon;

TrainingWindow::TrainingWindow(QString ip)
{
    PylonInitialize();
    setWindowTitle("Training Window - " + ip);

    PylonCamera* pylonCamera = new PylonCamera;
    videoDisplay = new VideoDisplay(this);
    frameGrabber = new FrameGrabber(pylonCamera->createCameraFromIP(ip.toStdString()));
    videoPlayback = new VideoPlayback(frameGrabber->getPylonHolder(), frameGrabber->getImageHolder());
    darknetDispatcher = new DarknetDispatcher(frameGrabber->getImageHolder());
    darknetLoader = nullptr;
    detectionPainter = new DetectionPainter(frameGrabber->getImageHolder(),
                                            darknetDispatcher->getDetectionHolder());

    videoDisplay->start(frameGrabber->getImageHolder());
    frameGrabber->start();
    connect(videoDisplay, SIGNAL(fpsChanged(double)), this, SLOT(setFps(double)));

    QDateTime time = QDateTime::currentDateTime();
    qint64 e = time.toMSecsSinceEpoch();
    imgPath = GlobalVariables::imageDir + "/" + QString::number(e);
    QDir dir(GlobalVariables::imageDir);
    if (!dir.cd(imgPath))
        dir.mkdir(imgPath);

    createActions();
    createToolbar();
    QVBoxLayout* layout = new QVBoxLayout(this);
    createLayout(layout);
}

TrainingWindow::~TrainingWindow()
{
    QDir dir(imgPath);
    if (dir.isEmpty())
        dir.rmdir(imgPath);

    if (recordVideoAct->isChecked()) {
        videoPlayback->stopRecording();
        frameGrabber->setRecording(false);
    }

    if (openVideoAct->isChecked()) {
        videoPlayback->stopPlaying();
        frameGrabber->setPlaying(false);
    }

    if (!videoPlayback->wait(5000)) {
        videoPlayback->terminate();
        videoPlayback->wait();
    }
    videoPlayback->deleteLater();

    detectionPainter->stop();
    if (!detectionPainter->wait(5000)) {
        detectionPainter->terminate();
        detectionPainter->wait();
    }
    detectionPainter->deleteLater();

    darknetDispatcher->stop();
    if (!darknetDispatcher->wait(5000)) {
        darknetDispatcher->terminate();
        darknetDispatcher->wait();
    }
    darknetDispatcher->deleteLater();

    if (darknetLoader != nullptr)
        darknetLoader->unload();

    frameGrabber->stop();
    if (!frameGrabber->wait(5000)) {
        frameGrabber->terminate();
        frameGrabber->wait();
    }
    frameGrabber->deleteLater();
    PylonTerminate();
}

void TrainingWindow::createLayout(QVBoxLayout *layout)
{
    QMargins zeroMargin;
    QFrame* hLine = new QFrame();
    hLine->setFrameShape(QFrame::HLine);
    hLine->setFrameShadow(QFrame::Sunken);

    QWidget* trainingWindowContainer = new QWidget(this);
    QHBoxLayout* trainingWindowLayout = new QHBoxLayout(trainingWindowContainer);
    trainingWindowLayout->addWidget(videoDisplay);

    statusBar = new QStatusBar(this);
    statusBar->showMessage("Ready");

    layout->addWidget(toolbar);
    layout->addWidget(hLine);
    layout->addWidget(trainingWindowContainer);
    layout->addWidget(statusBar);

    hLine->setContentsMargins(zeroMargin);
    trainingWindowContainer->setContentsMargins(zeroMargin);
    trainingWindowLayout->setContentsMargins(zeroMargin);
    layout->setContentsMargins(zeroMargin);
}

void TrainingWindow::createActions()
{
    QIcon camera("://resources/toolbar-icons/camera.png");
    screenCaptureAct = new QAction(camera, tr("Capture a frame"), this);
    connect(screenCaptureAct, SIGNAL(triggered()), this, SLOT(saveImage()));


    QIcon record("://resources/toolbar-icons/record.png");
    recordVideoAct = new QAction(record, tr("Record a video"), this);
    recordVideoAct->setCheckable(true);
    connect(recordVideoAct, SIGNAL(triggered()), this, SLOT(recordVideo()));


    QIcon open("://resources/toolbar-icons/open.png");
    openVideoAct = new QAction(open, tr("Open a video"), this);
    openVideoAct->setCheckable(true);
    connect(openVideoAct, SIGNAL(triggered()), this, SLOT(playVideo()));


    QIcon detect("://resources/toolbar-icons/detect.png");
    openModelAct = new QAction(detect, tr("Open a model"), this);
    openModelAct->setCheckable(true);
    connect(openModelAct, SIGNAL(triggered()), this, SLOT(beginDetection()));
}

void TrainingWindow::createToolbar()
{
    toolbar = new QToolBar(this);
    toolbar->addAction(screenCaptureAct);
    toolbar->addAction(recordVideoAct);
    toolbar->addAction(openVideoAct);
    toolbar->addSeparator();
    toolbar->addAction(openModelAct);
}

void TrainingWindow::saveImage()
{
    frameGrabber->saveImage(imgPath + "/" + QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()) + ".jpg");
}

void TrainingWindow::beginDetection()
{
    if (!openModelAct->isChecked()) {
        videoDisplay->stop();
        videoDisplay->start(frameGrabber->getImageHolder());
        detectionPainter->stop();
        darknetDispatcher->stop();
        darknetLoader->unload();
        darknetLoader->deleteLater();
        setStatus("");
        return;
    }
    setStatus("Load a model");
    ModelSelector* dialog = new ModelSelector;
    connect(dialog, SIGNAL(openModel(ModelInformation*)), this, SLOT(loadModel(ModelInformation*)));
    if (!dialog->exec()) {
        openModelAct->setChecked(false);
    }
}

void TrainingWindow::loadModel(ModelInformation* model)
{
    darknetLoader = new DarknetLoader(QString(model->modelID.split("|").at(2)));
    detectionPainter->setModelInformation(*model);
    darknetDispatcher->loadSubmodules(*model, this);

    QEventLoop loop;
    int id = QMetaType::type("DarknetInformation");
    if (id == QMetaType::UnknownType) {
        qRegisterMetaType<DarknetInformation>();
    }

    connect(darknetLoader, SIGNAL(networkLoaded(DarknetInformation)), darknetDispatcher, SLOT(setNetwork(DarknetInformation)));
    connect(darknetLoader, SIGNAL(networkLoaded(DarknetInformation)), detectionPainter, SLOT(setDarknetInformation(DarknetInformation)));
    connect(darknetLoader, SIGNAL(networkLoaded(DarknetInformation)), &loop, SLOT(quit()));
    connect(darknetLoader, SIGNAL(loadFailed(QString)), &loop, SLOT(quit()));
    connect(darknetLoader, SIGNAL(networkLoaded(DarknetInformation)), this, SLOT(modelLoaded()));
    connect(darknetLoader, SIGNAL(loadFailed(QString)), this, SLOT(modelFailed(QString)));

    darknetLoader->start();
    loop.exec();
}

void TrainingWindow::modelLoaded()
{
    videoDisplay->stop();
    videoDisplay->start(detectionPainter->getImageHolder());
    darknetDispatcher->start();
    detectionPainter->start();
    setStatus("Detecting");
}

void TrainingWindow::modelFailed(QString message)
{
    QMessageBox msg(QMessageBox::Critical, "Load Failed", message);
    msg.exec();
}

void TrainingWindow::recordVideo()
{
    if (!recordVideoAct->isChecked()) {
        videoPlayback->stopRecording();
        frameGrabber->setRecording(false);
        setStatus("");
        openVideoAct->setEnabled(true);
        return;
    }
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter("*.mpeg");
    dialog.setDefaultSuffix("mpeg");
    QString filename;
    if (dialog.exec()) {
        filename = dialog.selectedFiles().first();
        frameGrabber->setRecording(true);
        videoPlayback->startRecording(filename);
        setStatus("Recording");
        openVideoAct->setEnabled(false);
    } else recordVideoAct->setChecked(false);
}

void TrainingWindow::playVideo()
{
    if (openVideoAct->isChecked()) {
        QFileDialog dialog(this);
        dialog.setWindowModality(Qt::WindowModal);
        dialog.setAcceptMode(QFileDialog::AcceptOpen);
        dialog.setNameFilter("*.mpeg");
        QString filename;
        if (dialog.exec()) {
            filename = dialog.selectedFiles().first();
            frameGrabber->setPlaying(true);
            videoPlayback->playVideo(filename);
            setStatus("Playing");
            recordVideoAct->setEnabled(false);
        } else openVideoAct->setChecked(false);
    } else {
        videoPlayback->stopPlaying();
        frameGrabber->setPlaying(false);
        setStatus("");
        recordVideoAct->setEnabled(true);
    }
}

void TrainingWindow::setStatus(QString s)
{
    status = s;
    statusBar->showMessage(status);
    statusBar->update();
}

void TrainingWindow::setFps(double fps)
{
    statusBar->showMessage(status + "  FPS: " + QString::number(fps));
    statusBar->update();
}
