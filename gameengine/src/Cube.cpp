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

    if( getUtility()->getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        createPlaceHolders();
        m_initialized = true;
    }
    else
    {
        m_engine->pushPreRenderTask(
            [this]()
            {
                createPlaceHolders();
                m_initialized = true;
                m_engine->addObjectToRender( this );
            } );
    }
}

// void Cube::setImage(unsigned wallIndex, const CUL::FS::Path& imagePath, CUL::Graphics::IImageLoader* imageLoader)
void Cube::setImage( unsigned, const CUL::FS::Path&, CUL::Graphics::IImageLoader* )
{
}

void Cube::setColor( const CUL::Graphics::ColorS& color )
{
    m_color = color;

    if( !m_initialized )
    {
        return;
    }

    std::lock_guard<std::mutex> renderLock( m_renderMutex );

    static const size_t wallsSize = m_walls.size();
    for( size_t i = 0; i < wallsSize; ++i )
    {
        m_walls[i]->setColor( m_color );
    }
}

void Cube::createPlaceHolders()
{
    TransformComponent::Pos cubeSize( 2.f, 2.f, 2.f );
    TransformComponent::Pos quadSize( 2.f, 2.f, 0.f );

    m_transformComponent->setSize( cubeSize );
    m_transformComponent->setPivot( { 0.0f, 0.f, 0.f } );

    TransformComponent::Pos pivot;
    // 0
    {
        Quad* quad = m_engine->createQuad( this );
        quad->setDisableRenderOnMyOwn( true );
        addChild( quad );
        TransformComponent* transformCmp = quad->getTransform();
        transformCmp->setWorldPosition( ITransformable::Pos( 0.f, 0.f, 1.f ) );
        transformCmp->setSize( quadSize );
        transformCmp->setPivot( pivot );
        quad->getnameCmp()->setName( "Wall00" );
        m_walls[0] = quad;
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
        quad->setColor( m_color );
        m_walls[1] = quad;
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
        quad->setColor( m_color );
        m_walls[2] = quad;
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
        quad->setColor( m_color );
        m_walls[3] = quad;
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
        quad->setColor( m_color );
        m_walls[4] = quad;
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
        quad->setColor( m_color );
        m_walls[5] = quad;
    }
}

void Cube::render()
{
    std::lock_guard<std::mutex> renderLock( m_renderMutex );
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
