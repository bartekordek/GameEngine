#include "Game.hpp"

#include "gameengine/IGameEngine.hpp"
#include "gameengine/IDebugOverlay.hpp"
#include "gameengine/Primitives/Quad.hpp"
#include "gameengine/Components/TransformComponent.hpp"

#include "SDL2Wrapper/ISDL2Wrapper.hpp"
#include "SDL2Wrapper/IWindow.hpp"

#include "CUL/ITimer.hpp"

Game::Game( int rows, int cols, const CUL::Graphics::Pos2Di& windowPos, const SDL2W::WinSize& winSize )
    : m_windowPos( windowPos ), m_windowSize( winSize ), m_rowsCount( rows ), m_colsCount( cols )
{
    m_background.resize( rows );
    for( auto& row : m_background )
    {
        row.resize( cols );
    }

    SDL2W::WindowData windowData;
    windowData.name = "Snake!";
    windowData.pos = m_windowPos;
    windowData.currentRes = m_windowSize;
    windowData.rendererName = "opengl";

    m_sdlw.reset( SDL2W::ISDL2Wrapper::createSDL2Wrapper() );
    m_sdlw->init( windowData, "Config.txt" );

    m_oglw.reset( LOGLW::IGameEngine::createGameEngine( m_sdlw.get(), true ) );

    m_configFile.reset( m_oglw->getConfig() );

    m_sdlw->registerMouseEventListener( this );
    m_sdlw->registerKeyboardEventListener( this );

    m_logger = m_oglw->getLoger();
    m_utility = m_oglw->getDevice();
    m_objectFactory = m_oglw->getObjectFactory();

    m_oglw->onInitialize(
        [this]()
        {
            this->afterInit();
        } );

    m_oglw->beforeFrame(
        [this]()
        {
            this->renderScene();
        } );

    m_oglw->registerWindowEventCallback( [this]( const SDL2W::WindowEvent::Type type ) {
        if( SDL2W::WindowEvent::Type::CLOSE == type )
        {
            closeApp();
        }
    } );

    m_possibleSizes.push_back( { 1024, 800 } );
    m_possibleSizes.push_back( { 1680, 900 } );
    m_possibleSizes.push_back( { 1920, 1080 } );
}

void Game::afterInit()
{
    m_mainWindow = m_oglw->getMainWindow();
    m_mainWindow->setBackgroundColor( SDL2W::ColorS( 1.0f, 0.0f, 0.0f, 1.0f ) );

    glm::vec3 eye = { 0.0f, 0.0f, 300.f };
    m_oglw->getCamera().setEyePos( eye );

    m_oglw->getCamera().setZNear( 1.f );

    reloadConfig();
    configModificationTime = m_configFile->getModificationTime();

    m_mainWindow->toggleFpsCounter( true, 8u );

    m_oglw->drawDebugInfo( true );
    m_oglw->drawOrigin( true );

    m_oglw->getDebugOverlay()->addSliderValue( "Blue Z", &blueTriangleZ, -64.0f, 128.f,
                                               []()
                                               {
                                               } );

    m_oglw->getDebugOverlay()->addSliderValue( "Red Z", &redTriangleZ, -64.0f, 128.f,
                                               []()
                                               {
                                               } );

    m_mouseData = m_oglw->getMouseData();

    const float size = 32.f;

    const float offset = size * 0.4f;
    size_t index = 0;
    const auto rowsCount = m_background.size();
    if( rowsCount > 0 )
    {
        const auto colsCount = m_background[0].size();

        const float backgroundW = rowsCount * size + ( rowsCount - 1 ) * offset;
        const float backgroundH = colsCount * size + ( colsCount - 1 ) * offset;

        const float xOffset = size / 2 - backgroundW / 2;
        const float yOffset = size / 2 - backgroundH / 2;

        for( size_t row = 0; row < rowsCount; ++row )
        {
            for( size_t col = 0; col < colsCount; ++col )
            {
                LOGLW::Quad* quad = m_oglw->createQuad( nullptr );
                quad->getTransform()->setPivot( { 0.5f, 0.5f, 0.f } );
                quad->getTransform()->setSize( { size, size, size } );
                quad->getTransform()->setWorldPosition( xOffset + col * ( size + offset ), yOffset + row * ( size + offset ), 0.f );
                m_background[row][col] = quad;
                ++index;
            }
        }
    }

    //m_testQuadUp = m_oglw->createQuad( nullptr );
    //m_testQuadUp->getTransform()->setPivot( { 0.5f, 0.f, 0.f } );
    //m_testQuadUp->getTransform()->setSize( { size, size, size } );
    //m_testQuadUp->getTransform()->setWorldPosition( 0.f, 0.0f, 200.f );
    //m_testQuadUp->setColor( CUL::Graphics::ColorE::GREEN );

    //m_testQuadDown = m_oglw->createQuad( nullptr );
    //m_testQuadDown->getTransform()->setPivot( { 0.5f, 1.f, 0.f } );
    //m_testQuadDown->getTransform()->setSize( { size, size, size } );
    //m_testQuadDown->getTransform()->setWorldPosition( 0.0f, 0.f, 200.f );
    //m_testQuadDown->setColor( CUL::Graphics::ColorE::RED );

    m_boardInitializedB = true;
    m_boardInitialized.notify_one();

    m_eyePos = m_oglw->getCamera().getEye();
}

