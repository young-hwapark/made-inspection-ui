#include "GlobalVariables.h"

QString GlobalVariables::exePath;
QString GlobalVariables::rootPath;
QString GlobalVariables::dataDir;
QString GlobalVariables::imageDir;
QString GlobalVariables::csvDir;
QString GlobalVariables::customerID = "";
QString GlobalVariables::license = "";
QString GlobalVariables::token = "";
QString GlobalVariables::name = "";

int GlobalVariables::maximumTrainingWindows = 1;
double GlobalVariables::tolerance = 0.001;
float GlobalVariables::darknetThresh = float(0.25);
float GlobalVariables::darknetHier = float(0.5);
float GlobalVariables::nmsThresh = float(0.45);
float GlobalVariables::kerasThresh = float(0.05);
double GlobalVariables::kerasSuccess = double(0.8);
int GlobalVariables::kerasMaxRetry = 1;
quint16 GlobalVariables::socketPort = 2556;
int GlobalVariables::currentVRAMusage = 0;
int GlobalVariables::maxVRAMusage = 0;
int GlobalVariables::networkVRAMsize = 1800;
int GlobalVariables::cameraWidth = 1920;
int GlobalVariables::cameraHeight = 1200;

GlobalVariables::GlobalVariables(QObject *parent) : QObject(parent)
{

}
