#pragma once

#include "Snake.hpp"

#include "gameengine/Camera.hpp"
#include "gameengine/Viewport.hpp"
#include "gameengine/Input/IKeyboardObserver.hpp"
#include "gameengine/Input/IMouseObserver.hpp"
#include "gameengine/Windowing/WinData.hpp"
#include "gameengine/Input/MouseData.hpp"

#include "CUL/Time.hpp"
#include "CUL/Graphics/Pos2D.hpp"
#include "CUL/Graphics/Color.hpp"
#include "CUL/Math/Angle.hpp"

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
class IRenderDevice;
class ShaderProgram;
NAMESPACE_END( LOGLW )

NAMESPACE_BEGIN( LOGLW )
class ISDL2Wrapper;
class IWindow;
NAMESPACE_END( LOGLW )

class Game final: public LOGLW::IMouseObserver, public LOGLW::IKeyboardObserver
{
public:
    Game( int rows, int cols,
        const LOGLW::WinPos& winPos,
        const LOGLW::WinSize& winSize );
    void run();

    void stopGame();
    ~Game();

protected:
private:
    void reloadConfig();
    void afterInit();
    void onMouseEvent( const LOGLW::MouseData& mouseData );
    void onKeyBoardEvent( const LOGLW::KeyboardState& key );
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

    std::unique_ptr<LOGLW::ISDL2Wrapper> m_sdlw;
    std::unique_ptr<LOGLW::IGameEngine> m_oglw;

    std::mutex m_boardMtx;
    std::atomic<bool> m_boardInitializedB = false;
    std::vector<std::vector<LOGLW::Quad*>> m_background;
    std::condition_variable m_boardInitialized;

    LOGLW::MouseData m_mouseData;

    LOGLW::IRenderDevice* m_utility = nullptr;
    CUL::LOG::ILogger* m_logger = nullptr;

    std::unique_ptr<CUL::GUTILS::IConfigFile> m_configFile;
    LOGLW::WinPos m_windowPos;
    LOGLW::WinSize m_windowSize;

    LOGLW::IObject* m_triangle0 = nullptr;
    CUL::Time configModificationTime;
    LOGLW::IWindow* m_mainWindow = nullptr;

    int m_mouseX = 0.0f;

    CUL::MATH::Angle m_ang90 { 90, CUL::MATH::Angle::Type::DEGREE };
    CUL::MATH::Angle m_ang180 { 180, CUL::MATH::Angle::Type::DEGREE };
    CUL::MATH::Angle m_ang270 { 270, CUL::MATH::Angle::Type::DEGREE };
    glm::vec3 m_eyePos;

    CUL::Graphics::ColorS red = CUL::Graphics::ColorE::RED;
    CUL::Graphics::ColorS yellow = CUL::Graphics::ColorE::YELLOW;
    CUL::Graphics::ColorS blue = CUL::Graphics::ColorE::BLUE;
    CUL::Graphics::ColorS white = CUL::Graphics::ColorE::WHITE;
    CUL::MATH::Angle m_angle;
    LOGLW::ShaderProgram* program = nullptr;
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