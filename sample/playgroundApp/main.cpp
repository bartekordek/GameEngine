#include "gameengine/IGameEngine.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/IDebugOverlay.hpp"
#include "gameengine/Sprite.hpp"
#include "gameengine/EngineParams.hpp"

#include "SDL2Wrapper/Input/IKey.hpp"
#include "SDL2Wrapper/IWindowEventListener.hpp"
#include "SDL2Wrapper/IWindow.hpp"
#include "SDL2Wrapper/WindowData.hpp"
#include "SDL2Wrapper/Input/MouseData.hpp"

#include "CUL/GenericUtils/DumbPtr.hpp"
#include "CUL/GenericUtils/ConsoleUtilities.hpp"

using GLWrap = CUL::GUTILS::DumbPtr<LOGLW::IGameEngine>;
using ColorS = CUL::Graphics::ColorS;
using ColorE = CUL::Graphics::ColorE;
using ShaderFile = CUL::FS::IFile;
template <typename TYPE>
using DumbPtr = CUL::GUTILS::DumbPtr<TYPE>;
using FF = CUL::FS::FileFactory;
using Rect = CUL::Graphics::Rect3Di;
using Pos3Df = CUL::Graphics::Pos3Df;
using Triangle = CUL::MATH::Primitives::Triangle;
using String = CUL::String;

DumbPtr<CUL::GUTILS::IConfigFile> g_configFile;
GLWrap g_oglw;
LOGLW::IUtility* g_utility = nullptr;
CUL::LOG::ILogger* g_logger = nullptr;
ColorS red( ColorE::RED );
ColorS yellow( 1.0f, 1.0f, 0.0f, 1.0f );
ColorS blue( ColorE::BLUE );
ColorS white( ColorE::WHITE );
CUL::MATH::Angle g_angle;
LOGLW::IObjectFactory* of = nullptr;
CUL::FS::Path vertexShaderFile;
CUL::FS::Path fragmentShaderFile;
LOGLW::Program* program = nullptr;
float blueTriangleZ = -1.0f;
float redTriangleZ = 1.0f;
LOGLW::Camera g_camera;
glm::vec3 g_eyePos;
LOGLW::IObject* g_triangle0 = nullptr;
CUL::TimeConcrete configModificationTime;
SDL2W::IWindow* g_mainWindow = nullptr;
SDL2W::MouseData g_mouseData;
std::mutex g_renderMtx;

LOGLW::ITriangle* g_blueTriangle = nullptr;
LOGLW::ITriangle* g_whiteTriangle = nullptr;

LOGLW::ITriangle* g_redTriangle = nullptr;
LOGLW::ITriangle* g_yellowTriangle = nullptr;

LOGLW::Sprite* g_sprite = nullptr;

const CUL::String wrapperDir = "../gameengine";
const CUL::FS::Path shadersDir( wrapperDir + "/shaders/" );

int g_mouseX = 0.0f;

CUL::MATH::Angle ang90( 90, CUL::MATH::Angle::Type::DEGREE );
CUL::MATH::Angle ang180( 180, CUL::MATH::Angle::Type::DEGREE );
CUL::MATH::Angle ang270( 270, CUL::MATH::Angle::Type::DEGREE );

void afterInit();
void renderScene();
void onKeyBoardEvent( const SDL2W::IKey& key );
SDL2W::WindowCallback g_windowCallback;
void onWindowEvent( const SDL2W::WindowEvent::Type type );
void closeApp();
void reloadConfig();

void onMouseEvent( const SDL2W::MouseData& md );

int main( int argc, char** argv )
{
    CUL::GUTILS::ConsoleUtilities cu;
    cu.setArgs( argc, argv );

    auto width = 1280;
    auto height = 600;

    auto valW = cu.getFlagValue( "-w" ).string();
    if( !valW.empty() )
    {
        width = std::stoi( valW );
    }

    auto valH = cu.getFlagValue( "-h" ).string();
    if( !valH.empty() )
    {
        height = std::stoi( valH );
    }

    CUL::Graphics::Pos2Di winPos = { 200, 200 };
    SDL2W::WinSize winSize = { width, height };

    LOGLW::EngineParams engineParams;
    engineParams.configPath = "Config.txt";
    engineParams.legacy = false;
    engineParams.windowPosition = winPos;
    engineParams.winSize = winSize;
    engineParams.winName = "gameenginePlaygroundApp";

    g_oglw = LOGLW::IGameEngine::createGameEngine( engineParams );
    g_oglw->addMouseEventCallback( onMouseEvent );
    g_configFile = g_oglw->getConfig();

    g_logger = g_oglw->getLoger();

    g_utility = g_oglw->getUtility();
    g_oglw->onInitialize( afterInit );
    g_oglw->beforeFrame( renderScene );

    g_oglw->registerKeyboardEventCallback( &onKeyBoardEvent );
    g_oglw->registerWindowEventCallback( &onWindowEvent );

    g_oglw->startRenderingLoop();

    g_oglw->runEventLoop();

    return 0;
}

