#include "gameengine/Cube.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/IObjectFactory.hpp"
#include "gameengine/Primitives/Quad.hpp"
#include "gameengine/Components/TransformComponent.hpp"

#include "CUL/ThreadUtils.hpp"

using namespace LOGLW;

Cube::Cube( Camera* camera, IGameEngine* engine ) : IObject( engine ), m_camera( camera ), m_engine( engine )
{
    m_wallsPositions[0] = ITransformable::Pos( 0.f, 0.f, 1.f );
    // m_rotations[0].roll.setValue( 90.f, CUL ::MATH::Angle::Type::DEGREE );

    m_wallsPositions[1] = ITransformable::Pos( 0.f, 0.f, -1.f );
    // m_rotations[1].roll.setValue( 90.f, CUL ::MATH::Angle::Type::DEGREE );

    m_wallsPositions[2] = ITransformable::Pos( -1.f, 0.f, 0.f );
    m_rotations[2].pitch.setValue( 90.f, CUL ::MATH::Angle::Type::DEGREE );

    m_wallsPositions[3] = ITransformable::Pos( 1.f, 0.f, 0.f );
    m_rotations[3].pitch.setValue( 90.f, CUL ::MATH::Angle::Type::DEGREE );

    m_wallsPositions[4] = ITransformable::Pos( 0.f, -1.f, 0.f );
    m_rotations[4].roll.setValue( 90.f, CUL ::MATH::Angle::Type::DEGREE );

    m_wallsPositions[5] = ITransformable::Pos( 0.f, 1.f, 0.f );
    m_rotations[5].roll.setValue( 90.f, CUL ::MATH::Angle::Type::DEGREE );

    if( getUtility()->getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        createPlaceHolders();
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
        LOGLW::Quad* quad = m_engine->createQuad( this );
        m_walls[i] = quad;
        quad->setDisableRenderOnMyOwn(true);
        addChild( quad );
        TransformComponent* transformCmp = static_cast<TransformComponent*>( quad->getComponent( "TransformComponent" ) );
        if( transformCmp )
        {
            transformCmp->setWorldPosition( m_wallsPositions[i] );
            transformCmp->setWorldRotation( m_rotations[i] );
        }
    }
}

void Cube::render()
{
    if( getUtility()->isLegacy() )
    {
        getUtility()->matrixStackPush();

        const auto position = getTransform()->getWorldPosition();
        const auto rotation = getTransform()->getWorldRotation();

        getUtility()->translate( position );

        getUtility()->rotate( rotation.yaw.getDeg(), 0.f, 0.f, 1.f );
        getUtility()->rotate( rotation.pitch.getDeg(), 0.f, 1.f, 0.f );
        getUtility()->rotate( rotation.roll.getDeg(), 1.f, 0.f, 0.f );
    }

    const auto children = getChildren();
    for( const auto child : children )
    {
        child->render();
    }

    if( getUtility()->isLegacy() )
    {
        getUtility()->matrixStackPop();
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