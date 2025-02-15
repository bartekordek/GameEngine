#include "RegularSDL2Window.hpp"
#include "TextureSDL.hpp"
#include "Sprite.hpp"
#include "TextureSDL.hpp"
#include "CUL/Filesystem/FSApi.hpp"

#include "gameengine/ISDL2Wrapper.hpp"

#include "CUL/Filesystem/FS.hpp"
#include "CUL/ITimer.hpp"

#include "sdl2wrapper/IMPORT_SDL.hpp"
#include "CUL/IMPORT_tracy.hpp"

using namespace LOGLW;

using IPivot = CUL::MATH::IPivot;

#ifdef _MSC_VER
// Yes, I know that is a Spectre mitigation.
// But for now, I let this as TODO, since i don't know
// How to fix this.
// TODO
#pragma warning( push )
#pragma warning( disable : 5045 )
#endif
RegularSDL2Window::RegularSDL2Window( const WinData& winData, ISDL2Wrapper* wrapper, CUL::CULInterface* culInterface )
    : m_windowData( winData ),
      m_wrapper( wrapper ),
      m_culInterface( culInterface ),
      m_fsApi( culInterface->getFS() ),
      m_logger( culInterface->getLogger() )
{
    m_window = createWindow( winData );

    fetchSreenData();

    setName( winData.Name );
    m_il = m_culInterface->getImageLoader();
}
#ifdef _MSC_VER
#pragma warning( pop )
#endif

SDL_Window* RegularSDL2Window::createWindow( const WinData& winData )
{
    auto& pos = winData.Pos;
    auto& currentRes = winData.CurrentRes;
    setCurrentRendererType( winData.RendererType );
    auto& winName = winData.Name;

    m_logger->log( "Creating window with:", CUL::LOG::Severity::INFO );
    m_logger->log( "Pos.x = " + CUL::String( pos.X ) + ", Pos.y = " + CUL::String( pos.Y ), CUL::LOG::Severity::INFO );
    m_logger->log( "Width = " + CUL::String( currentRes.W ) + ", height = " + CUL::String( currentRes.H ), CUL::LOG::Severity::INFO );
    SDL_Window* result = nullptr;
    Uint32 windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;
    if( getCurrentRendererType() == RenderTypes::RendererType::OPENGL_LEGACY ||
        getCurrentRendererType() == RenderTypes::RendererType::OPENGL_MODERN )
    {
        windowFlags |= SDL_WINDOW_OPENGL;
    }

    const auto targetWidth = (int)( currentRes.W );
    const auto targetHeight = (int)( currentRes.H );

    auto winTargetName = winName;
    if( winTargetName.empty() )
    {
        winTargetName = "Window: " + std::to_string( currentRes.W ) + "x" + std::to_string( currentRes.H );
    }

    result = SDL_CreateWindow( winTargetName.cStr(), pos.X, pos.Y, targetWidth, targetHeight, windowFlags );
    if( nullptr == result )
    {
        auto sdlError = SDL_GetError();
        CUL::String s_sdlError( sdlError );
        m_logger->log( "SDL ERROR: [ " + s_sdlError + " ] ", CUL::LOG::Severity::CRITICAL );
        CUL::Assert::simple( false, "The Window has not been initialized." );
    }
    else
    {
        SDL_SysWMinfo wmInfo;
        SDL_VERSION( &wmInfo.version );
        SDL_GetWindowWMInfo( result, &wmInfo );
#if defined( CUL_WINDOWS )
        m_hwnd = wmInfo.info.win.window;
#endif  // #if defined(CUL_WINDOWS)
    }

    return result;
}

void RegularSDL2Window::fetchSreenData()
{
    SDL_DisplayMode nativeDisplayMode;
    const auto winId = SDL_GetWindowID( m_window );
    const auto displayIndex = SDL_GetWindowDisplayIndex( m_window );
    SDL_GetCurrentDisplayMode( displayIndex, &nativeDisplayMode );

    setWindowID( winId );

    m_windowData.NativeRes = { (std::uint16_t)nativeDisplayMode.w, (std::uint16_t)nativeDisplayMode.h };
    m_windowData.WindowRes = m_windowData.CurrentRes;
}

