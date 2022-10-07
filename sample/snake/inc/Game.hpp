#pragma once

#include "Snake.hpp"

#include "gameengine/Camera.hpp"
#include "gameengine/Viewport.hpp"

#include "SDL2Wrapper/Input/IKeyboardObserver.hpp"
#include "SDL2Wrapper/Input/IMouseObserver.hpp"
#include "SDL2Wrapper/WindowData.hpp"
#include "SDL2Wrapper/Input/MouseData.hpp"

#include "CUL/Graphics/Pos2D.hpp"
#include "CUL/TimeConcrete.hpp"
#include "CUL/Graphics/Color.hpp"

#include "CUL/IMPORT_GLM.hpp"
#include "CUL/STL_IMPORTS/STD_thread.hpp"
#include "CUL/STL_IMPORTS/STD_atomic.hpp"
#include "CUL/STL_IMPORTS/STD_condition_variable.hpp"

NAMESPACE_BEGIN( CUL::GUTILS )
class IConfigFile;
NAMESPACE_END( CUL::GUTILS )

NAMESPACE_BEGIN( LOGLW )
class Program;
class IGameEngine;
class Quad;
class IObject;
class IUtility;
class IObjectFactory;
NAMESPACE_END( LOGLW )

NAMESPACE_BEGIN( SDL2W )
class ISDL2Wrapper;
class IWindow;
NAMESPACE_END( SDL2W )

class Game final: public SDL2W::IMouseObserver, public SDL2W::IKeyboardObserver
{
public:
    Game( int rows, int cols, const CUL::Graphics::Pos2Di& windowPos, const SDL2W::WinSize& winSize );
    void run();

    void stopGame();
    ~Game();

protected:
private:
    void reloadConfig();
    void afterInit();
    void onMouseEvent( const SDL2W::MouseData& mouseData );
    void onKeyBoardEvent( const SDL2W::KeyboardState& key );
    void renderScene();
    void gameLoop();
    void updateGFXLoop();
    void moveSnake();
    void randomizeCandy();
    bool isCandy( const Snake::Pos& pos ) const;
    void changeSnakeMoveDirection( Snake::HeadDirection direction );

    void closeApp();
    Game() = delete;

    Snake::Pos generateStartingPos();

    std::thread m_gameLoopThread;
    std::thread m_updateGFXThread;
    std::atomic<bool> m_runGameLoop = true;

    std::unique_ptr<SDL2W::ISDL2Wrapper> m_sdlw;
    std::unique_ptr<LOGLW::IGameEngine> m_oglw;

    std::mutex m_boardMtx;
    std::atomic<bool> m_boardInitializedB = false;
    std::vector<std::vector<LOGLW::Quad*>> m_background;
    std::condition_variable m_boardInitialized;

    SDL2W::MouseData m_mouseData;

    LOGLW::IUtility* m_utility = nullptr;
    CUL::LOG::ILogger* m_logger = nullptr;
    LOGLW::IObjectFactory* m_objectFactory = nullptr;

    std::unique_ptr<CUL::GUTILS::IConfigFile> m_configFile;
    CUL::Graphics::Pos2Di m_windowPos;
    SDL2W::WinSize m_windowSize;

    LOGLW::IObject* m_triangle0 = nullptr;
    CUL::TimeConcrete configModificationTime;
    SDL2W::IWindow* m_mainWindow = nullptr;

    int m_mouseX = 0.0f;

    CUL::MATH::Angle m_ang90 = { 90, CUL::MATH::Angle::Type::DEGREE };
    CUL::MATH::Angle m_ang180 = { 180, CUL::MATH::Angle::Type::DEGREE };
    CUL::MATH::Angle m_ang270 = { 270, CUL::MATH::Angle::Type::DEGREE };
    glm::vec3 m_eyePos;

    CUL::Graphics::ColorS red = CUL::Graphics::ColorE::RED;
    CUL::Graphics::ColorS yellow = CUL::Graphics::ColorE::YELLOW;
    CUL::Graphics::ColorS blue = CUL::Graphics::ColorE::BLUE;
    CUL::Graphics::ColorS white = CUL::Graphics::ColorE::WHITE;
    CUL::MATH::Angle m_angle;
    LOGLW::IObjectFactory* of = nullptr;
    LOGLW::Program* program = nullptr;
    float blueTriangleZ = -1.0f;
    float redTriangleZ = 1.0f;
    LOGLW::Camera* m_projectionData = nullptr;

    unsigned m_secondsToStartGame = 2;
    std::atomic<unsigned> m_moveDelayMs = 600;
    std::atomic<unsigned> m_deltaTime = 8;

    int m_rowsCount = 0;
    int m_colsCount = 0;

    std::atomic<bool> m_snakeHasMoved = false;

    Snake* m_snake = nullptr;
    Snake::Pos m_candyPos = { -1, -1 };

    std::vector<LOGLW::WindowSize> m_possibleSizes;
    size_t m_currentResolution = 0;
    LOGLW::Viewport m_viewport;

    LOGLW::Quad* m_testQuadUp = nullptr;
    LOGLW::Quad* m_testQuadDown = nullptr;
};