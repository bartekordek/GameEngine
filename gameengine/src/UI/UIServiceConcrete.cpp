#include "UI/UIServiceConcrete.hpp"
#include "UI/WidgetTexture.hpp"

namespace LOGLW
{
CUIServiceConcrete::CUIServiceConcrete( IGameEngine& inEngine ):
    m_engine( inEngine )
{
}

IWidget* CUIServiceConcrete::createWidget()
{
    CWidgetTexture* result = new CWidgetTexture();
    return result;
}

CUIServiceConcrete::~CUIServiceConcrete()
{
}
}  // namespace LOGLW