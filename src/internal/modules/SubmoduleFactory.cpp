#include "SubmoduleFactory.h"
#include "ObjectDetectionSubmodule/ObjectDetectionSubmodule.h"
#include "ObjectPositionCheckerSubmodule/ObjectPositionCheckerSubmodule.h"

SubmoduleFactory::SubmoduleFactory(QString classList)
{
    QStringList list = classList.split("|");

    if (list.contains("ObjectDetectionSubmodule"))
        map["ObjectDetectionSubmodule"] = createInstance<ObjectDetectionSubmodule>();
    if (list.contains("ObjectPositionCheckerSubmodule"))
        map["ObjectPositionCheckerSubmodule"] = createInstance<ObjectPositionCheckerSubmodule>();
}

DarknetSubmodule* SubmoduleFactory::getSubmodule(QString className)
{
    if (map.contains(className))
        return map[className];
    qDebug() << "Module not found: " << className;
    return nullptr;
}
