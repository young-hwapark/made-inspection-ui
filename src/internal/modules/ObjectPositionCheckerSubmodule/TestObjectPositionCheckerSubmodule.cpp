#include "TestObjectPositionCheckerSubmodule.h"
#include "../SubmoduleFactory.h"
#include "../../GenericHolder.h"

TestObjectPositionCheckerSubmodule::TestObjectPositionCheckerSubmodule()
{
    factory = new SubmoduleFactory("ObjectPositionCheckerSubmodule");
    testModule = factory->getSubmodule("ObjectPositionCheckerSubmodule");
    GenericHolder<std::vector<detection_with_class>>* holder = new GenericHolder<std::vector<detection_with_class>>;
    DarknetDispatcher* dispatcher = new DarknetDispatcher(nullptr);

    char person[] = "person";
    char car[] = "car";
    char* names[2] = {person, car};

    ModelInformation info;
    info.modelID = "AETEST";
    info.totalNumObj = 8;

    dispatcher->setDetectionHolder(holder);
    dispatcher->setNetwork({
                               2,
                               names,
                               nullptr
                           });
    testModule->setInfo(dispatcher, info);

    std::vector<detection_with_class> vec;
    detection_with_class detclass;
    detclass.det.bbox.x = 0.2;
    detclass.det.bbox.y = 0.8;
    for(int i = 0; i < 8; ++i)
        vec.push_back(detclass);

    connect(testModule, SIGNAL(newResult()), this, SLOT(checkModuleResult()));
    QEventLoop loop;
    connect(testModule, SIGNAL(moduleReady()), &loop, SLOT(quit()));
    loop.exec();

    testModule->startInspection();
    QThread::msleep(300); // Simulate delay in getting first detection
    holder->set(vec);
    testModule->stopInspection();
}

void TestObjectPositionCheckerSubmodule::checkModuleResult()
{
    qDebug() << testModule->getResult() << testModule->status();
}