void afterInit()
{
    g_mainWindow = g_oglw->getMainWindow();
    g_mainWindow->setBackgroundColor( SDL2W::ColorS( 1.0f, 0.0f, 0.0f, 1.0f ) );
    const auto& winSize = g_mainWindow->getSize();

    g_camera.setSize( { winSize.getWidth(), winSize.getHeight() } );
    g_camera.setEyePos( { 0.0f, 0.0f, 131.f } );

    reloadConfig();
    configModificationTime = g_configFile->getModificationTime();

    g_mainWindow->toggleFpsCounter( true, 8u );

    g_oglw->drawDebugInfo( true );

    // g_oglw->getDebugOverlay()->addText( "Blue Z: %f", &blueTriangleZ );
    g_oglw->getDebugOverlay()->addSliderValue( "Blue Z", &blueTriangleZ, -64.0f, 128.f,
                                               []()
                                               {
                                               } );

    g_oglw->getDebugOverlay()->addSliderValue( "Red Z", &redTriangleZ, -64.0f, 128.f,
                                               []()
                                               {
                                               } );

    g_mouseData = g_oglw->getMouseData();

    const float size = 32.f;
    LOGLW::TriangleData values;
    values[0] = LOGLW::PointType( size, -size, 0.0f );
    values[1] = LOGLW::PointType{ -size, -size, 0.0f };
    values[2] = LOGLW::PointType{ -size, size, 0.0f };

    g_blueTriangle = g_oglw->getObjectFactory()->createTriangle( values, LOGLW::ColorE::BLUE );
    g_whiteTriangle = g_oglw->getObjectFactory()->createTriangle( values, LOGLW::ColorE::WHITE );

    g_redTriangle = g_oglw->getObjectFactory()->createTriangle( values, LOGLW::ColorE::RED );
    g_yellowTriangle = g_oglw->getObjectFactory()->createTriangle( values, LOGLW::ColorE::YELLOW );

    g_sprite = g_oglw->getObjectFactory()->createSprite( "../../media/texture.png" );
}

void renderScene()
{
    g_blueTriangle->setWorldAngle( CUL::MATH::EulerAngles::YAW, g_angle );
    g_whiteTriangle->setWorldAngle( CUL::MATH::EulerAngles::YAW, g_angle + ang180 );

    g_redTriangle->setWorldAngle( CUL::MATH::EulerAngles::YAW, g_angle );
    g_yellowTriangle->setWorldAngle( CUL::MATH::EulerAngles::YAW, g_angle + ang180 );

    auto oldPosWhiteBlue = g_blueTriangle->getWorldPosition();
    oldPosWhiteBlue.z() = blueTriangleZ;
    g_blueTriangle->setWorldPosition( oldPosWhiteBlue );
    g_whiteTriangle->setWorldPosition( oldPosWhiteBlue );

    auto oldPosRedYellow = g_redTriangle->getWorldPosition();
    oldPosRedYellow.z() = redTriangleZ;
    g_redTriangle->setWorldPosition( oldPosRedYellow );
    g_yellowTriangle->setWorldPosition( oldPosRedYellow );

    g_sprite->setWorldPosition( 0.f, 80.f * std::sin( g_angle.getRad() ), 40.f * std::cos( g_angle.getRad() ) );

    g_angle += 0.01f;

    if( g_configFile )
    {
        auto newTime = g_configFile->getModificationTime();
        if( newTime > configModificationTime )
        {
            g_logger->log( "Reloading..." );
            reloadConfig();
            g_logger->log( "Reloading... done." );

            configModificationTime = g_configFile->getModificationTime();
        }
    }

    const auto amp = 64.f;
    const auto frac = 0.8f;

    blueTriangleZ = amp + std::sin( g_angle.getRad() * frac ) * amp;
    redTriangleZ = amp + std::cos( g_angle.getRad() * frac ) * amp;
}

