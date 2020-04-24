#ifndef OBJECTPOSITIONCHECKER_H
#define OBJECTPOSITIONCHECKER_H

#include "../DarknetSubmodule.h"
#include "../../utils/tf_utils.h"

class ObjectPositionCheckerLoader;
class ObjectPositionCheckerDispatcher;
template<class T> class GenericHolder;

struct TensorFlowInformation {
    TF_Session* session;
    TF_Graph* graph;
    TF_Output* inputLayer;
    TF_Output* outputLayer;
    TF_Status* status;
    std::vector<int64_t> inputDims;
};

class ObjectPositionCheckerSubmodule : public DarknetSubmodule
{
    Q_OBJECT

public:
    ObjectPositionCheckerSubmodule();

    void setInfo(DarknetDispatcher *dispatcher = nullptr, ModelInformation info = {});
    void startInspection();
    void stopInspection();

private slots:
    void shareInfo();
    void getInspectionResult(QVector<float>);

private:
    bool loaded;
    ObjectPositionCheckerLoader* loader;
    ObjectPositionCheckerDispatcher* dispatcher;
    GenericHolder<std::vector<detection_with_class>>* detectionHolder;
};

#endif // OBJECTPOSITIONCHECKER_H
