#ifndef OBJECTDETECTIONSUBMODULE_H
#define OBJECTDETECTIONSUBMODULE_H

#include "../DarknetSubmodule.h"

class ObjectDetectionSubmodule : public DarknetSubmodule
{
    Q_OBJECT

public:
    ObjectDetectionSubmodule();

    void startInspection();
    void stopInspection();
    void setInfo(DarknetDispatcher* dispatcher = nullptr, ModelInformation info = {});

signals:
    void newResult();

private:
    DarknetDispatcher* darknetDispatcher;
    GenericHolder<std::vector<detection_with_class>>* detectionHolder;
    ModelInformation info;
    bool checkMissing;
    bool checkExtra;
    QMap<int, int> objectDifferences;
    QMap<int, int> expectedObjects;
    QMap<int, QMap<int, int>> frequencyMap;
    int inspectionTally;

    void createStatusString();

private slots:
    void newDetection();
};

#endif // OBJECTDETECTIONSUBMODULE_H
