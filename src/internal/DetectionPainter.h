#ifndef DETECTIONPAINTER_H
#define DETECTIONPAINTER_H

#include <QtWidgets>
#include <darknet.h>
#include "DarknetDispatcher.h"
#include "ModelInformation.h"

template <class T> class GenericHolder;

class DetectionPainter : public QThread
{
    Q_OBJECT

public:
    DetectionPainter(GenericHolder<QImage>* inputImage,
                     GenericHolder<std::vector<detection_with_class>>* dets);

    void run();
    void stop();
    GenericHolder<QImage>* getImageHolder();
    void setModelInformation(ModelInformation info);

public slots:
    void setDarknetInformation(DarknetInformation info);

private:
    bool isRunning;
    GenericHolder<QImage>* holderIn;
    GenericHolder<QImage>* holderOut;
    GenericHolder<std::vector<detection_with_class>>* darknetHolder;
    DarknetInformation darknetInformation;
    ModelInformation modelInformation;
};

#endif // DETECTIONPAINTER_H
