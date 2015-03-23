#ifndef UTOKENFACTORY_H
#define UTOKENFACTORY_H

#include "UToken.h"

class UTokenFactory
{
    public:
        UTokenFactory() {};
        ~UTokenFactory() {};
        static UScriptToken* Create(UToken Type);
};

#endif // UTOKENFACTORY_H
