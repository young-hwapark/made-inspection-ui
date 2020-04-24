#ifndef TRAININGWINDOW_H
#define TRAININGWINDOW_H

#include <QtWidgets>
#include <pylon/PylonIncludes.h>
#include <darknet.h>

template <class T> class GenericHolder;
class VideoDisplay;
class VideoPlayback;
class FrameGrabber;
class DarknetLoader;
class DarknetDispatcher;
class DetectionPainter;
struct ModelInformation;

class TrainingWindow : public QWidget
{
    Q_OBJECT

public:
    TrainingWindow(QString ip);
    ~TrainingWindow();

    void createLayout(QVBoxLayout*);
    void createActions();
    void createToolbar();

private:
    VideoDisplay* videoDisplay;
    VideoPlayback* videoPlayback;
    FrameGrabber* frameGrabber;
    DarknetLoader* darknetLoader;
    DarknetDispatcher* darknetDispatcher;
    DetectionPainter* detectionPainter;

    QString status;
    QStatusBar* statusBar;
    QToolBar* toolbar;
    QAction* screenCaptureAct;
    QAction* recordVideoAct;
    QAction* openVideoAct;
    QAction* openModelAct;

    QString imgPath;

private slots:
    void saveImage();
    void beginDetection();
    void loadModel(ModelInformation* model);
    void modelLoaded();
    void modelFailed(QString message);
    void recordVideo();
    void playVideo();
    void setStatus(QString status);
    void setFps(double fps);
};

#endif // TRAININGWINDOW_H
