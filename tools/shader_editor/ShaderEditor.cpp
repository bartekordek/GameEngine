#include "ShaderEditor.hpp"

#include "gameengine/IGameEngine.hpp"
#include "gameengine/EngineParams.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/IDebugOverlay.hpp"
#include "gameengine/Primitives/Triangle.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/Cube.hpp"
#include "gameengine/Primitives/Quad.hpp"
#include "gameengine/Sprite.hpp"

#include "SDL2Wrapper/IWindow.hpp"
#include "SDL2Wrapper/Input/MouseData.hpp"

#include "LOGLWAdditionalDeps/ImportImgui.hpp"

#include "CUL/ITimer.hpp"
#include "CUL/Filesystem/PathDialog.hpp"
#include "CUL/Filesystem/RegularFile.hpp"

CUL::MATH::Angle ang90( 90, CUL::MATH::Angle::Type::DEGREE );
CUL::MATH::Angle ang180( 180, CUL::MATH::Angle::Type::DEGREE );
CUL::MATH::Angle ang270( 270, CUL::MATH::Angle::Type::DEGREE );


struct EditorState
{
    CUL::String Name;
    TextEditor Editor;
    CUL::String Extension;
    std::unique_ptr<CUL::FS::RegularFile> File;
    std::string CachedText;
};

ShaderEditor::ShaderEditor()
{

}

void ShaderEditor::run()
{
    auto width = 1280;
    auto height = 600;

    CUL::Graphics::Pos2Di winPos = { 200, 200 };
    SDL2W::WinSize winSize = { width, height };

    LOGLW::EngineParams engineParams;
    engineParams.configPath = "Config.txt";
    engineParams.legacy = false;
    engineParams.windowPosition = winPos;
    engineParams.winSize = winSize;
    engineParams.winName = "gameengineShaderEditorApp";

    m_engine = LOGLW::IGameEngine::createGameEngine( engineParams );

    m_engine->addMouseEventCallback( [this] ( const SDL2W::MouseData& mouseData ){onMouseEvent( mouseData ); } );

    m_engine->onInitialize( [this] (){afterInit(); } );

    m_engine->registerKeyboardEventCallback( [this] ( const SDL2W::KeyboardState& key ){onKeyBoardEvent( key ); } );
    m_engine->registerWindowEventCallback( [this] ( const SDL2W::WindowEvent::Type type ){onWindowEvent( type ); } );
    m_engine->drawOrigin( true );
    m_engine->startRenderingLoop();

    m_timer.reset(CUL::TimerFactory::getChronoTimer( m_engine->getLoger() ) );

    m_engine->runEventLoop();
}

void ShaderEditor::afterInit()
{
    m_mainWindow = m_engine->getMainWindow();
    m_mainWindow->setBackgroundColor(SDL2W::ColorS(1.0f, 0.0f, 0.0f, 1.0f));
    const auto& winSize = m_mainWindow->getSize();


    m_camera = &m_engine->getCamera();

    m_camera->setSize({winSize.getWidth(), winSize.getHeight()});
    m_camera->setEyePos({0.0f, 0.0f, 32.f});
    m_camera->setCenter({0.f, 0.f, -10.f});
    m_camera->setZNear(1.f);

    m_mainWindow->toggleFpsCounter(true, 8u);

    m_engine->drawDebugInfo(true);
    m_engine->drawOrigin(true);


    m_timer->runEveryPeriod([this]() {timer(); }, 40000);

    m_engine->guiFrameDelegate.addDelegate(
        [this](float x, float y)
    {
        guiIteration(x, y);
    });
    m_engine->toggleDrawDebugInfo(false);

    TextEditor::LanguageDefinition languageDefinition = TextEditor::LanguageDefinition::GLSL();

    std::unique_ptr<EditorState> pixelShader = std::make_unique<EditorState>();
    pixelShader->Extension = "frag";
    pixelShader->Name = "pixel_shader";
    pixelShader->Editor.SetReadOnly(true);
    m_editors.insert({pixelShader->Name.string(), std::move(pixelShader)});


    std::unique_ptr<EditorState> vertexShader = std::make_unique<EditorState>();
    vertexShader->Extension = "vert";
    vertexShader->Name = "vertex_shader";
    vertexShader->Editor.SetReadOnly(true);
    m_editors.insert({vertexShader->Name.string(), std::move(vertexShader)});

    for(auto& [currentName, currentPtr] : m_editors)
    {
        currentPtr->Editor.SetLanguageDefinition(languageDefinition);
    }
}

