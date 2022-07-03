#pragma once

#include "Snake.hpp"

#include "SDL2Wrapper/Input/IKeyboardObserver.hpp"
#include "SDL2Wrapper/Input/IMouseObserver.hpp"
#include "SDL2Wrapper/WindowData.hpp"

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
    void onKeyBoardEvent( const SDL2W::IKey& key );
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

    DumbPtr<SDL2W::ISDL2Wrapper> m_sdlw;
    DumbPtr<LOGLW::IGameEngine> m_oglw;

    std::mutex m_boardMtx;
    std::atomic<bool> m_boardInitializedB = false;
    std::vector<std::vector<LOGLW::IQuad*>> m_background;
    std::condition_variable m_boardInitialized;

    SDL2W::MouseData m_mouseData;

    LOGLW::IUtility* m_utility = nullptr;
    CUL::LOG::ILogger* m_logger = nullptr;
    LOGLW::IObjectFactory* m_objectFactory = nullptr;

    DumbPtr<CUL::GUTILS::IConfigFile> m_configFile;
    CUL::Graphics::Pos2Di m_windowPos;
    SDL2W::WinSize m_windowSize;

    LOGLW::IObject* m_triangle0 = nullptr;
    CUL::TimeConcrete configModificationTime;
    SDL2W::IWindow* m_mainWindow = nullptr;

    int m_mouseX = 0.0f;

    CUL::MATH::Angle m_ang90 = { 90, CUL::MATH::Angle::Type::DEGREE };
    CUL::MATH::Angle m_ang180 = { 180, CUL::MATH::Angle::Type::DEGREE };
    CUL::MATH::Angle m_ang270 = { 270, CUL::MATH::Angle::Type::DEGREE };
    Pos3Df m_eyePos;

    ColorS red = ColorE::RED;
    ColorS yellow = ColorE::YELLOW;
    ColorS blue = ColorE::BLUE;
    ColorS white = ColorE::WHITE;
    CUL::MATH::Angle m_angle;
    LOGLW::IObjectFactory* of = nullptr;
    CUL::FS::Path vertexShaderFile;
    CUL::FS::Path fragmentShaderFile;
    LOGLW::Program* program = nullptr;
    float blueTriangleZ = -1.0f;
    float redTriangleZ = 1.0f;
    LOGLW::Camera m_projectionData;

    unsigned m_secondsToStartGame = 2;
    std::atomic<unsigned> m_moveDelayMs = 600;
    std::atomic<unsigned> m_deltaTime = 16;

    int m_rowsCount = 0;
    int m_colsCount = 0;

    std::atomic<bool> m_snakeHasMoved = false;

    Snake* m_snake = nullptr;
    Snake::Pos m_candyPos = { -1, -1 };

    std::vector<LOGLW::WindowSize> m_possibleSizes;
    size_t m_currentResolution = 0;
    LOGLW::Viewport m_viewport;
};