#include "CUL/STL_IMPORTS/STD_iostream.hpp"
void RegularSDL2Window::toggleFpsCounter( bool on, short unsigned everyNsecond )
{
    if( on )
    {
        if( !m_runInfoLoop )
        {
            m_sleepTimeInfoLoop = everyNsecond;
            m_runInfoLoop = true;
            m_infoPrintLoop = std::thread( &RegularSDL2Window::infoLoop, this );
        }
    }
    else
    {
        m_runInfoLoop = false;
        closeInfoLoop();
    }
}

void RegularSDL2Window::infoLoop()
{
}

RegularSDL2Window::operator SDL_Window*()
{
    return m_window;
}

RegularSDL2Window::operator const SDL_Window*()
{
    return m_window;
}

void RegularSDL2Window::destroyObjects()
{
    std::lock_guard<std::mutex> objectsMutexGuard( m_objectsMtx );
    m_textures.clear();
}

const WinSize& RegularSDL2Window::getCurrentScreenNativeResolution() const
{
    return m_windowData.NativeRes;
}

void RegularSDL2Window::updateScreenBuffers()
{
    ZoneScoped;
    if( ( m_windowData.RendererType == RenderTypes::RendererType::OPENGL_LEGACY ) ||
        ( m_windowData.RendererType == RenderTypes::RendererType::OPENGL_MODERN ) )
    {
        CUL::Assert::simple( nullptr != m_window, "The Window is not initialized." );
        SDL_GL_SwapWindow( m_window );
        // ^ https://forums.libsdl.org/viewtopic.php?p=52399
    }
    else
    {
        // if( m_renderer )
        //{
        //     SDL_RenderPresent( m_renderer );
        // }
    }
}

void RegularSDL2Window::renderAll()
{
    // SDL_SetRenderDrawColor( m_renderer, (Uint8)m_backgroundColor.getRUI(),
    //                         (Uint8)m_backgroundColor.getGUI(),
    //                         (Uint8)m_backgroundColor.getBUI(),
    //                         (Uint8)m_backgroundColor.getAUI() );
    // std::lock_guard<std::mutex> objectsMutexGuard( m_objectsMtx );
    // for( const auto& object : m_objects )
    //{
    //     if( CUL::Graphics::IObject::Type::SPRITE == object->getType() )
    //     {
    //         auto sprite = static_cast<Sprite*>( object );
    //         auto& pos = object->getRenderPosition();
    //         auto& size = object->getSizeAbs();
    //         auto pivot = object->getPivot( IPivot::PivotType::ABSOLUTE );

    //        SDL_Rect renderQuad;
    //        renderQuad.x = static_cast<int>( pos.X );
    //        renderQuad.y = static_cast<int>( pos.Y );
    //        renderQuad.w = static_cast<int>( size.X );
    //        renderQuad.h = static_cast<int>( size.Y );

    //        std::unique_ptr<SDL_Rect> srcRect;

    //        auto tex = sprite->getTexture();
    //        // TODO: WTF?
    //        auto const texSDLW = dynamic_cast<TextureSDL*>( tex );

    //        const double angle = sprite->getAngle().getValueD(CUL::MATH::Angle::Type::DEGREE);

    //        SDL_Point center;
    //        center.x = static_cast<int>( pivot.X );
    //        center.y = static_cast<int>( pivot.Y );

    //        auto result = SDL_RenderCopyEx(
    //            m_renderer,
    //            texSDLW->getTexture(),
    //            srcRect.get(),
    //            &renderQuad,
    //            angle, &center, SDL_RendererFlip::SDL_FLIP_NONE );
    //        CUL::Assert::simple( result == 0, "Cannot render SDL texture..." );
    //    }
    //}
}

