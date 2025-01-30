#include "RegularSDL2Window.hpp"
#include "TextureSDL.hpp"
#include "Sprite.hpp"
#include "TextureSDL.hpp"
#include "CUL/Filesystem/FSApi.hpp"

#include "SDL2Wrapper/ISDL2Wrapper.hpp"

#include "CUL/Filesystem/FS.hpp"
#include "CUL/ITimer.hpp"

#include "SDL2Wrapper/IMPORT_SDL.hpp"

using namespace SDL2W;

using IPivot = CUL::MATH::IPivot;

#ifdef _MSC_VER
// Yes, I know that is a Spectre mitigation.
// But for now, I let this as TODO, since i don't know
// How to fix this.
// TODO
#pragma warning( push )
#pragma warning( disable: 5045 )
#endif
RegularSDL2Window::RegularSDL2Window(
    const WindowData& winData,
    ISDL2Wrapper* wrapper,
    CUL::CULInterface* culInterface ):
    m_windowData( winData ),
    m_wrapper( wrapper ),
    m_culInterface( culInterface ),
    m_fsApi( culInterface->getFS() ),
    m_logger( culInterface->getLogger() )
{
    m_window = createWindow( winData );

    fetchSreenData();

    setName( winData.name );
    m_il = m_culInterface->getImageLoader();
}
#ifdef _MSC_VER
#pragma warning( pop )
#endif

SDL_Window* RegularSDL2Window::createWindow( const WindowData& winData )
{
    auto& pos = winData.pos;
    auto& currentRes = winData.currentRes;
    setCurrentRendererType( winData.rendererType );
    setCurrentRendererType( winData.rendererType );
    auto& winName = winData.name;

    m_logger->log( "Creating window with:", CUL::LOG::Severity::INFO );
    m_logger->log( "Pos.x = " + CUL::String( pos.getX() ) + ", Pos.y = " + CUL::String( pos.getY() ), CUL::LOG::Severity::INFO );
    m_logger->log( "Width = " + CUL::String( currentRes.getWidth() ) + ", height = " + CUL::String( currentRes.getHeight() ), CUL::LOG::Severity::INFO );
    SDL_Window* result = nullptr;
    Uint32 windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;
    if( getCurrentRendererType() == RenderTypes::RendererType::OPENGL_LEGACY ||
        getCurrentRendererType() == RenderTypes::RendererType::OPENGL_MODERN )
    {
        windowFlags |= SDL_WINDOW_OPENGL;
    }

    const auto targetWidth = (int)( currentRes.getWidth() );
    const auto targetHeight = (int)( currentRes.getHeight() );

    auto winTargetName = winName;
    if( winTargetName.empty() )
    {
        winTargetName = "Window: " + std::to_string( currentRes.getWidth() ) + "x" + std::to_string( currentRes.getHeight() );
    }

    result = SDL_CreateWindow(
        winTargetName.cStr(),
        pos.getX(),
        pos.getY(),
        targetWidth,
        targetHeight,
        windowFlags );
    if( nullptr == result )
    {
        auto sdlError = SDL_GetError();
        CUL::String s_sdlError( sdlError );
        m_logger->log(
            "SDL ERROR: [ " + s_sdlError + " ] ", CUL::LOG::Severity::CRITICAL );
        CUL::Assert::simple( false, "The Window has not been initialized." );
    }
    else
    {

        SDL_SysWMinfo wmInfo;
        SDL_VERSION( &wmInfo.version );
        SDL_GetWindowWMInfo( result, &wmInfo );
#if defined(SDL2W_WINDOWS)
        m_hwnd = wmInfo.info.win.window;
#endif // #if defined(SDL2W_WINDOWS)
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

    m_windowData.nativeRes.setSize( nativeDisplayMode.w, nativeDisplayMode.h );
    m_windowData.windowRes = m_windowData.currentRes;
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

            std::cout << "Sizeof m_fpsCounter: " << sizeof( CUL::Video::FPSCounter ) << "\n";
            m_fpsCounter = std::make_unique< CUL::Video::FPSCounter>( m_logger );
            m_fpsCounter->setSampleSize( 4 );
            addFPSCounter( m_fpsCounter.get() );
            m_fpsCounter->start();
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
    while( m_runInfoLoop )
    {
        CUL::ITimer::sleepSeconds( m_sleepTimeInfoLoop );
        const auto currentFpsCount = m_fpsCounter->getCurrentFps();
        const auto averageFpsCount = m_fpsCounter->getAverageFps();
        const auto messageCfps = "CURRENT FPS: " + CUL::String( currentFpsCount );
        const auto messageAfps = "AVERAGE FPS: " + CUL::String( averageFpsCount );
        m_logger->log( messageCfps );
        m_logger->log( messageAfps );
    }
}

RegularSDL2Window::operator SDL_Window*( )
{
    return m_window;
}

RegularSDL2Window::operator const SDL_Window*( )
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
    return m_windowData.nativeRes;
}

void RegularSDL2Window::updateScreenBuffers()
{
    if( ( m_windowData.rendererType == RenderTypes::RendererType::OPENGL_LEGACY ) ||
        ( m_windowData.rendererType == RenderTypes::RendererType::OPENGL_MODERN ) )
    {
        CUL::Assert::simple( nullptr != m_window, "The Window is not initialized." );
        SDL_GL_SwapWindow( m_window );
        // ^ https://forums.libsdl.org/viewtopic.php?p=52399
    }
    else
    {
        //if( m_renderer )
        //{
        //    SDL_RenderPresent( m_renderer );
        //}

    }
    frameHasEnded();
}

