#ifndef OBJECTPOSITIONCHECKERLOADER_H
#define OBJECTPOSITIONCHECKERLOADER_H

#include "ObjectPositionCheckerSubmodule.h"
#include <QThread>

class ObjectPositionCheckerLoader : public QThread
{
    Q_OBJECT

public:
    ObjectPositionCheckerLoader(ModelInformation info);

    void run();
    TensorFlowInformation getTensorFlowInfo();

signals:
    void loadSuccess();

private:
    ModelInformation modelInformation;
    TensorFlowInformation tensorFlowInformation;
    TF_Session* session;
    TF_Graph* graph;
    TF_Output* inputLayer;
    TF_Output* outputLayer;
    TF_Status* status;
};

#endif // OBJECTPOSITIONCHECKERLOADER_H
