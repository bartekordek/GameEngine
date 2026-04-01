#pragma once

#include "gameengine/UI/Widget.hpp"

namespace LOGLW
{
class CWidgetTexture: public IWidget
{
public:
    CWidgetTexture();
    void render();
    ~CWidgetTexture();

    CUL_NONCOPYABLE( CWidgetTexture )
protected:
private:
};
}  // namespace LOGLW