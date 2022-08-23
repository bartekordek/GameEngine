#include "MatrixStack.hpp"
#include "ImportFreeglut.hpp"

using namespace LOGLW;

MatrixStack::MatrixStack()
{
}

void MatrixStack::push()
{
    glPushMatrix();
    ++m_currentMatrix;
}

void MatrixStack::pop()
{
    glPopMatrix();
    --m_currentMatrix;
}

MatrixStack::~MatrixStack()
{
}