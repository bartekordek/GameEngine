#pragma once

#include "SDL2Wrapper/IWindow.hpp"
#include "SDL2Wrapper/WindowData.hpp"

#include "CUL/Video/FPSCounter.hpp"
#include "CUL/Graphics/IImageLoader.hpp"

#include "CUL/Memory/UniquePtrOnStack.hpp"

#include "CUL/CULInterface.hpp"

#include "CUL/STL_IMPORTS/STD_atomic.hpp"
#include "CUL/STL_IMPORTS/STD_thread.hpp"
#include "CUL/STL_IMPORTS/STD_memory.hpp"
#include "CUL/STL_IMPORTS/STD_set.hpp"
#include "CUL/STL_IMPORTS/STD_mutex.hpp"
#include "CUL/STL_IMPORTS/STD_utility.hpp"

struct SDL_Surface;

NAMESPACE_BEGIN( CUL::FS )
class Path;
NAMESPACE_END( CUL::FS )

NAMESPACE_BEGIN( SDL2W )

using SurfaceImage = std::pair<SDL_Surface*, CUL::Graphics::IImage*>;

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable: 4820 )
#endif

class RegularSDL2Window final:
    public IWindow
{
public:
    RegularSDL2Window(
        const WindowData& winData,
        ISDL2Wrapper* wrapper,
        CUL::CULInterface* culInterface );

    void fetchSreenData();

    ~RegularSDL2Window();
protected:
private:
    SDL_Window* createWindow( const WindowData& winDatae );
    SurfaceImage createSurface( const CUL::FS::Path& path );
    CUL::Graphics::ITexture* createTexture( SDL_Surface* surface, const CUL::FS::Path& path );
    void destroyObjects();
    const WinSize& getCurrentScreenNativeResolution() const override;

    void toggleFpsCounter( bool on, short unsigned everyNsecond = 2 ) override;
    void closeInfoLoop();
    void infoLoop();

    std::unique_ptr< CUL::Video::FPSCounter> m_fpsCounter;
    std::atomic<bool> m_runInfoLoop = false;
    std::atomic<unsigned int> m_sleepTimeInfoLoop = 2u;
    std::thread m_infoPrintLoop;

    // Inherited via IWindow
    void addObject( CUL::Graphics::IObject* object ) override;
    void removeObject( CUL::Graphics::IObject* object ) override;

    operator SDL_Window*( ) override;
    operator const SDL_Window*( ) override;

    const CUL::Graphics::Pos2Di& getPos() const override;
    void setPos( const CUL::Graphics::Pos2Di& pos ) override;

    const WinSize& getSize() const override;
    void setSize( const WinSize& size ) override;

    void setBackgroundColor( const ColorE color ) override;
    void setBackgroundColor( const ColorS& color ) override;
    void clearBuffers() override;
    void renderAll() override;
    void updateScreenBuffers() override;

    IWindow::Type getType() const override;

    CUL::Graphics::ITexture* createTexture( const CUL::FS::Path& path ) override;
    ISprite* createSprite( const CUL::FS::Path& path ) override;
    ISprite* createSprite( CUL::Graphics::ITexture* tex ) override;

    ColorS getBackgroundColor() const override;

    CUL::Video::FPSCounter* getFpsCounter() override;
    void setFullscreen( bool fullscreen ) override;

    WindowData m_windowData;
    ColorS m_backgroundColor;
    std::set<CUL::Graphics::IObject*> m_objects;
    std::mutex m_objectsMtx;
    TextureMap m_textures;
    CUL::Graphics::IImageLoader* m_il = nullptr;
    ISDL2Wrapper* m_wrapper = nullptr;
    CUL::CULInterface* m_culInterface = nullptr;
    CUL::FS::FSApi* m_fsApi = nullptr;
    CUL::LOG::ILogger* m_logger = nullptr;

// Deleted:
    RegularSDL2Window( const RegularSDL2Window& win ) = delete;
    RegularSDL2Window( RegularSDL2Window&& win ) = delete;
    RegularSDL2Window& operator=( const RegularSDL2Window& right ) = delete;
    RegularSDL2Window& operator=( RegularSDL2Window&& right ) = delete;
    RegularSDL2Window() = delete;
};

#ifdef _MSC_VER
#pragma warning( pop )
#endif

NAMESPACE_END( SDL2W )