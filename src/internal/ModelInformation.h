#ifndef MODELINFORMATION_H
#define MODELINFORMATION_H

#include <QtWidgets>

struct SubpartModel
{
    QString name;
    QString color;
    QString yoloID;
    int numObjects;
    bool isPositive;
};

struct ModelInformation
{
    QMap<QString, SubpartModel*> subpartMap;
    QString modelID;
    QString modelName;
    QString modelType;
    QString desc;
    QString status;
    QString lastUpdated;
    QString modelIP;
    QString destIP;
    QString nextModel;
    QString sequence = "";
    int totalNumObj = 0;
    QMap<QString, QString> label;
    QMap<QString, QJsonValue> submoduleMap;

    // ObjectDetectionSubmodule
    QMap<int, int> expectedObjects; // do this in constructor
    double objectDifferenceThreshold = 0.8; // will be moved to settings
};

#endif // MODELINFORMATION_H
