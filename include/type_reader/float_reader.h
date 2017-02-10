#pragma once

#include "type_reader/type_reader.h"

class FloatTypeReader : public ITypeReader
{
    public:

    void 
    fill_type(
        GParamSpec * const pspec,
        GValue * const value,
        param_keys& keys);
};