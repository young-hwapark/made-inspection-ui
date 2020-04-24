#include "TestObjectDetectionSubmodule.h"
#include "ObjectDetectionSubmodule.h"
#include "../SubmoduleFactory.h"
#include "../../GenericHolder.h"

TestObjectDetectionSubmodule::TestObjectDetectionSubmodule()
{
    {
        SubmoduleFactory factory("ObjectDetectionSubmodule");
        DarknetSubmodule* testModule = factory.getSubmodule("ObjectDetectionSubmodule");
        GenericHolder<std::vector<detection_with_class>>* holder = new GenericHolder<std::vector<detection_with_class>>;
        DarknetDispatcher* dispatcher = new DarknetDispatcher(nullptr);

        QMap<int, int> expectedObjects;
        expectedObjects[0] = 1;
        expectedObjects[1] = 1;

        ModelInformation info;
        info.expectedObjects = expectedObjects;
        info.objectDifferenceThreshold = 0.7;

        char person[] = "person";
        char car[] = "car";
        char* names[2] = {person, car};

        dispatcher->setDetectionHolder(holder);
        dispatcher->setNetwork({
                                   2,
                                   names,
                                   nullptr
                               });
        testModule->setInfo(dispatcher, info);

        { // TEST 1
            testModule->startInspection();

            std::vector<detection_with_class> vec;
            detection_with_class classDet;
            detection det;
            classDet.det = det;
            classDet.best_class = 0;

            vec.push_back(classDet);

            holder->set(vec);
            testModule->stopInspection();

            qDebug() << testModule->getResult() << testModule->status();
        }

        { // TEST 2
            testModule->startInspection();

            std::vector<detection_with_class> vec;
            detection_with_class classDet1;
            detection det1;
            classDet1.det = det1;
            classDet1.best_class = 0;

            detection_with_class classDet2;
            detection det2;
            classDet2.det = det2;
            classDet2.best_class = 1;

            vec.push_back(classDet1);
            vec.push_back(classDet2);

            holder->set(vec);
            testModule->stopInspection();

            qDebug() << testModule->getResult() << testModule->status();
        }

        { // TEST 3
            testModule->startInspection();

            std::vector<detection_with_class> vec1;
            std::vector<detection_with_class> vec2;
            detection_with_class classDet1;
            detection det1;
            classDet1.det = det1;
            classDet1.best_class = 0;

            detection_with_class classDet2;
            detection det2;
            classDet2.det = det2;
            classDet2.best_class = 1;

            vec1.push_back(classDet1);
            vec1.push_back(classDet2);

            vec2.push_back(classDet1);

            for (int i = 0; i < 10; ++i) {
                holder->set(vec2);
            }
            holder->set(vec1);

            testModule->stopInspection();

            qDebug() << testModule->getResult() << testModule->status();
        }
    }

    {
        SubmoduleFactory factory("ObjectDetectionSubmodule");
        DarknetSubmodule* testModule = factory.getSubmodule("ObjectDetectionSubmodule");
        GenericHolder<std::vector<detection_with_class>>* holder = new GenericHolder<std::vector<detection_with_class>>;
        DarknetDispatcher* dispatcher = new DarknetDispatcher(nullptr);

        QMap<int, int> expectedObjects;
        expectedObjects[0] = 4;
        expectedObjects[1] = 4;

        ModelInformation info;
        info.expectedObjects = expectedObjects;
        info.objectDifferenceThreshold = 0.7;

        char person[] = "person";
        char car[] = "car";
        char* names[2] = {person, car};

        dispatcher->setDetectionHolder(holder);
        dispatcher->setNetwork({
                                   2,
                                   names,
                                   nullptr
                               });
        testModule->setInfo(dispatcher, info);

        { // TEST 4
            testModule->startInspection();

            std::vector<detection_with_class> vec1;
            std::vector<detection_with_class> vec2;
            detection_with_class classDet1;
            detection det1;
            classDet1.det = det1;
            classDet1.best_class = 0;

            detection_with_class classDet2;
            detection det2;
            classDet2.det = det2;
            classDet2.best_class = 1;

            vec1.push_back(classDet1);
            vec1.push_back(classDet1);
            vec1.push_back(classDet1);
            vec1.push_back(classDet2);

            vec2.push_back(classDet1);
            vec2.push_back(classDet1);
            vec2.push_back(classDet1);
            vec2.push_back(classDet1);
            vec2.push_back(classDet2);
            vec2.push_back(classDet2);
            vec2.push_back(classDet2);
            vec2.push_back(classDet2);

            for (int i = 0; i < 10; ++i) {
                holder->set(vec1);
            }
            holder->set(vec2);

            testModule->stopInspection();

            qDebug() << testModule->getResult() << testModule->status();
        }

        { // TEST 5
            testModule->startInspection();

            std::vector<detection_with_class> vec1;
            std::vector<detection_with_class> vec2;
            detection_with_class classDet1;
            detection det1;
            classDet1.det = det1;
            classDet1.best_class = 0;

            detection_with_class classDet2;
            detection det2;
            classDet2.det = det2;
            classDet2.best_class = 1;

            vec1.push_back(classDet1);
            vec1.push_back(classDet1);
            vec1.push_back(classDet1);
            vec1.push_back(classDet1);
            vec1.push_back(classDet1);
            vec1.push_back(classDet2);

            vec2.push_back(classDet1);
            vec2.push_back(classDet1);
            vec2.push_back(classDet1);
            vec2.push_back(classDet1);
            vec2.push_back(classDet2);
            vec2.push_back(classDet2);
            vec2.push_back(classDet2);
            vec2.push_back(classDet2);

            for (int i = 0; i < 10; ++i) {
                holder->set(vec1);
            }
            holder->set(vec2);

            testModule->stopInspection();

            qDebug() << testModule->getResult() << testModule->status();
        }
    }
}
