#include "gameengine/UI/Widget.hpp"

namespace LOGLW
{
IWidget::IWidget():
    IObject( CUL_STR( "IWidget" ), false )
{

}

IWidget::~IWidget()
{
}
}