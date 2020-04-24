#ifndef PYLONCAMERA_H
#define PYLONCAMERA_H

#include <string>
#include <pylon/PylonIncludes.h>

using namespace std;

class PylonCamera
{
public:
    PylonCamera();
    ~PylonCamera();

    Pylon::CInstantCamera* createCameraFromIP(string);
    Pylon::DeviceInfoList_t getDeviceList();

private:
    Pylon::IPylonDevice *pDevice = nullptr;
};

#endif // PYLONCAMERA_H
