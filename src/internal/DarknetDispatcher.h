#ifndef DARKNETDISPATCHER_H
#define DARKNETDISPATCHER_H

#include <QtWidgets>
#include "ModelInformation.h"
#include <darknet.h>

template <class T> class GenericHolder;
class LetterBoxer;
class DarknetSubmodule;

struct DarknetInformation
{
    int classes;
    char** names;
    network* net;
};

Q_DECLARE_METATYPE(DarknetInformation);
Q_DECLARE_METATYPE(QVector<float>);

class DarknetDispatcher : public QThread
{
    Q_OBJECT

public:
    DarknetDispatcher(GenericHolder<QImage>* imageHolder);
    ~DarknetDispatcher();

    void run();
    void stop();
    GenericHolder<std::vector<detection_with_class>>* getDetectionHolder();
    void setDetectionHolder(GenericHolder<std::vector<detection_with_class>>*);
    DarknetInformation getInfo();
    bool loadSubmodules(ModelInformation info, QWidget* window);

public slots:
    void setNetwork(DarknetInformation info);

private:
    bool loaded;
    bool isDetecting;
    LetterBoxer* cuda;
    GenericHolder<QImage>* imageHolder;
    GenericHolder<std::vector<detection_with_class>>* detectionHolder;
    QVector<DarknetSubmodule*> modules;

    // Darknet members
    network* net;
    int m_classes;
    char** m_names;
    bool detect;
};

#endif // DARKNETDISPATCHER_H
