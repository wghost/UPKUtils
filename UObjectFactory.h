#ifndef UOBJECTFACTORY_H
#define UOBJECTFACTORY_H

#include "UObject.h"

/// object factory to create Unreal objects of specified type
class UObjectFactory
{
    public:
        UObjectFactory() {};
        ~UObjectFactory() {};
        static UObject*   Create(GlobalType Type);
        static UObject*   Create(std::string name);
        static GlobalType NameToType(std::string name);
};

#endif // UOBJECTFACTORY_H