void reloadConfig()
{
    if( g_configFile )
    {
        g_configFile->reload();

        const auto x = 0.0f;

        g_camera.setCenter(
            glm::vec3( x, g_configFile->getValue( "CENTER_Y" ).toFloat(), g_configFile->getValue( "CENTER_Z" ).toFloat() ) );
        g_eyePos = g_camera.getEye();
        g_eyePos.z = g_configFile->getValue( "EYE_Z" ).toFloat();
        g_camera.setEyePos( g_eyePos );

        g_camera.setUp( glm::vec3( 0.0f, 1.0f, 0.0f ) );
        g_camera.getCenter().z =  g_configFile->getValue( "Z_FAR" ).toFloat();
    }
}

void onMouseEvent( const SDL2W::MouseData& mouseData )
{
    if( mouseData.isButtonDown( 3 ) )
    {
        const auto& md = g_oglw->getMouseData();
        const auto& winSize = g_oglw->getMainWindow()->getSize();
        const auto winW = winSize.getWidth();
        const auto winH = winSize.getHeight();
        const auto mouseX = md.getX();
        const auto mouseY = md.getY();
        const auto centerX = winW / 2 - mouseX;
        const auto centerY = winH / 2 - mouseY;

        g_logger->log( "Mouse:" );
        g_logger->log( md.serialize( 0 ) );

        const auto rectW = winW / 2;
        const auto rectH = winH / 2;

        const auto leftX = ( winW - rectW ) / 2;
        const auto rightX = winW - leftX;

        const auto bottom = ( winH - rectH ) / 2;
        const auto top = winH - bottom;

        if( mouseX < rightX && mouseX > leftX && mouseY < top && mouseY > bottom )

        {
            auto eye = g_oglw->getCamera().getEye();
            static auto delta = 0.5f;
            eye.x = +centerX * delta;
            eye.y = -centerY * delta;
            g_oglw->setEyePos( eye );
            g_mouseData = md;
        }
    }
}

void onKeyBoardEvent( const SDL2W::IKey& key )
{
    if( key.getKeyIsDown() == false )
    {
        return;
    }

    const auto deltaZ = 1.0f;
    const auto delta = 2.0f;
    const CUL::String& keyName = key.getKeyName();
    if( keyName == "Q" )
    {
        closeApp();
    }
    else if( keyName == "W" )
    {
        blueTriangleZ -= deltaZ;
        g_logger->log( "setting blueTriangleZ to: " + String( blueTriangleZ ) );
    }
    else if( keyName == "S" )
    {
        blueTriangleZ += deltaZ;
        g_logger->log( "setting blueTriangleZ to: " + String( blueTriangleZ ) );
    }
    else if( keyName == "U" )
    {
        auto newVal = g_camera.getZfar() + delta;
        g_camera.getCenter().z = newVal;
        g_logger->log( "setting zFar to: " + String( newVal ) );
    }
    else if( keyName == "J" )
    {
        auto newVal = g_camera.getZfar() - delta;

         g_camera.getCenter().z = newVal;
        g_logger->log( "setting zFar to: " + String( newVal ) );
    }
    else if( keyName == "I" )
    {
        g_eyePos.z += 2.0f;
        g_oglw->getCamera().setEyePos( g_eyePos );
        g_logger->log( "setting g_eyePos.z to: " + String( g_eyePos.z ) );
    }
    else if( keyName == "K" )
    {
        g_eyePos.z -= 2.0f;
        g_oglw->getCamera().setEyePos( g_eyePos );
        g_logger->log( "setting g_eyePos.z to: " + String( g_eyePos.z ) );
    }
    else if( keyName == "T" )
    {
        redTriangleZ -= deltaZ;
        g_logger->log( "setting redTriangleZ to: " + String( redTriangleZ ) );
    }
    else if( keyName == "G" )
    {
        redTriangleZ += deltaZ;
        g_logger->log( "setting redTriangleZ to: " + String( redTriangleZ ) );
    }
    else if( keyName == "P" )
    {
        static bool toggle = true;
        if( toggle == true )
        {
            g_logger->log( "Changing projection to ortographic." );
           // g_oglw->setProjectionType( LOGLW::ProjectionType::ORTO );
        }
        else
        {
            g_logger->log( "Changing projection to PERSPECTIVE." );
           // g_oglw->setProjectionType( LOGLW::ProjectionType::PERSPECTIVE );
        }
        toggle = !toggle;
    }
}

void onWindowEvent( const SDL2W::WindowEvent::Type type )
{
    if( SDL2W::WindowEvent::Type::CLOSE == type )
    {
        closeApp();
    }
}

void closeApp()
{
    g_oglw->stopRenderingLoop();
    g_oglw->stopEventLoop();
}