void RegularSDL2Window::renderAll()
{
    //SDL_SetRenderDrawColor( m_renderer, (Uint8)m_backgroundColor.getRUI(),
    //                        (Uint8)m_backgroundColor.getGUI(),
    //                        (Uint8)m_backgroundColor.getBUI(),
    //                        (Uint8)m_backgroundColor.getAUI() );
    //std::lock_guard<std::mutex> objectsMutexGuard( m_objectsMtx );
    //for( const auto& object : m_objects )
    //{
    //    if( CUL::Graphics::IObject::Type::SPRITE == object->getType() )
    //    {
    //        auto sprite = static_cast<Sprite*>( object );
    //        auto& pos = object->getRenderPosition();
    //        auto& size = object->getSizeAbs();
    //        auto pivot = object->getPivot( IPivot::PivotType::ABSOLUTE );

    //        SDL_Rect renderQuad;
    //        renderQuad.x = static_cast<int>( pos.getX() );
    //        renderQuad.y = static_cast<int>( pos.getY() );
    //        renderQuad.w = static_cast<int>( size.getX() );
    //        renderQuad.h = static_cast<int>( size.getY() );

    //        std::unique_ptr<SDL_Rect> srcRect;

    //        auto tex = sprite->getTexture();
    //        // TODO: WTF?
    //        auto const texSDLW = dynamic_cast<TextureSDL*>( tex );

    //        const double angle = sprite->getAngle().getValueD(CUL::MATH::Angle::Type::DEGREE);

    //        SDL_Point center;
    //        center.x = static_cast<int>( pivot.getX() );
    //        center.y = static_cast<int>( pivot.getY() );

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
    //if( m_renderer )
    //{
    //    SDL_RenderClear( m_renderer );
    //}
}

void RegularSDL2Window::setBackgroundColor( const ColorS& color )
{
    m_backgroundColor = color;
}

const CUL::Graphics::Pos2Di& RegularSDL2Window::getPos() const
{
    return m_windowData.pos;
}

void RegularSDL2Window::setPos( const CUL::Graphics::Pos2Di& pos )
{
    m_windowData.pos = pos;
    SDL_SetWindowPosition( m_window, pos.getX(), pos.getY() );
}

const WinSize& RegularSDL2Window::getSize() const
{
    return m_windowData.currentRes;
}

void RegularSDL2Window::setSize( const WinSize& size )
{
    m_windowData.currentRes = size;
    SDL_SetWindowSize( m_window, size.w, size.h );
    //SDL_SetWindowDisplayMode( m_window, &m_nativeDisplayMode  );
    //SDL_RenderSetLogicalSize( m_renderer, size.w, size.h );
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

ISprite* RegularSDL2Window::createSprite(
    CUL::Graphics::ITexture* tex )
{
    auto spritePtr = new Sprite();
    spritePtr->setTexture( tex );
    addObject( spritePtr );
    return spritePtr;
}

SurfaceImage RegularSDL2Window::createSurface(
    const CUL::FS::Path& path )
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


    surfaceImage.first = SDL_CreateRGBSurfaceWithFormatFrom( (void*) image->getData(),
        imageInfo.size.width, imageInfo.size.height,
        imageInfo.depth, imageInfo.pitch, pixelFormat );

    CUL::Assert::simple( nullptr != surfaceImage.first, "Cannot create surf: " + path.getPath() );
    CUL::Assert::simple( 0 != surfaceImage.first->pixels, "Cannot create surf: " + path.getPath() );

    surfaceImage.second = image;

    return surfaceImage;
}

CUL::Graphics::ITexture* RegularSDL2Window::createTexture( SDL_Surface* surface, const CUL::FS::Path& path )
{
    m_logger->log( "Creating texture from: " + path );
    //CUL::Assert::simple( nullptr != m_renderer, "RENDERER NOT READY!\n" );
    CUL::Assert::simple( nullptr != surface, "SURFACE IS NULL!\n" );

    //auto texSDL = new TextureSDL();

    //auto const tex = SDL_CreateTextureFromSurface( m_renderer, surface );
    //CUL::Assert::simple(
    //    nullptr != tex,
    //    "Cannot create texture from " +
    //    path.getPath() +
    //    " does not exist." );

    //texSDL->setTexture( tex, path );
    //m_textures[path.getPath()] = std::unique_ptr<ITexture>( texSDL );
    //return texSDL;
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

CUL::Video::FPSCounter* RegularSDL2Window::getFpsCounter()
{
    return m_fpsCounter.get();
}

void RegularSDL2Window::setFullscreen( bool fullscreen )
{
    const Uint32 flag = fullscreen ? (Uint32) SDL_WINDOW_FULLSCREEN : 0u;
    SDL_SetWindowFullscreen( m_window, flag );
    SDL_ShowCursor(fullscreen);
}

RegularSDL2Window::~RegularSDL2Window()
{
    closeInfoLoop();

    m_logger->log( "RegularSDL2Window::~RegularSDL2Window()" );
    destroyObjects();

    //if( m_renderer )
    //{
    //    SDL_DestroyRenderer( m_renderer );
    //    m_renderer = nullptr;
    //}

    //Assert( nullptr != m_window, "The Window has been destroyed somwhere else." );
    //SDL_DestroyWindow( m_window );
    //m_window = nullptr;
}

void RegularSDL2Window::closeInfoLoop()
{
    if( m_infoPrintLoop.joinable() )
    {
        m_runInfoLoop = false;
        m_infoPrintLoop.join();
    }
}