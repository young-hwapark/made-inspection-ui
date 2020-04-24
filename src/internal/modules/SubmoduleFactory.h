#ifndef SUBMODULEFACTORY_H
#define SUBMODULEFACTORY_H

#include "DarknetSubmodule.h"

class SubmoduleFactory
{
public:
    SubmoduleFactory(QString classList);
    DarknetSubmodule* getSubmodule(QString className);

private:
    template<typename T> DarknetSubmodule* createInstance() { return new T; }
    typedef QMap<QString, DarknetSubmodule*> map_type;
    map_type map;
};

#endif // SUBMODULEFACTORY_H
