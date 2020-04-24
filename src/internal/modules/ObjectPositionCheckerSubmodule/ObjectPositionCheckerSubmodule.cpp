#include "ObjectPositionCheckerSubmodule.h"
#include "ObjectPositionCheckerLoader.h"
#include "ObjectPositionCheckerDispatcher.h"
#include "../../GenericHolder.h"
#include "../../utils/tf_utils.h"

ObjectPositionCheckerSubmodule::ObjectPositionCheckerSubmodule() :
    loaded(false)
{

}

void ObjectPositionCheckerSubmodule::setInfo(DarknetDispatcher *darknetDispatcher, ModelInformation info)
{
    detectionHolder = darknetDispatcher->getDetectionHolder();
    loader = new ObjectPositionCheckerLoader(info);
    dispatcher = new ObjectPositionCheckerDispatcher(detectionHolder);
    connect(loader, SIGNAL(loadSuccess()), this, SLOT(shareInfo()));
    loader->start();
}

void ObjectPositionCheckerSubmodule::startInspection()
{
    result = false;
    if (!loaded) {
        statusString = "Info was not set.";
        return;
    }
    dispatcher->start();
    connect(dispatcher, SIGNAL(inspectionFinished(QVector<float>)), this, SLOT(getInspectionResult(QVector<float>)));
}

void ObjectPositionCheckerSubmodule::stopInspection()
{
    dispatcher->stop();
}

void ObjectPositionCheckerSubmodule::shareInfo()
{
    dispatcher->setTensorFlowInformation(loader->getTensorFlowInfo());
    loaded = true;
    emit moduleReady();
}

void ObjectPositionCheckerSubmodule::getInspectionResult(QVector<float> vec)
{
    result = true;
    if (!vec.size()) {
        result = false;
        statusString = "No detections found.";
    }
    for (int i = 0; i < vec.size(); ++i) {
        if (vec[i] > 0.1) { // SET THRESHOLD
            result = false;
            statusString = "An object was misplaced.";
        }
    }
    emit newResult();
}
