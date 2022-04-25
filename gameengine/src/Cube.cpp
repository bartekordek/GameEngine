#include "gameengine/Cube.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/IObjectFactory.hpp"
#include "gameengine/Primitives/Quad.hpp"

using namespace LOGLW;

Cube::Cube( Camera* camera, IGameEngine* engine ) : m_camera( camera ), m_engine( engine )
{
    m_wallsPositions[0] = {  0.f,  0.f,  1.f };
    m_wallsPositions[1] = {  0.f,  0.f, -1.f };

    m_wallsPositions[2] = { -1.f,  0.f,  0.f };
    m_wallsPositions[3] = {  1.f,  0.f,  0.f };

    m_wallsPositions[4] = {  0.f, -1.f,  0.f };
    m_wallsPositions[5] = {  0.f,  1.f,  0.f };

    createPlaceHolders();
}

//void Cube::setImage(unsigned wallIndex, const CUL::FS::Path& imagePath, CUL::Graphics::IImageLoader* imageLoader)
void Cube::setImage(unsigned, const CUL::FS::Path&, CUL::Graphics::IImageLoader*)
{

}

void Cube::createPlaceHolders()
{
    for( size_t i = 0; i < 6; ++i )
    {
        m_walls[i] = m_engine->createQuad();
        m_walls[i]->setWorldPosition( m_wallsPositions[i] );
    }
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
    release();
}

void Cube::release()
{
    for( size_t i = 0; i < 6; ++i )
    {
        delete m_walls[i];
        m_walls[i] = nullptr;
    }
}