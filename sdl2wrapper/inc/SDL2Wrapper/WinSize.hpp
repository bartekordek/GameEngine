#pragma once

#include "SDL2Wrapper/Import.hpp"

NAMESPACE_BEGIN( SDL2W )

struct SDL2WAPI WinSize
{
    int w = 800;
    int h = 600;

    void setSize( int iW, int iH )
    {
        w = iW;
        h = iH;
    }

    int getWidth() const
    {
        return w;
    }

    int getHeight() const
    {
        return h;
    }
};

NAMESPACE_END( SDL2W )