#include "ObjectPositionCheckerLoader.h"
#include "../../GlobalVariables.h"

ObjectPositionCheckerLoader::ObjectPositionCheckerLoader(ModelInformation info) :
    modelInformation(info)
{

}

void ObjectPositionCheckerLoader::run()
{
    inputLayer = new TF_Output;
    outputLayer = new TF_Output;

    graph = tf_utils::LoadGraph((GlobalVariables::dataDir + "/" + modelInformation.modelID + "/" + modelInformation.modelID + ".pb").toLocal8Bit());
    if (graph == nullptr) {
        qDebug() << "Could not load graph.";
        return;
    }

    *inputLayer = TF_Output{TF_GraphOperationByName(graph, "input_1"), 0}; // Get input layer name from some other source
    if (inputLayer->oper == nullptr) {
        qDebug() << "Could not find input layer.";
        return;
    }

    *outputLayer = TF_Output{TF_GraphOperationByName(graph, "out/BiasAdd"), 0}; // Get output layer name from some other source
    if (outputLayer->oper == nullptr) {
        qDebug() << "Could not find output layer.";
        return;
    }

    status = TF_NewStatus();
    TF_SessionOptions* options = tf_utils::CreateSessionOptions(0.1, nullptr); // Set gpu fraction
    session = TF_NewSession(graph, options, status);
    TF_DeleteSessionOptions(options);

    if (TF_GetCode(status) != TF_OK) {
        qDebug() << "Error running Tensorflow Session.";
        return;
    }

    const std::vector<int64_t> input_dims = {1, modelInformation.totalNumObj * 2};

    // Flush network once
    std::vector<float> inputs(modelInformation.totalNumObj * 2, 0.5f);
    auto inputTensor = tf_utils::CreateTensor(TF_FLOAT, input_dims, inputs);
    TF_Tensor* outputTensor = nullptr;
    TF_SessionRun(session,
                  nullptr,
                  inputLayer, &inputTensor, 1,
                  outputLayer, &outputTensor, 1,
                  nullptr, 0,
                  nullptr,
                  status);
    auto data = static_cast<float*>(TF_TensorData(outputTensor));

    tensorFlowInformation = {session,
                            graph,
                            inputLayer,
                            outputLayer,
                            status,
                            input_dims};
    emit loadSuccess();
}

TensorFlowInformation ObjectPositionCheckerLoader::getTensorFlowInfo()
{
    return tensorFlowInformation;
}
