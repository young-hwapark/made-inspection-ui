#ifndef DARKNETSUBMODULE_H
#define DARKNETSUBMODULE_H

#include "../DarknetDispatcher.h"
#include "../ModelInformation.h"
#include <QtWidgets>

class DarknetSubmodule : public QObject
{
    Q_OBJECT

public:
    DarknetSubmodule() { qRegisterMetaType<QVector<float>>(); }

    virtual void startInspection() = 0;
    virtual void stopInspection() = 0;
    virtual void setInfo(DarknetDispatcher* dispatcher = nullptr, ModelInformation info = {}) = 0;
    bool getResult() { return result; }
    QString status() { return statusString; }

signals:
    void moduleReady();
    void newResult();

protected:
    bool result;
    QString statusString;
};


#endif // DARKNETSUBMODULE_H
