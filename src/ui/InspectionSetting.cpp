#include "InspectionSetting.h"
#include "../internal/modules/ObjectDetectionSubmodule/ObjectDetectionSubmoduleSettings.h"


InspectionSetting::InspectionSetting( QWidget *parent)
    : QDialog(parent)
{
    ObjectDetectionSubmoduleSettings* settings1 = new ObjectDetectionSubmoduleSettings;
    ObjectDetectionSubmoduleSettings* settings2 = new ObjectDetectionSubmoduleSettings;


    tabWidget = new QTabWidget;
    tabWidget->addTab(settings1, "tab1");
    tabWidget->addTab(settings2, "tab2");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(tabWidget);

}
