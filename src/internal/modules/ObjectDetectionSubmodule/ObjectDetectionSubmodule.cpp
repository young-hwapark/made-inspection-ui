#include "ObjectDetectionSubmodule.h"
#include "../../GenericHolder.h"

ObjectDetectionSubmodule::ObjectDetectionSubmodule() :
    inspectionTally(0)
{

}

void ObjectDetectionSubmodule::startInspection()
{
    if (!darknetDispatcher) {
        statusString = "ERROR: Darknet Dispatcher not set.";
        return;
    }
    result = false;
    inspectionTally = 0;
    objectDifferences.clear();
    frequencyMap.clear();
    detectionHolder->connectSet(this, SLOT(newDetection()));
}

void ObjectDetectionSubmodule::stopInspection()
{
    detectionHolder->disconnectSet(this, SLOT(newDetection()));
    createStatusString();
    emit newResult();
}

void ObjectDetectionSubmodule::setInfo(DarknetDispatcher *dispatcher, ModelInformation darknetInfo)
{
    darknetDispatcher = dispatcher;
    if (darknetDispatcher)
        detectionHolder = dispatcher->getDetectionHolder();
    info = darknetInfo;
    expectedObjects = info.expectedObjects;
}

void ObjectDetectionSubmodule::newDetection()
{
    if (inspectionTally == 0)
        result = true;

    inspectionTally++;

    std::vector<detection_with_class> dets;
    dets = detectionHolder->get();

    QMap<int, int> detectedObjects;

    for (ulong i = 0; i < dets.size(); i++) {
        detectedObjects[dets.at(i).best_class]++;
        objectDifferences[dets.at(i).best_class]++;
    }

    QMapIterator<int, int> i(expectedObjects);
    while (i.hasNext()) {
        i.next();
        objectDifferences[i.key()] -= i.value();
        frequencyMap[i.key()][detectedObjects[i.key()] - i.value()]++;
    }

    QMapIterator<int, int> j(objectDifferences);
    while (j.hasNext()) {
        j.next();
        if (abs(double(j.value()) / double(inspectionTally)) > info.objectDifferenceThreshold)
            result = false;
    }

}

void ObjectDetectionSubmodule::createStatusString()
{
    if (result) {
        statusString = "Success. Correct parts detected.";
        return;
    }
    bool needComma = false;
    bool header = true;

    QString message = "Failed. Incorrect sub-parts: ";
    char** names = darknetDispatcher->getInfo().names;

    QMapIterator<int, int> i(objectDifferences);
    while (i.hasNext()) {
        i.next();
        if (i.value() * (-1) > info.objectDifferenceThreshold) {
            if (header) {
                message += "\nMissing: ";
                header = false;
            }
            if (needComma)
                message += ", ";
            else
                needComma = true;

            QMapIterator<int, int> f(frequencyMap[i.key()]);
            f.next();
            int freq = f.key();
            int num = f.value();
            while (f.hasNext()) {
                f.next();
                if (f.value() > num) {
                    freq = f.key();
                    num = f.value();
                }
            }
            double prob = 100 * i.value() / (inspectionTally * freq);

            message += QString::number(freq * (-1)) + "x " + QString(names[i.key()]) + "(" + QString::number(prob) + "%)";
        }
    }

    if (!header)
        message += ". ";
    needComma = false;
    header = true;

    i.toFront();
    while (i.hasNext()) {
        i.next();
        if (i.value() > info.objectDifferenceThreshold) {
            if (header) {
                message += "\nExtra: ";
                header = false;
            }
            if (needComma)
                message += ", ";
            else
                needComma = true;

            QMapIterator<int, int> f(frequencyMap[i.key()]);
            f.next();
            int freq = f.key();
            int num = f.value();
            while (f.hasNext()) {
                f.next();
                if (f.value() > num) {
                    freq = f.key();
                    num = f.value();
                }
            }
            double prob = 100 * i.value() / (inspectionTally * freq);

            message += QString::number(freq) + "x " + QString(names[i.key()]) + "(" + QString::number(prob) + "%)";
        }
    }

    if (!header)
        message += ". ";

    statusString = message;
}
