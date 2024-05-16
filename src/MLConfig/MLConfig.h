#pragma once
#include "mlconfig_global.h"

class MLCONFIG_EXPORT MLConfig
{
public:
    MLConfig();
    static MLConfig* instance();
private:
    static MLConfig* self;
};