void RegularSDL2Window::setBackgroundColor( const ColorE color )
{
    setBackgroundColor( ColorS( color ) );
}

void RegularSDL2Window::clearBuffers()
{
    // if( m_renderer )
    //{
    //     SDL_RenderClear( m_renderer );
    // }
}

void RegularSDL2Window::setBackgroundColor( const ColorS& color )
{
    m_backgroundColor = color;
}

const WinPos& RegularSDL2Window::getPos() const
{
    return m_windowData.Pos;
}

void RegularSDL2Window::setPos( const WinPos& pos )
{
    m_windowData.Pos = pos;
    SDL_SetWindowPosition( m_window, pos.X, pos.Y );
}

const WinSize& RegularSDL2Window::getSize() const
{
    return m_windowData.CurrentRes;
}

void RegularSDL2Window::setSize( const WinSize& size )
{
    m_windowData.CurrentRes = size;
    SDL_SetWindowSize( m_window, size.W, size.H );
    // SDL_SetWindowDisplayMode( m_window, &m_nativeDisplayMode  );
    // SDL_RenderSetLogicalSize( m_renderer, size.w, size.h );
}

IWindow::Type RegularSDL2Window::getType() const
{
    return IWindow::Type::SDL_WIN;
}

ISprite* RegularSDL2Window::createSprite( const CUL::FS::Path& objPath )
{
    ISprite* result = nullptr;
    CUL::Assert::simple( objPath.getPath() != "", "EMTPY PATH." );
    auto it = m_textures.find( objPath.getPath().string() );
    if( m_textures.end() == it )
    {
        auto tex = createTexture( objPath );
        result = createSprite( tex );
    }
    else
    {
        auto tex = it->second.get();
        result = createSprite( tex );
    }
    return result;
}

CUL::Graphics::ITexture* RegularSDL2Window::createTexture( const CUL::FS::Path& objPath )
{
    CUL::Graphics::ITexture* result = nullptr;
    CUL::Assert::simple( objPath.getPath() != "", "EMTPY PATH." );
    auto it = m_textures.find( objPath.getPath().string() );
    if( m_textures.end() == it )
    {
        auto surface = createSurface( objPath );
        auto tex = createTexture( surface.first, objPath );
        SDL_FreeSurface( surface.first );
        surface.first = nullptr;

        delete surface.second;
        surface.second = nullptr;
        result = tex;
    }
    else
    {
        result = it->second.get();
    }
    return result;
}

ISprite* RegularSDL2Window::createSprite( CUL::Graphics::ITexture* tex )
{
#if SDL2WRAPPER_SPRITE
    auto spritePtr = new Sprite();
    spritePtr->setTexture( tex );
    addObject( spritePtr );
    return spritePtr;
#else   // #if SDL2WRAPPER_SPRITE
    (void*)tex;
    return nullptr;
#endif  // #if SDL2WRAPPER_SPRITE
}

