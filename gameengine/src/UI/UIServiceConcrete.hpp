#pragma once

#include "gameengine/UI/UIService.hpp"

namespace LOGLW
{
class IGameEngine;

class CUIServiceConcrete: public IUIService
{
public:
    explicit CUIServiceConcrete( IGameEngine& inEngine );

    IWidget* createWidget();

    ~CUIServiceConcrete();

    CUL_NONCOPYABLE( CUIServiceConcrete )
protected:
private:
    IGameEngine& m_engine;
};
}  // namespace LOGLW