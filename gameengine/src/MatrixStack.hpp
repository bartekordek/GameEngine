#pragma once

#include "gameengine/Import.hpp"

NAMESPACE_BEGIN( LOGLW )

class MatrixStack final
{
public:
    MatrixStack();
    void push();
    void pop();
    ~MatrixStack();

protected:
private:
    unsigned int m_currentMatrix = 0;
};

NAMESPACE_END( LOGLW )