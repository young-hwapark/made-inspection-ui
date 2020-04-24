#ifndef OBJECTPOSITIONCHECKERDISPATCHER_H
#define OBJECTPOSITIONCHECKERDISPATCHER_H

#include "ObjectPositionCheckerSubmodule.h"
#include <QThread>

class ObjectPositionCheckerDispatcher : public QThread
{
    Q_OBJECT

public:
    ObjectPositionCheckerDispatcher(GenericHolder<std::vector<detection_with_class>>* holder);

    void run();
    void stop();
    void setTensorFlowInformation(TensorFlowInformation info);

signals:
    void inspectionFinished(QVector<float>);

private:
    bool isInspecting;
    GenericHolder<std::vector<detection_with_class>>* detectionHolder;

    TF_Session* session;
    TF_Graph* graph;
    TF_Output* inputLayer;
    TF_Output* outputLayer;
    TF_Status* status;
    std::vector<int64_t> inputDims;
    QVector<float> outputDiffTotal;
    int inspectionTally;

    QVector<float> prepareInputs(std::vector<detection_with_class> input);
    QVector<QVector<float>> sortObjects(QVector<QVector<float>> arr);
    int makeIndex(QVector<QVector<float>> arr, int i);
    QVector<QVector<float>> moveDuplicates(QVector<QVector<float>> arr, int start, int end);
    QVector<QVector<float>> sortDuplicates(QVector<QVector<float>> arr);

    QVector<float> diffInspection(QVector<float> inputs, float* outputs);
    void addInspection(QVector<float>);
    QVector<float> averageInspection();
};

#endif // OBJECTPOSITIONCHECKERDISPATCHER_H
