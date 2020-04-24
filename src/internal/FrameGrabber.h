#ifndef FRAMEGRABBER_H
#define FRAMEGRABBER_H

#include <QThread>
#include "pylon/PylonIncludes.h"
#include "PylonCamera.h"

template <class T> class GenericHolder;

class FrameGrabber : public QThread
{
    Q_OBJECT

public:
    FrameGrabber(Pylon::CInstantCamera* camera);
    ~FrameGrabber();

    void setIP(QString ip);
    void run();
    void stop();
    void setPlaying(bool);
    void setRecording(bool);
    GenericHolder<QImage>* getImageHolder();
    GenericHolder<Pylon::CPylonImage*>* getPylonHolder();
    void saveImage(QString);

private:
    bool isStop = false;
    bool isPlaying = false;
    bool isRecording = false;
    Pylon::CInstantCamera *instantCamera;
    GenericHolder<QImage>* imageHolder;
    GenericHolder<Pylon::CPylonImage*>* pylonHolder;
};

#endif // FRAMEGRABBER_H
