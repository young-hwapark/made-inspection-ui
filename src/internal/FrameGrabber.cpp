#include "FrameGrabber.h"
#include "GenericHolder.h"
#include "PylonCamera.h"
#include "GlobalVariables.h"
#include <QtDebug>
#include <QImage>

using namespace Pylon;

FrameGrabber::FrameGrabber(Pylon::CInstantCamera* camera) :
    instantCamera(camera)
{
    imageHolder = new GenericHolder<QImage>;
    pylonHolder = new GenericHolder<CPylonImage*>;
}

FrameGrabber::~FrameGrabber()
{

}

void FrameGrabber::run()
{
    try {
        CPylonImage pylonImage;
        QImage img;
        CGrabResultPtr ptrGrab;
        CImageFormatConverter converter;
        converter.OutputPixelFormat = PixelType_RGB8packed;
        const uchar* buffer;

        instantCamera->StartGrabbing();

        while (instantCamera->IsGrabbing() && !isStop) {
            instantCamera->RetrieveResult(1000, ptrGrab, TimeoutHandling_Return);

            if(!ptrGrab->GrabSucceeded() || !ptrGrab.IsValid()) {
                 qDebug() << "Grabbing frame failure.";
            } else {
                if (!isPlaying) {
                    converter.Convert(pylonImage, ptrGrab);

                    if (pylonImage.IsValid()) {
                        if (isRecording)
                            pylonHolder->set(&pylonImage);

                        int width = int(pylonImage.GetWidth());
                        int height = int(pylonImage.GetHeight());
                        int step = int(pylonImage.GetAllocatedBufferSize() / ulong(height));

                        buffer = static_cast<uchar*>(pylonImage.GetBuffer());
                        QImage qimage = QImage(buffer, width, height, step,
                                               QImage::Format_RGB888);

                        imageHolder->set(qimage);
                    }
                }
            }
        }

        instantCamera->StopGrabbing();
    } catch (GenericException &e) {
        qDebug() << e.GetDescription();
    }
}

void FrameGrabber::stop()
{
    isStop = true;
}

void FrameGrabber::setPlaying(bool x)
{
    isPlaying = x;
}

void FrameGrabber::setRecording(bool x)
{
    isRecording = x;
}

GenericHolder<QImage>* FrameGrabber::getImageHolder()
{
    return imageHolder;
}

GenericHolder<CPylonImage*>* FrameGrabber::getPylonHolder()
{
    return pylonHolder;
}

void FrameGrabber::saveImage(QString path)
{
    imageHolder->get().save(path);
}
