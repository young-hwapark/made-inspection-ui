#include "TemperatureMonitor.h"
#include <nvml.h>
#include <QDebug>

TemperatureMonitor::TemperatureMonitor()
{
    abort = false;
    tempMax = 0;
    tempCurrent = 0;
}

void TemperatureMonitor::setAbort()
{
    abort = true;
}

void TemperatureMonitor::run()
{
    nvmlReturn_t result;

    while (true && !abort) {
        result = nvmlInit();

        if (NVML_SUCCESS != result) {
            qDebug() << "Failed to initialize NVML: ";
            sleep(3);
            continue;
        }

        nvmlDevice_t device;
        result = nvmlDeviceGetHandleByIndex(0, &device);

        if (NVML_SUCCESS != result) {
            qDebug() << "Failed to get handle for device: ";
            sleep(3);
            continue;
        }

        result = nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &tempCurrent);

        if (NVML_SUCCESS != result) {
            qDebug() << "Failed to get temperature of device";
            sleep(3);
            continue;
        }

        if (tempCurrent > tempMax)
            tempMax = tempCurrent;

        emit temperatureChanged(tempCurrent, tempMax);
        sleep(1);
    }

}

void TemperatureMonitor::stop()
{
    nvmlShutdown();
}
