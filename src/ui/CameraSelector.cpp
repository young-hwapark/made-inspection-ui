#include "CameraSelector.h"
#include "../internal/PylonCamera.h"
#include <pylon/PylonIncludes.h>

using namespace Pylon;

CameraSelector::CameraSelector() :
    QDialog(),
    cameraIp("--")
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    createForms();

    QHBoxLayout* hLayout = new QHBoxLayout(this);
    hLayout->addWidget(groupBox);
    setWindowTitle("Choose a camera IP");
}

void CameraSelector::accept()
{
    QString cameraID = cameraCombo->currentText();

    if (cameraID.startsWith("--")) {
        QMessageBox msg(QMessageBox::Warning, "Warning", "Please select a camera");
        msg.exec();
    } else {
        cameraIp = cameraID.split(":").value(0);
        emit openTrainingWindow(cameraIp);
        QDialog::accept();
    }
}

void CameraSelector::reject()
{
    QDialog::reject();
}

void CameraSelector::createForms()
{
    groupBox = new QGroupBox("Camera", this);
    layout = new QVBoxLayout(groupBox);
    groupBox->setMinimumWidth(400);
    cameraCombo = new QComboBox;
    updateCameraList();


    QWidget* cameraComboWidget = new QWidget(this);
    QPushButton* refreshButton = new QPushButton;
    QIcon refreshIcon = QIcon::fromTheme("view-refresh");
    refreshButton->setIcon(refreshIcon);
    cameraCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    refreshButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    QHBoxLayout* cameraButtonLayout = new QHBoxLayout(cameraComboWidget);
    cameraButtonLayout->addWidget(cameraCombo);
    cameraButtonLayout->addWidget(refreshButton);

    connect(cameraCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(handleCameraComboChanged(QString)));
    connect(refreshButton, SIGNAL(clicked()), this, SLOT(updateCameraList()));

    ipLineEdit = new QLineEdit("");
    modelLineEdit = new QLineEdit("");
    serialLineEdit = new QLineEdit("");
    versionLineEdit = new QLineEdit("");
    userNameLineEdit = new QLineEdit("");
    friendlyNameLineEdit = new QLineEdit("");
    vendorLineEdit = new QLineEdit("");
    deviceClassLineEdit = new QLineEdit("");

    QWidget* leftForm = new QWidget(groupBox);
    QFormLayout* leftFormLayout = new QFormLayout(leftForm);
    leftFormLayout->addRow("IP Address", ipLineEdit);
    leftFormLayout->addRow("Model", modelLineEdit);
    leftFormLayout->addRow("Serial #", serialLineEdit);
    leftFormLayout->addRow("Version", versionLineEdit);
    leftFormLayout->addRow("Username", userNameLineEdit);
    leftFormLayout->addRow("Friendly Name", friendlyNameLineEdit);
    leftFormLayout->addRow("Vendor", vendorLineEdit);
    leftFormLayout->addRow("Device Class", deviceClassLineEdit);

    acceptButton = new QPushButton("Next");
    cancelButton = new QPushButton("Cancel");

    QWidget* buttonBox = new QWidget(groupBox);
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonBox);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(acceptButton);
    buttonLayout->addWidget(cancelButton);

    connect(acceptButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(cameraComboWidget);
    layout->addWidget(leftForm);
    layout->addWidget(buttonBox);
}

void CameraSelector::handleCameraComboChanged(const QString ip)
{
    QList<QString> list = cameraList[ip];

    QString model = list.value(1, "");
    QString serial = list.value(2, "");
    QString version = list.value(3, "");
    QString userName = list.value(4, "");
    QString friendlyName = list.value(5, "");
    QString vendor = list.value(6, "");
    QString deviceClass = list.value(7, "");

    if (!ip.startsWith("--")){
        ipLineEdit->setText(ip);
    }
    else{
        ipLineEdit->setText("");
    }

    modelLineEdit->setText(model);
    serialLineEdit->setText(serial);
    versionLineEdit->setText(version);
    userNameLineEdit->setText(userName);
    friendlyNameLineEdit->setText(friendlyName);
    vendorLineEdit->setText(vendor);
    deviceClassLineEdit->setText(deviceClass);
}

void CameraSelector::updateCameraList()
{
    PylonInitialize();
    QString ip;
    cameraCombo->clear();
    cameraCombo->addItem("-- Please select a camera --");

    try {
        PylonCamera pylonCamera;
        DeviceInfoList_t deviceList = pylonCamera.getDeviceList();

        if (!deviceList.empty()) {
            DeviceInfoList_t::const_iterator it;

            for (it = deviceList.begin(); it != deviceList.end(); it++) {
                QString fullName = QString(it->GetFullName().c_str());
                QStringList strList = fullName.split(QRegExp("#"));

                ip = strList.at(2);

                QList<QString> list;

                list.append(ip);
                list.append(it->GetModelName().c_str());        // 0
                list.append(it->GetSerialNumber().c_str());     // 1
                list.append(it->GetDeviceVersion().c_str());    // 2
                list.append(it->GetUserDefinedName().c_str());  // 3
                list.append(it->GetFriendlyName().c_str());     // 4
                list.append(it->GetVendorName().c_str());       // 5
                list.append(it->GetDeviceClass().c_str());      // 6

                cameraList[ip] = list;
                cameraCombo->addItem(ip);
            }
        }
    } catch (GenericException &e) {
        qDebug() << "Caught generic exception: " << e.GetDescription();
    } catch ( ... ) {
        qDebug() << "Caught an unknown exception.";
    }
    PylonTerminate();
}
