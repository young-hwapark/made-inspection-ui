#include "PylonCamera.h"
#include <QtDebug>

using namespace Pylon;
using namespace std;

PylonCamera::PylonCamera()
{

}

PylonCamera::~PylonCamera()
{
    try {
        if (pDevice) {
            if (pDevice->IsOpen()) {
                pDevice->Close();
            }
        }
    } catch (GenericException &e) {
        qDebug() << "Could not close PylonCamera: " << e.GetDescription();
    } catch ( ... ) {
        qDebug() << "Could not close PylonCamera: Unknown reason.";
    }
}

DeviceInfoList_t PylonCamera::getDeviceList()
{
    DeviceInfoList_t list;

    try {
        CTlFactory& TlFactory = CTlFactory::GetInstance();
        TlFactory.EnumerateDevices(list);
    } catch (GenericException &e) {
        qDebug() << "Couldn't get the device list using PylonSDK"
                   << e.GetDescription();
    } catch ( ... ) {
        qDebug() << "Couldn't get the device list using PylonSDK";
    }

    return list;
}

CInstantCamera *PylonCamera::createCameraFromIP(string address)
{
    try {
        CTlFactory& TlFactory = CTlFactory::GetInstance();
        DeviceInfoList_t listDevices;
        TlFactory.EnumerateDevices(listDevices);

        if (!listDevices.empty()) {
            DeviceInfoList_t::const_iterator it;
            string delimiter = "#";
            string port = ":";

            for (it = listDevices.begin(); it != listDevices.end(); ++it) {
                string ip = string(it->GetFullName()); // Move this to InspectionWindow later
                auto start = ip.find_last_of(delimiter) + 1;
                auto end = ip.find_last_of(port);
                ip = ip.substr(start, end - start);

                if(ip == address){
                    qDebug() << "Connected to: " << ip.c_str();
                    break;
                } else if (it == listDevices.end() - 1) {
                    qDebug() << "No matching IP address";
                    return nullptr;
                }
            }
            pDevice = TlFactory.CreateDevice(listDevices[ulong(it - listDevices.begin())]);
            CInstantCamera *camera = new CInstantCamera(pDevice);

            return camera;
        } else {
            qDebug() << "No devices found!";
            return nullptr;
        }
    } catch (GenericException &e) {
        qDebug() << "Couldn't initiate the device list using PylonSDK"
                   << e.GetDescription();
    } catch ( ... ) {
        qDebug() << "Couldn't get the device list using PylonSDK";
    }

    return nullptr;
}
