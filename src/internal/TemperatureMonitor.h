#ifndef TEMPERATUREMONITOR_H
#define TEMPERATUREMONITOR_H

#include <QThread>

class TemperatureMonitor : public QThread
{
    Q_OBJECT

public:
    TemperatureMonitor();
    void run();
    void stop();
    void setAbort();

private:
    bool abort;
    unsigned int tempMax;
    unsigned int tempCurrent;

signals:
    void temperatureChanged(unsigned int tempCurrent, unsigned int tempMax);
};

#endif // TEMPERATUREMONITOR_H