void Game::run()
{
    m_oglw->startRenderingLoop();
    m_gameLoopThread = std::thread( &Game::gameLoop, this );
    m_oglw->runEventLoop();
}

void Game::gameLoop()
{
    CUL::ITimer::sleepMiliSeconds( m_secondsToStartGame * 1000 );

    Snake snake( 0, 0 );
    snake.m_headDirection = Snake::HeadDirection::EAST;
    m_snake = &snake;

    m_updateGFXThread = std::thread( &Game::updateGFXLoop, this );

    randomizeCandy();
    while( m_runGameLoop )
    {
        m_snakeHasMoved = false;
        CUL::ITimer::sleepMiliSeconds( m_moveDelayMs );
        moveSnake();
    }

    if( m_updateGFXThread.joinable() )
    {
        m_updateGFXThread.join();
    }
}

void Game::updateGFXLoop()
{
    std::mutex mtx;
    std::unique_lock<std::mutex> lk( mtx );
    m_boardInitialized.wait( lk,
                             [this]
                             {
                                 return m_boardInitializedB == true;
                             } );

    while( m_runGameLoop )
    {
        for( int row = 0; row < m_rowsCount; ++row )
        {
            for( int col = 0; col < m_colsCount; ++col )
            {
                auto cell = m_background[row][col];
                if( m_snake->isFieldSnake( { row, col } ) )
                {
                    cell->setColor( LOGLW::ColorE::GREEN );
                }
                else if( isCandy( { row, col } ) )
                {
                    cell->setColor( LOGLW::ColorE::RED );
                }
                else
                {
                    cell->setColor( LOGLW::ColorE::WHITE );
                }
            }
        }
    }
}

void Game::moveSnake()
{
    if( !m_snake )
    {
        return;
    }

    if( m_snakeHasMoved )
    {
        return;
    }

    if( m_snake->nextMoveIsGameOver( m_rowsCount, m_colsCount ) )
    {
        m_logger->log( "GAME OVER!" );
    }
    else
    {
        const auto isOnCandy = m_snake->isFieldSnake( m_candyPos );

        if( isOnCandy )
        {
            randomizeCandy();
            m_moveDelayMs -= m_deltaTime;
        }

        m_snake->move( isOnCandy );

        m_snakeHasMoved = true;
    }
}

void Game::changeSnakeMoveDirection( Snake::HeadDirection direction )
{
    if( !m_snake )
    {
        return;
    }

    m_snake->m_headDirection = direction;
}

void Game::randomizeCandy()
{
    if( m_rowsCount != 0 && m_colsCount != 0 )
    {
        srand( (unsigned)time( NULL ) );

        do
        {
            m_candyPos.row = rand() % m_rowsCount;
            m_candyPos.col = rand() % m_colsCount;
        } while( m_snake->isFieldSnake( m_candyPos ) );
    }
}

bool Game::isCandy( const Snake::Pos& pos ) const
{
    return m_candyPos == pos;
}

void Game::renderScene()
{
    if( m_configFile )
    {
        auto newTime = m_configFile->getModificationTime();
        if( newTime > configModificationTime )
        {
            m_logger->log( "Reloading..." );
            reloadConfig();
            m_logger->log( "Reloading... done." );

            configModificationTime = m_configFile->getModificationTime();
        }
    }

    m_angle += 0.01f;
}

