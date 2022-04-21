#pragma once

#include "gameengine/Import.hpp"
#include "CUL/GenericUtils/SimpleAssert.hpp"
#include "ImportFreeglut.hpp"

NAMESPACE_BEGIN( LOGLW )

class MatrixStack
{
public:
    MatrixStack()
    {
    }

    ~MatrixStack()
    {

    }

    void push()
    {
        glPushMatrix();
        ++m_currentMatrix;
    }

    void pop()
    {
        glPopMatrix();
        --m_currentMatrix;
    }

protected:
private:
    unsigned int m_currentMatrix = 0;
};

NAMESPACE_END( LOGLW )