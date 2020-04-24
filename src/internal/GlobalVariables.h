#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

#include <QObject>

class GlobalVariables : public QObject
{
    Q_OBJECT
public:
    explicit GlobalVariables(QObject *parent = nullptr);

public:
    static QString exePath;
    static QString rootPath;
    static QString dataDir;
    static QString imageDir;
    static QString csvDir;
    static QString customerID;
    static QString license;
    static QString token;
    static QString name;
    static int maximumTrainingWindows;
    static double tolerance;
    static float darknetThresh;
    static float darknetHier;
    static float nmsThresh;
    static float kerasThresh;
    static double kerasSuccess;
    static int kerasMaxRetry;
    static quint16 socketPort;
    static int currentVRAMusage;
    static int maxVRAMusage;
    static int networkVRAMsize;
    static int cameraWidth;
    static int cameraHeight;
};

#endif // GLOBALVARIABLES_H
