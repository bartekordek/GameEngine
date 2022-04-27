#include "gameengine/Cube.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/IObjectFactory.hpp"
#include "gameengine/Primitives/Quad.hpp"

#include "CUL/ThreadUtils.hpp"

using namespace LOGLW;

Cube::Cube( Camera* camera, IGameEngine* engine ) : m_camera( camera ), m_engine( engine )
{
    m_wallsPositions[0] = ITransformable::Pos( 0.f, 0.f, 1.f );
    //m_rotations[0].roll.setValue( 90.f, CUL ::MATH::Angle::Type::DEGREE );

    m_wallsPositions[1] = ITransformable::Pos( 0.f, 0.f, -1.f );
    //m_rotations[1].roll.setValue( 90.f, CUL ::MATH::Angle::Type::DEGREE );

    m_wallsPositions[2] = ITransformable::Pos( -1.f, 0.f, 0.f );
    m_rotations[2].yaw.setValue( 90.f, CUL ::MATH::Angle::Type::DEGREE );

    m_wallsPositions[3] = ITransformable::Pos( 1.f, 0.f, 0.f );
    m_rotations[3].yaw.setValue( 90.f, CUL ::MATH::Angle::Type::DEGREE );

    m_wallsPositions[4] = ITransformable::Pos( 0.f, -1.f, 0.f );
    m_rotations[4].pitch.setValue( 90.f, CUL ::MATH::Angle::Type::DEGREE );

    m_wallsPositions[5] = ITransformable::Pos( 0.f, 1.f, 0.f );
    m_rotations[5].pitch.setValue( 90.f, CUL ::MATH::Angle::Type::DEGREE );

    if( getUtility()->getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        createPlaceHolders();
        m_engine->addObjectToRender( this );
    }
    else
    {
        m_engine->pushPreRenderTask(
            [this]()
            {
                createPlaceHolders();
                m_engine->addObjectToRender( this );
            } );
    }
}

// void Cube::setImage(unsigned wallIndex, const CUL::FS::Path& imagePath, CUL::Graphics::IImageLoader* imageLoader)
void Cube::setImage( unsigned, const CUL::FS::Path&, CUL::Graphics::IImageLoader* )
{
}

void Cube::createPlaceHolders()
{
    for( size_t i = 0; i < 6; ++i )
    {
        m_walls[i] = m_engine->createQuad();
        m_walls[i]->setWorldPosition( m_wallsPositions[i] );
        m_walls[i]->setWorldRotation( m_rotations[i] );
    }
}

void Cube::render()
{
    const auto children = getChildren();
    for( const auto child : children )
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
    m_engine->removeObjectToRender( this );
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