void ShaderEditor::timer()
{
}

void ShaderEditor::guiIteration( float x, float /*y*/ )
{
    const static CUL::String name = "MAIN";
    ImGui::Begin( name.cStr(), nullptr,
                  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar );

    ImGui::SetWindowPos( { x, 0 } );

    auto winSize = m_engine->getMainWindow()->getSize();
    const auto targetWidht = (float)winSize.w - x;
    const auto targetHeight = (float)winSize.h * 1.f;
    const float wholeWidth = targetWidht * 0.5f;
    const float editorWidth = wholeWidth / 2.f;
    ImGui::SetWindowSize( {wholeWidth, targetHeight } );

    drawEditor(x, 0, editorWidth, targetHeight, "pixel_shader");
    drawEditor(x + editorWidth, 0, editorWidth, targetHeight, "vertex_shader");

    ImGui::End();
}

void ShaderEditor::drawEditor( float x, float y, float w, float h, const CUL::String& name )
{
    EditorState& editorState = *m_editors[name.string()];

    ImGui::SetNextWindowPos( ImVec2( x, y ), ImGuiCond_Always );
    ImGui::SetNextWindowSize( ImVec2( w, h ), ImGuiCond_Always );

    bool bOpen{ true };
    ImGuiWindowFlags window_flags{ 0 };
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;

    ImGui::Begin( name.cStr(), &bOpen, window_flags );

    if( ImGui::Button( "Press to find file." ) )
    {
        CUL::FS::PathDialog::Filter filter;
        filter.Name = editorState.Name;
        filter.Spec = editorState.Extension;
        const CUL::String choosenShader = CUL::FS::PathDialog::getInstance().openDialog( filter );
        if( choosenShader.empty() == false )
        {
            editorState.Editor.SetReadOnly( true );
            editorState.File = std::make_unique<CUL::FS::RegularFile>( choosenShader, m_engine->getCul() );
            editorState.File->setPath( choosenShader );
            editorState.File->loadBackground( true, true, [&editorState]() {
                editorState.CachedText = editorState.File->getAsOneString().string();
            } );
        }
    }

    if( !editorState.File || editorState.File->getPath().getIsEmpty() )
    {
        ImGui::Text( "Choose path->" );
    }
    else
    {
        ImGui::Text( "%s",editorState.File->getPath().getPath().cStr() );
    }

    if( editorState.CachedText.empty() == false )
    {
        editorState.Editor.SetText( editorState.CachedText );
        editorState.Editor.SetReadOnly( false );
        const std::string editorText = editorState.Editor.GetText();
        if( editorText != editorState.CachedText )
        {
            m_engine->getLoger()->log("Hello!");
        }

    }

    editorState.Editor.Render( name.cStr() );

    ImGui::End();
}

void ShaderEditor::onMouseEvent( const SDL2W::MouseData& /*mouseData*/ )
{
}

void ShaderEditor::onKeyBoardEvent( const SDL2W::KeyboardState& /*key*/ )
{
}

void ShaderEditor::onWindowEvent( const SDL2W::WindowEvent::Type type )
{
    if( SDL2W::WindowEvent::Type::CLOSE == type )
    {
        closeApp();
    }
}

void ShaderEditor::closeApp()
{
    m_engine->stopRenderingLoop();
    m_engine->stopEventLoop();
}

ShaderEditor::~ShaderEditor()
{

}