#include "ObjectPositionCheckerDispatcher.h"
#include "../../GenericHolder.h"
#include "../../GlobalVariables.h"

ObjectPositionCheckerDispatcher::ObjectPositionCheckerDispatcher(GenericHolder<std::vector<detection_with_class>>* holder) :
    detectionHolder(holder),
    inspectionTally(0)
{

}

void ObjectPositionCheckerDispatcher::run()
{
    isInspecting = true;
    QTimer timer; // consider removing timer
    timer.setSingleShot(true);
    QEventLoop loop;
    outputDiffTotal.fill(0);
    inspectionTally = 0;

    detectionHolder->connectSet(&loop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

    while (isInspecting) {
        timer.start(100);

        loop.exec();

        if (timer.isActive()) {
            QVector<float> inputs = prepareInputs(detectionHolder->get());
            auto inputTensor = tf_utils::CreateTensor(TF_FLOAT, inputDims, inputs.toStdVector());
            TF_Tensor* outputTensor = nullptr;
            TF_SessionRun(session,
                          nullptr, // Run options
                          inputLayer, &inputTensor, 1, // Number of inputs
                          outputLayer, &outputTensor, 1, // Number of outputs
                          nullptr, 0, // Target operations, number of targets
                          nullptr, // Run metadata
                          status);
            auto data = static_cast<float*>(TF_TensorData(outputTensor));
            addInspection(diffInspection(inputs, data));
        }
    }
    emit inspectionFinished(averageInspection());
}

void ObjectPositionCheckerDispatcher::stop()
{
    isInspecting = false;
}

QVector<float> ObjectPositionCheckerDispatcher::diffInspection(QVector<float> inputs, float* outputs)
{
    QVector<float> outputDebug;
    QVector<float> diff(inputs.size(), 0);
    for (int i = 0; i < inputs.size(); ++i) {
        diff[i] = inputs[i] - outputs[i];
        outputDebug.push_back(outputs[i]);
    }
    return diff;
}

void ObjectPositionCheckerDispatcher::addInspection(QVector<float> data)
{
    for (int i = 0; i < outputDiffTotal.size(); ++i) {
        outputDiffTotal[i] += data[i];
    }
    inspectionTally++;
}

QVector<float> ObjectPositionCheckerDispatcher::averageInspection()
{
    QVector<float> avg(outputDiffTotal.size(), 0);
    for (int i = 0; i < outputDiffTotal.size(); ++i) {
        avg[i] = outputDiffTotal[i] / inspectionTally;
    }
    return avg;
}

void ObjectPositionCheckerDispatcher::setTensorFlowInformation(TensorFlowInformation info)
{
    session = info.session;
    graph = info.graph;
    inputLayer = info.inputLayer;
    outputLayer = info.outputLayer;
    status = info.status;
    inputDims = info.inputDims;
    outputDiffTotal.resize(info.inputDims[1]);
}

QVector<float> ObjectPositionCheckerDispatcher::prepareInputs(std::vector<detection_with_class> input)
{
    QVector<QVector<float>> detCenters;
    for (ulong i = 0; i < input.size(); ++i) {
        QVector<float> temp;
        box bbox = input[i].det.bbox;
        temp.push_back(input[i].best_class);
        temp.push_back(bbox.x);
        temp.push_back(bbox.y);

        detCenters.push_back(temp);
    }
    detCenters = sortObjects(detCenters);
    detCenters = sortDuplicates(detCenters);

    QVector<float> kerasInput;
    for (int i = 0; i < detCenters.size(); ++i) {
        kerasInput.push_back(detCenters[i][1]);
        kerasInput.push_back(detCenters[i][2]);
    }

    return kerasInput;
}

QVector<QVector<float>> ObjectPositionCheckerDispatcher::sortObjects(QVector<QVector<float>> arr)
{
    int i = 1;
    int j = 0;
    while(i < arr.size()){
        j = i;
        while(j > 0 && arr[j - 1][0] > arr[j][0]){
            auto temp = arr[j];
            arr[j] = arr[j - 1];
            arr[j - 1] = temp;
            j -= 1;
        }
        i += 1;
    }
    return arr;
}

int ObjectPositionCheckerDispatcher::makeIndex(QVector<QVector<float>> arr, int i)
{
    if(abs(arr[i - 1][2] - arr[i][2]) > abs(arr[i - 1][1] - arr[i][1]))
        return 2;
    return 1;
}

QVector<QVector<float>> ObjectPositionCheckerDispatcher::moveDuplicates(QVector<QVector<float>> arr, int start, int end)
{
    int i = start + 1;
    int j = 0;
    while(i < end){
        j = i;
        int index = makeIndex(arr, j);
        while(j > start && arr[j - 1][index] > arr[j][index]){
            index = makeIndex(arr, j);
            auto temp = arr[j];
            arr[j] = arr[j - 1];
            arr[j - 1] = temp;
            j -= 1;
        }
        i += 1;
    }
    return arr;
}

QVector<QVector<float>> ObjectPositionCheckerDispatcher::sortDuplicates(QVector<QVector<float>> arr)
{
    int start = -100;
    for(int i = 0; i < arr.size(); i++){
        if(i == 0)
            continue;
        if(arr[i][0] == arr[i - 1][0]){
            if(start < 0)
                start = i - 1;
            else if(i == arr.size() - 1)
                arr = moveDuplicates(arr, start, i + 1);
        }
        else if(start > -1){
            arr = moveDuplicates(arr, start, i);
            start = -100;
        }
    }
    return arr;
}
