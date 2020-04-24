#ifndef DARKNETLOADER_H
#define DARKNETLOADER_H

#include <QtWidgets>
#include <darknet.h>
#include "DarknetDispatcher.h"

struct DarknetInformation;

class DarknetLoader : public QThread
{
    Q_OBJECT

public:
    DarknetLoader(QString modelName);
    ~DarknetLoader();
    void run();
    void unload();

signals:
    void networkLoaded(DarknetInformation);
    void loadFailed(QString);

private:
    bool isLoaded;
    QString model;
    network net;
    int m_classes;
    char** m_names;
};

#endif // DARKNETLOADER_H