SurfaceImage RegularSDL2Window::createSurface( const CUL::FS::Path& path )
{
    SurfaceImage surfaceImage;

    m_logger->log( "Current dir: " + m_fsApi->getCurrentDir() );

    if( false == path.exists() )
    {
        m_logger->log( "Checking for path FAILED: ", CUL::LOG::Severity::CRITICAL );
        CUL::Assert::simple( false, "File " + path.getPath() + " does not exist." );
    }

    m_logger->log( "Loading: " + path );
    auto image = m_il->loadImage( path );
    CUL::Assert::simple( nullptr != image, "Cannot load: " + path.getPath() );

    const auto& imageInfo = image->getImageInfo();

    Uint32 pixelFormat = 0;
    switch( imageInfo.pixelFormat )
    {
        case CUL::Graphics::PixelFormat::RGB:
            pixelFormat = SDL_PIXELFORMAT_RGB24;
            break;
        case CUL::Graphics::PixelFormat::ARGB:
            pixelFormat = SDL_PIXELFORMAT_ARGB32;
            break;
        case CUL::Graphics::PixelFormat::RGBA:
            pixelFormat = SDL_PIXELFORMAT_RGBA32;
            break;
        case CUL::Graphics::PixelFormat::NONE:
            CUL::Assert::simple( false, "Pixel format of surface not set." );
            break;
        case CUL::Graphics::PixelFormat::BGR:
            pixelFormat = SDL_PIXELFORMAT_BGR24;
            break;
        case CUL::Graphics::PixelFormat::BGRA:
            pixelFormat = SDL_PIXELFORMAT_BGRA32;
            break;
        case CUL::Graphics::PixelFormat::ABGR:
            pixelFormat = SDL_PIXELFORMAT_ABGR32;
            break;
        case CUL::Graphics::PixelFormat::TEST:
            pixelFormat = SDL_PIXELFORMAT_RGB555;
            break;
    }

    surfaceImage.first = SDL_CreateRGBSurfaceWithFormatFrom( (void*)image->getData(), imageInfo.size.width, imageInfo.size.height,
                                                             imageInfo.depth, imageInfo.pitch, pixelFormat );

    CUL::Assert::simple( nullptr != surfaceImage.first, "Cannot create surf: " + path.getPath() );
    CUL::Assert::simple( 0 != surfaceImage.first->pixels, "Cannot create surf: " + path.getPath() );

    surfaceImage.second = image;

    return surfaceImage;
}

CUL::Graphics::ITexture* RegularSDL2Window::createTexture( SDL_Surface* surface, const CUL::FS::Path& path )
{
    m_logger->log( "Creating texture from: " + path );
    // CUL::Assert::simple( nullptr != m_renderer, "RENDERER NOT READY!\n" );
    CUL::Assert::simple( nullptr != surface, "SURFACE IS NULL!\n" );

    // auto texSDL = new TextureSDL();

    // auto const tex = SDL_CreateTextureFromSurface( m_renderer, surface );
    // CUL::Assert::simple(
    //     nullptr != tex,
    //     "Cannot create texture from " +
    //     path.getPath() +
    //     " does not exist." );

    // texSDL->setTexture( tex, path );
    // m_textures[path.getPath()] = std::unique_ptr<ITexture>( texSDL );
    // return texSDL;
    return nullptr;
}

void RegularSDL2Window::addObject( CUL::Graphics::IObject* object )
{
    std::lock_guard<std::mutex> objectsMutexGuard( m_objectsMtx );
    m_objects.insert( object );
}

void RegularSDL2Window::removeObject( CUL::Graphics::IObject* object )
{
    std::lock_guard<std::mutex> objectsMutexGuard( m_objectsMtx );
    m_objects.erase( object );
}

ColorS RegularSDL2Window::getBackgroundColor() const
{
    return m_backgroundColor;
}

void RegularSDL2Window::setFullscreen( bool fullscreen )
{
    const Uint32 flag = fullscreen ? (Uint32)SDL_WINDOW_FULLSCREEN : 0u;
    SDL_SetWindowFullscreen( m_window, flag );
    SDL_ShowCursor( fullscreen );
}

RegularSDL2Window::~RegularSDL2Window()
{
    closeInfoLoop();

    m_logger->log( "RegularSDL2Window::~RegularSDL2Window()" );
    destroyObjects();

    // if( m_renderer )
    //{
    //     SDL_DestroyRenderer( m_renderer );
    //     m_renderer = nullptr;
    // }

    // Assert( nullptr != m_window, "The Window has been destroyed somwhere else." );
    // SDL_DestroyWindow( m_window );
    // m_window = nullptr;
}

void RegularSDL2Window::closeInfoLoop()
{
    if( m_infoPrintLoop.joinable() )
    {
        m_runInfoLoop = false;
        m_infoPrintLoop.join();
    }
}