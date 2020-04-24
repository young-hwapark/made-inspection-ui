#include "DarknetDispatcher.h"
#include "GenericHolder.h"
#include "LetterBoxer.h"
#include "GlobalVariables.h"
#include "modules/SubmoduleFactory.h"
#include <pylon/PylonIncludes.h>

using namespace Pylon;

DarknetDispatcher::DarknetDispatcher(GenericHolder<QImage>* iholder) :
    loaded(false),
    isDetecting(false),
    imageHolder(iholder),
    detect(true)
{
    detectionHolder = new GenericHolder<std::vector<detection_with_class>>;
    cuda = new LetterBoxer(1920, 1200, 608, 608, 3);
}

DarknetDispatcher::~DarknetDispatcher()
{
    delete cuda;
}

void DarknetDispatcher::setDetectionHolder(GenericHolder<std::vector<detection_with_class> > *d)
{
    detectionHolder = d;
}

void DarknetDispatcher::setNetwork(DarknetInformation info)
{
    m_classes = info.classes;
    m_names = info.names;
    net = info.net;
}

void DarknetDispatcher::run()
{
    isDetecting = true;
    CPylonImage* packedImage = new CPylonImage;
    CPylonImage* planarImage = new CPylonImage;
    QImage qimage;
    image* darknetImage = new image;
    *darknetImage = make_image(608, 608, 3);

    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    imageHolder->connectSet(&loop, SLOT(quit()), Qt::DirectConnection);
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

    while (isDetecting) {
        timer.start(1000);
        loop.exec();

        if (timer.isActive()) {
            qimage = imageHolder->get();
            packedImage->CopyImage(qimage.bits(),
                                   qimage.width() * qimage.height() * 3, // May need to add logic for this?
                                   PixelType::PixelType_RGB8packed,
                                   qimage.width(),
                                   qimage.height(),
                                   0);

            CImageFormatConverter converter;
            converter.OutputPixelFormat = PixelType_RGB8planar;
            converter.Convert(*planarImage, *packedImage);

            if (planarImage != nullptr && planarImage->IsValid()){
                uchar* buffer = static_cast<uchar*>(planarImage->GetBuffer());
                cuda->letterbox(buffer, darknetImage->data);
                network_predict(*net, darknetImage->data);
                int nboxes = 0;
                detection *dets = get_network_boxes(net, 1920, 1200,
                                                    GlobalVariables::darknetThresh,
                                                    GlobalVariables::darknetHier,
                                                    0, 1, &nboxes, 1);

                if (nboxes > 0) {
                    do_nms_sort(dets, nboxes, m_classes, GlobalVariables::nmsThresh);
                    int selected_detections_num;
                    detection_with_class* selected_detections = get_actual_detections(dets,
                                                                                      nboxes,
                                                                                      GlobalVariables::darknetThresh,
                                                                                      &selected_detections_num,
                                                                                      m_names);
                    std::vector<detection_with_class> sendDets;
                    for (int i = 0; i < selected_detections_num; i++) {
                        sendDets.push_back(selected_detections[i]);
                    }
                    detectionHolder->set(sendDets);
                } else {
                    detectionHolder->set({});
                }
            }
        }
        msleep(50);
    }
}

void DarknetDispatcher::stop()
{
    isDetecting = false;
}

GenericHolder<std::vector<detection_with_class>>* DarknetDispatcher::getDetectionHolder()
{
    return detectionHolder;
}

DarknetInformation DarknetDispatcher::getInfo()
{
    return DarknetInformation{ m_classes, m_names, net };
}

bool DarknetDispatcher::loadSubmodules(ModelInformation info, QWidget* window)
{
    QStringList submoduleList;
    QMapIterator<QString, QJsonValue> i(info.submoduleMap);
    while (i.hasNext()) {
        i.next();
        if ((window->windowTitle().startsWith("Training Window") && i.value()["showOnTrainingWindow"].toBool()) ||
            (window->windowTitle().startsWith("Inspection Window") && i.value()["showOnInspectionWindow"].toBool()))
            submoduleList.append(i.value()["moduleClassName"].toString());
    }

    modules.clear();
    SubmoduleFactory submoduleFactory(submoduleList.join("|"));

    for (int i = 0; i < submoduleList.size(); ++i) {
        DarknetSubmodule* m = submoduleFactory.getSubmodule(submoduleList.at(i));
        m->setInfo(this, info);
        modules.push_back(m);
    }

    return true;
}
