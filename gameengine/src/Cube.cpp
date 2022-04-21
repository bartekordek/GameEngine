#include "gameengine/Cube.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/IObjectFactory.hpp"

using namespace LOGLW;


Cube::Cube( Camera* camera, IGameEngine* engine ) : m_camera( camera ), m_engine( engine )
{
    //m_engine->getObjectFactory()->createSprite();
}

//void Cube::setImage(unsigned wallIndex, const CUL::FS::Path& imagePath, CUL::Graphics::IImageLoader* imageLoader)
void Cube::setImage(unsigned, const CUL::FS::Path&, CUL::Graphics::IImageLoader*)
{

}

void Cube::render()
{
    const auto children = getChildren();
    for (const auto child: children)
    {
        child->render();
    }

    // if( getUtility()->isLegacy() )
    // {
    //     renderLegacy();
    // }
    // else
    // {
    //     renderModern();
    // }
}

void Cube::renderModern()
{

}

void Cube::renderLegacy()
{

}

Cube::~Cube()
{

}