void Game::onMouseEvent( const SDL2W::MouseData& mouseData )
{
    if( mouseData.isButtonDown( 3 ) )
    {
        const auto& md = m_oglw->getMouseData();
        const auto& winSize = m_oglw->getMainWindow()->getSize();
        const auto winW = winSize.getWidth();
        const auto winH = winSize.getHeight();
        const auto mouseX = md.getX();
        const auto mouseY = md.getY();
        const auto centerX = winW / 2 - mouseX;
        const auto centerY = winH / 2 - mouseY;

        const auto rectW = winW / 2;
        const auto rectH = winH / 2;

        const auto leftX = ( winW - rectW ) / 2;
        const auto rightX = winW - leftX;

        const auto bottom = ( winH - rectH ) / 2;
        const auto top = winH - bottom;

        if( mouseX < rightX && mouseX > leftX && mouseY < top && mouseY > bottom )

        {
            auto eye = m_oglw->getCamera().getEye();
            static auto delta = 0.5f;
            eye.x = +centerX * delta;
            eye.y = -centerY * delta;
            m_oglw->setEyePos( eye );
            m_mouseData = md;
        }
    }

    if( mouseData.getEventType() == SDL2W::MouseData::EventType::MOUSEWHEEL )
    {
        if( mouseData.getWheelY() == 1 || mouseData.getWheelY() == -1 )
        {
            m_eyePos.z -= mouseData.getWheelY() * 1.f;
            m_oglw->setEyePos( m_eyePos );
            m_logger->log( "setting m_eyePos.z to: " + CUL::String( m_eyePos.z ) );
        }
    }
}

void Game::onKeyBoardEvent( const SDL2W::KeyboardState& key )
{
    if( !m_snakeHasMoved )
    {
        if( key.at( "Up" ) )
        {
            changeSnakeMoveDirection( Snake::HeadDirection::NORTH );
        }
        if( key.at( "Down" ) )
        {
            changeSnakeMoveDirection( Snake::HeadDirection::SOUTH );
        }
        if( key.at( "Left" ) )
        {
            changeSnakeMoveDirection( Snake::HeadDirection::WEST );
        }
        if( key.at( "Right" ) )
        {
            changeSnakeMoveDirection( Snake::HeadDirection::EAST );
        }
    }

    if( key.at( "U" ) )
    {
        glm::vec3 eyePos = m_oglw->getCamera().getEye();
        eyePos.z += 1.f;
        m_oglw->getCamera().setEyePos( eyePos );
    }
    if( key.at( "J" ) )
    {
        glm::vec3 eyePos = m_oglw->getCamera().getEye();
        eyePos.z -= 1.f;
        m_oglw->getCamera().setEyePos( eyePos );
    }

    if( key.at( "Q" ) )
    {
        closeApp();
    }
    else if( key.at( "P" ) )
    {
        static bool toggle = true;
        if( toggle == true )
        {
            m_logger->log( "Changing projection to ortographic." );
            //m_oglw->setProjectionType( LOGLW::ProjectionType::ORTO );
        }
        else
        {
            m_logger->log( "Changing projection to PERSPECTIVE." );
            //m_oglw->setProjectionType( LOGLW::ProjectionType::PERSPECTIVE );
        }
        toggle = !toggle;
    }
    else if( key.at( "F" ) )
    {
        static bool fullScreen = false;
        fullScreen = !fullScreen;
        m_sdlw->getMainWindow()->setFullscreen( fullScreen );
    }
    else if( key.at( "S" ) )
    {
        m_viewport = m_oglw->getViewport();
        if( m_currentResolution == m_possibleSizes.size() - 1 )
        {
            m_currentResolution = 0;
        }
        else
        {
            ++m_currentResolution;
        }

        LOGLW::Size2Di pos;
        pos.setSize( m_possibleSizes[m_currentResolution].w, m_possibleSizes[m_currentResolution].h );
        m_viewport.set( { 0, 0 }, pos );

        LOGLW::WindowSize windowSize =  m_possibleSizes[m_currentResolution];
        m_sdlw->getMainWindow()->setSize( windowSize.w, windowSize.h );
        m_oglw->setViewport( m_viewport );
    }
}

void Game::reloadConfig()
{
    if( m_configFile )
    {
        m_configFile->reload();
    }
}

void Game::closeApp()
{
    stopGame();
    m_oglw->stopRenderingLoop();
    m_oglw->stopEventLoop();
}

Snake::Pos Game::generateStartingPos()
{
    // std::default_random_engine generator;
    // std::uniform_int_distribution<int> distribution( 1, 6 );
    // int dice_roll = distribution( generator );
    return Snake::Pos();
}

void Game::stopGame()
{
    m_runGameLoop = false;
}

Game::~Game()
{
    if( m_gameLoopThread.joinable() )
    {
        m_gameLoopThread.join();
    }
}