#ifndef TESTOBJECTPOSITIONCHECKERSUBMODULE_H
#define TESTOBJECTPOSITIONCHECKERSUBMODULE_H

#include <QObject>

class SubmoduleFactory;
class DarknetSubmodule;

class TestObjectPositionCheckerSubmodule : public QObject
{
    Q_OBJECT

public:
    TestObjectPositionCheckerSubmodule();

private:
    SubmoduleFactory* factory;
    DarknetSubmodule* testModule;

public slots:
    void checkModuleResult();
};

#endif // TESTOBJECTPOSITIONCHECKERSUBMODULE_H
