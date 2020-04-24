#include "ModelSelector.h"
#include "../internal/PylonCamera.h"
#include "../internal/utils/RestClient.h"
#include "../internal/GlobalVariables.h"
#include "../internal/ModelInformation.h"
#include <pylon/PylonIncludes.h>

using namespace Pylon;

ModelSelector::ModelSelector() :
    QDialog(),
    modelSelected(false)
{
    createRight();

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(rightGroupBox);

    setWindowTitle("Choose a Basic Inspection Model");
}

void ModelSelector::accept()
{
    acceptButton->setEnabled(false);
    cancelButton->setEnabled(false);
    QString modelID = modelCombo->currentText();

    if (modelCombo->currentText().startsWith("--")) {
        QMessageBox msg(QMessageBox::Warning, "Warning", "Please select a model");
        msg.exec();
    } else {
        if(checkFiles()) {
            emit openModel(modelMap[currentModelID]);
            QDialog::accept();
        }
    }
    acceptButton->setEnabled(true);
    cancelButton->setEnabled(true);
}

void ModelSelector::reject()
{
    QDialog::reject();
}

void ModelSelector::createRight()
{
    rightGroupBox = new QGroupBox("Model");
    rightLayout = new QVBoxLayout(rightGroupBox);
    rightForm = new QWidget;
    rightFormLayout = new QFormLayout(rightForm);
    rightButtonBox = new QWidget;
    rightButtonBoxLayout = new QHBoxLayout(rightButtonBox);

    modelCombo = new QComboBox;
    modelCombo->addItem("-- Please select a model --");
    currentModelID = "-- Please select a model --";

    QUrlQuery q;
    q.addQueryItem("customerID", GlobalVariables::customerID);

    RestClient* rest = new RestClient;
    std::pair<int, QJsonDocument>result = rest->reqGet("/models", q, GlobalVariables::token, GlobalVariables::customerID);
    int httpStatus = result.first;
    QJsonDocument json = result.second;

    if (httpStatus != 200) {
        QMessageBox msg;
        msg.setText("Could not retrieve model information.");
        msg.exec();
        return;
    }

    // test code

    QString jsonStr;
    QFile file;
    file.setFileName("meta_example.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    jsonStr = "[" + file.readAll() + "]";
    file.close();
    json = QJsonDocument::fromJson(jsonStr.toUtf8());

    // end of test code

    QJsonArray items = json.array();

    for (QJsonArray::const_iterator i = items.begin(); i != items.end(); ++i) {
        QJsonValue val = i[0];

        ModelInformation* model = new ModelInformation;
        model->modelID = val["SK"].toString();
        model->modelName = val["name"].toString();
        model->modelType = val["modelType"].toString();
        model->desc = val["desc"].toString();
        model->status = val["status"].toString();
        model->lastUpdated = val["lastUpdatedOn"].toString();
        model->modelIP = val["cameraIP"].toString();

        QJsonArray submoduleInfo = val["submodules"].toArray();
        for (QJsonArray::const_iterator j = submoduleInfo.begin(); j != submoduleInfo.end(); ++j) {
            model->submoduleMap[j[0].toObject()["name"].toString()] = j[0];
        }

        if (val["modelType"].toString() == QString("REMOTE")){
            model->destIP = val["destIP"].toString();
            model->nextModel = val["nextModel"].toString();
            model->sequence = QString::number(val["seq"].toDouble());
        }

        QUrlQuery q;
        q.addQueryItem("customerID", GlobalVariables::customerID);

        RestClient* rest = new RestClient;
        std::pair<int, QJsonDocument>result = rest->reqGet("/models/" + model->modelID, q, GlobalVariables::token, GlobalVariables::customerID);
        int httpStatus = result.first;
        QJsonDocument json = result.second;

        if (httpStatus != 200) {
            QMessageBox msg;
            msg.setText("Could not retrieve subpart information.");
            msg.exec();
            return;
        }

        QJsonObject items = json.object();

        if (items.isEmpty()) {
            QMessageBox msg(QMessageBox::Warning, "Warning", "Something wrong with subpart model information");
            msg.exec();
            return;
        }

        QJsonArray subpartArray = items["subparts"].toArray();

        for (QJsonArray::const_iterator i = subpartArray.begin(); i != subpartArray.end(); ++i) {
            QJsonValue val = i[0];
            QString yoloID = val["yoloID"].toString();
            int numObjects = val["numObjects"].toInt();

            SubpartModel* subpartModel = new SubpartModel;

            subpartModel->yoloID = yoloID;
            subpartModel->name = val["name"].toString();
            subpartModel->color = val["color"].toString();
            subpartModel->numObjects = numObjects;
            subpartModel->isPositive = val["isPositive"].toBool();

            model->totalNumObj += numObjects;
            model->expectedObjects[yoloID.toInt()] = numObjects; // remove later
            model->subpartMap[yoloID] = subpartModel;



        }
        modelMap[model->modelID] = model;
        modelCombo->addItem(model->modelName, model->modelID);
    }

    connect(modelCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(handleModelComboChanged(QString)));

    modelNameLineEdit = new QLineEdit("");
    descTextEdit = new QTextEdit("");
    statusLineEdit = new QLineEdit("");
    lastUpdatedLineEdit = new QLineEdit("");

    QWidget* formSpacer = new QWidget;
    rightFormLayout->addRow("Name", modelNameLineEdit);
    rightFormLayout->addRow("Description", descTextEdit);
    rightFormLayout->addRow("Status", statusLineEdit);
    rightFormLayout->addRow("Last Updated", lastUpdatedLineEdit);
    rightFormLayout->addRow(formSpacer);

    acceptButton = new QPushButton("Next");
    cancelButton = new QPushButton("Cancel");

    QWidget* buttonBox = new QWidget(rightGroupBox);
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonBox);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(acceptButton);
    buttonLayout->addWidget(cancelButton);

    connect(acceptButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    rightButtonBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    rightLayout->setAlignment(Qt::AlignTop);
    rightLayout->addWidget(modelCombo);
    rightLayout->addWidget(rightForm);
    rightLayout->addWidget(buttonBox);
}

bool ModelSelector::checkFiles()
{
    QString shortName = currentModelID.split("|").at(2);

    QDir dir(GlobalVariables::dataDir + "/" + shortName);

    if (!dir.exists()) {
        dir.mkpath(".");
        QMessageBox msg(QMessageBox::Warning, "Warning", "The data directory is missing.");
        msg.exec();
        return false;
    } else {
        if (!QFile(dir.path() + "/" + shortName + ".cfg").exists()) {
            qDebug() << dir.path() + "/" + shortName + ".cfg";
            QMessageBox msg(QMessageBox::Warning, "Warning", "The configuration file is missing.");
            msg.exec();
            return false;
        }
        if (!QFile(dir.path() + "/" + shortName + ".weights").exists()) {
            QMessageBox msg(QMessageBox::Warning, "Warning", "The weight file is missing.");
            msg.exec();
            return false;
        }
        if (!QFile(dir.path() + "/" + shortName + ".names").exists()) {
            QMessageBox msg(QMessageBox::Warning, "Warning", "The name file is missing.");
            msg.exec();
            return false;
        }
        if (!QFile(dir.path() + "/" + shortName + ".data").exists()) {
            QMessageBox msg(QMessageBox::Warning, "Warning", "The data file is missing.");
            msg.exec();
            return false;
        }
    }
    return true;
}

void ModelSelector::handleModelComboChanged(QString comboString)
{
    currentModelID =  modelCombo->currentData().toString();
    currentModel = modelMap[currentModelID];

    QString name = currentModel->modelName;
    QString desc = currentModel->desc;
    QString status = currentModel->status;
    QString lastUpdated = currentModel->lastUpdated;

    if (comboString.startsWith("--")) {
        currentModelID = "-- Please select a model --";
        name = "";
        desc = "";
        status = "";
        lastUpdated = "";
    }

    modelNameLineEdit->setText(name);
    descTextEdit->setText(desc);
    statusLineEdit->setText(status);
    lastUpdatedLineEdit->setText(lastUpdated);
}
