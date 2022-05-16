#include "gameengine/Cube.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/IObjectFactory.hpp"
#include "gameengine/Primitives/Quad.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/Components/Name.hpp"

#include "CUL/Threading/ThreadUtils.hpp"

using namespace LOGLW;

Cube::Cube( Camera* camera, IGameEngine* engine ) : IObject( engine ), m_camera( camera ), m_engine( engine )
{
    m_transformComponent = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
    m_transformComponent->setPivot( { 1.f, 0.f, 0.f } );

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
    TransformComponent::Pos quadSize(2.f, 2.f, 0.f);
    TransformComponent::Pos pivot;
    // 0
    {
        LOGLW::Quad* quad = m_engine->createQuad( this );
        quad->setDisableRenderOnMyOwn( true );
        addChild( quad );
        TransformComponent* transformCmp = quad->getTransform();
        transformCmp->setWorldPosition( ITransformable::Pos( 0.f, 0.f, 1.f ) );
        transformCmp->setSize( quadSize );
        transformCmp->setPivot( pivot );
        quad->getnameCmp()->setName( "Wall00" );
    }

    // 1
    {
        LOGLW::Quad* quad = m_engine->createQuad( this );
        quad->setDisableRenderOnMyOwn( true );
        addChild( quad );
        TransformComponent* transformCmp = quad->getTransform();
        transformCmp->setWorldPosition( ITransformable::Pos( 0.f, 0.f, -1.f ) );
        transformCmp->setSize( quadSize );
        transformCmp->setPivot( pivot );
        quad->getnameCmp()->setName( "Wall01" );
    }

    // 2
    {
        LOGLW::Quad* quad = m_engine->createQuad( this );
        quad->setDisableRenderOnMyOwn( true );
        addChild( quad );
        TransformComponent* transformCmp = quad->getTransform();
        transformCmp->setWorldPosition( ITransformable::Pos( -1.f, 0.f, 0.f ) );
        transformCmp->setSize( quadSize );
        transformCmp->setPivot( pivot );
        CUL::MATH::Rotation rotation;
        rotation.yaw.setValue( 90.f, CUL ::MATH::Angle::Type::DEGREE );
        transformCmp->setWorldRotation( rotation );
        quad->getnameCmp()->setName( "Wall02" );
    }

    // 3
    {
        LOGLW::Quad* quad = m_engine->createQuad( this );
        quad->setDisableRenderOnMyOwn( true );
        addChild( quad );
        TransformComponent* transformCmp = quad->getTransform();
        transformCmp->setWorldPosition( ITransformable::Pos( 1.f, 0.f, 0.f ) );
        transformCmp->setSize( quadSize );
        transformCmp->setPivot( pivot );
        CUL::MATH::Rotation rotation;
        rotation.yaw.setValue( 90.f, CUL ::MATH::Angle::Type::DEGREE );
        transformCmp->setWorldRotation( rotation );
        quad->getnameCmp()->setName( "Wall03" );
    }

    // 4
    {
        LOGLW::Quad* quad = m_engine->createQuad( this );
        quad->setDisableRenderOnMyOwn( true );
        addChild( quad );
        TransformComponent* transformCmp = quad->getTransform();
        transformCmp->setWorldPosition( ITransformable::Pos( 0.f, -1.f, 0.f ) );
        transformCmp->setSize( quadSize );
        transformCmp->setPivot( pivot );
        CUL::MATH::Rotation rotation;
        rotation.pitch.setValue( 90.f, CUL ::MATH::Angle::Type::DEGREE );
        transformCmp->setWorldRotation( rotation );
        quad->getnameCmp()->setName( "Wall04" );
    }

    // 5
    {
        LOGLW::Quad* quad = m_engine->createQuad( this );
        quad->setDisableRenderOnMyOwn( true );
        addChild( quad );
        TransformComponent* transformCmp = quad->getTransform();
        transformCmp->setWorldPosition( ITransformable::Pos( 0.f, 1.f, 0.f ) );
        transformCmp->setSize( quadSize );
        transformCmp->setPivot( pivot );
        CUL::MATH::Rotation rotation;
        rotation.pitch.setValue( 90.f, CUL ::MATH::Angle::Type::DEGREE );
        transformCmp->setWorldRotation( rotation );
        quad->getnameCmp()->setName( "Wall05" );
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
        getUtility()->rotate( rotation );
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