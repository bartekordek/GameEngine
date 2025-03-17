#include "ShaderEditor.hpp"

#include "gameengine/AttributeMeta.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/ExecuteType.hpp"
#include "gameengine/EngineParams.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/IDebugOverlay.hpp"
#include "gameengine/Primitives/Triangle.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/Cube.hpp"
#include "gameengine/Primitives/Quad.hpp"
#include "gameengine/Sprite.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"
#include "gameengine/VertexArray.hpp"

#include "gameengine/Windowing/IWindow.hpp"
#include "gameengine/Input/MouseData.hpp"

#include "LOGLWAdditionalDeps/ImportImgui.hpp"

#include "CUL/ITimer.hpp"
#include "CUL/Filesystem/PathDialog.hpp"
#include "CUL/Filesystem/RegularFile.hpp"
#include "CUL/Math/Utils.hpp"
#include "CUL/Threading/ThreadUtil.hpp"

#include "CUL/STL_IMPORTS/STD_cmath.hpp"

#define M_PI 3.14159265358979323846264338327950288

CUL::MATH::Angle ang90( 90, CUL::MATH::Angle::Type::DEGREE );
CUL::MATH::Angle ang180( 180, CUL::MATH::Angle::Type::DEGREE );
CUL::MATH::Angle ang270( 270, CUL::MATH::Angle::Type::DEGREE );

struct EditorState
{
    CUL::String Name;
    TextEditor Editor;
    CUL::String Extension;
    std::unique_ptr<CUL::FS::RegularFile> File;
    CUL::String CachedText;
    EShaderUnitState ShaderUnitState{ EShaderUnitState::Empty };
};

ShaderEditor::ShaderEditor( const LOGLW::WinSize& inWinSize, const LOGLW::WinPos& inWinPos ):
    m_winSize( inWinSize ), m_winPos( inWinPos )
{
}

void ShaderEditor::run()
{
    CUL::ThreadUtil::getInstance().setThreadName( "EventThread" );

    LOGLW::EngineParams engineParams;
    engineParams.ConfigPath = "Config.txt";
    engineParams.legacy = false;
    engineParams.WinDataVal.Pos = m_winPos;
    engineParams.WinDataVal.CurrentRes = m_winSize;
    engineParams.WinDataVal.Name = "gameengineShaderEditorApp";

    m_engine = LOGLW::IGameEngine::createGameEngine( engineParams );
    m_timer.reset( CUL::TimerFactory::getChronoTimerPtr( m_engine->getLoger() ) );
    m_engine->startRenderingLoop();

    m_engine->addMouseEventCallback(
        [this]( const LOGLW::MouseData& mouseData )
        {
            onMouseEvent( mouseData );
        } );

    m_engine->onInitialize(
        [this]()
        {
            afterInit();
        } );

    m_engine->registerKeyboardEventCallback(
        [this]( const LOGLW::KeyboardState& key )
        {
            onKeyBoardEvent( key );
        } );
    m_engine->registerWindowEventCallback(
        [this]( const LOGLW::WindowEvent::Type type )
        {
            onWindowEvent( type );
        } );

    m_engine->runEventLoop();
}

void ShaderEditor::afterInit()
{
    m_mainWindow = m_engine->getMainWindow();
    m_mainWindow->setBackgroundColor( LOGLW::ColorS( 1.0f, 0.0f, 0.0f, 1.0f ) );
    const auto& winSize = m_mainWindow->getSize();

    m_camera = &m_engine->getCamera();

    m_camera->setSize( { winSize.W, winSize.H } );
    m_camera->setEyePos( { 6.0f, 10.0f, 24.f } );
    m_camera->setCenter( { 0.f, 0.f, -0.1f } );
    m_camera->setZNear( 1.f );
    m_eye = m_camera->getEye();
    m_cameraPosSp.fromCarthezian( m_eye.x, m_eye.y, m_eye.z );

    m_mainWindow->toggleFpsCounter( true, 8u );

    m_engine->drawDebugInfo( true );
    m_engine->toggleDrawDebugInfo( false );
    m_engine->drawOrigin( true );

    m_timer->runEveryPeriod(
        [this]()
        {
            timer();
        },
        40000 );

    m_engine->guiFrameDelegate.addDelegate(
        [this]( float x, float y )
        {
            guiIteration( x, y );
        } );

    TextEditor::LanguageDefinition languageDefinition = TextEditor::LanguageDefinition::GLSL();

    std::unique_ptr<EditorState> pixelShader = std::make_unique<EditorState>();
    pixelShader->Extension = "frag";
    pixelShader->Name = "pixel_shader";
    pixelShader->Editor.SetReadOnly( true );
    m_editors.insert( { pixelShader->Name.string(), std::move( pixelShader ) } );

    std::unique_ptr<EditorState> vertexShader = std::make_unique<EditorState>();
    vertexShader->Extension = "vert";
    vertexShader->Name = "vertex_shader";
    vertexShader->Editor.SetReadOnly( true );
    m_editors.insert( { vertexShader->Name.string(), std::move( vertexShader ) } );

    for( auto& [currentName, currentPtr] : m_editors )
    {
        currentPtr->Editor.SetLanguageDefinition( languageDefinition );
    }

    m_vao = m_engine->createVAO( "ShaderEditor::vao" );
    LOGLW::VertexData vertData;
    // std::vector<std::uint32_t> indices = {
    //     // note that we start from 0!
    //     0, 1, 3,  // first Triangle
    //     1, 2, 3   // second Triangle
    // };
    std::vector<std::uint32_t> indices = {
        0, 1, 2  // first Triangle
    };
    vertData.Indices.createFrom( indices );

    // constexpr float size = 4.f;
    // vertData.vertices = m_shape.toVectorOfFloat();

    /*
    x0, y0, z0, r0, g0, b0,
    x1, y1, z1, r1, g1, b1,
    x2, y2, z2, r2, g2, b2,
    */
    std::vector<float> vertices = { 0.0f, 0.0f, 0.f, 1.f, 0.f, 0.f, 1.0f, 1.0f, 0.f, 1.f, 0.f, 0.f, 1.0f, 0.0f, 0.f, 1.f, 0.f, 0.f };

    vertData.Data.createFrom( vertices );

    LOGLW::AttributeMeta posAtt;
    posAtt.Name = "pos";
    posAtt.Index = 0;
    posAtt.Size = 3;
    posAtt.Type = LOGLW::DataType::FLOAT;
    posAtt.StrideBytes = sizeof( float ) * 6;
    vertData.Attributes.emplace_back( posAtt );

    LOGLW::AttributeMeta norAtt;
    norAtt.Name = "color";
    norAtt.Index = 1;
    norAtt.Size = 3;
    norAtt.Type = LOGLW::DataType::FLOAT;
    norAtt.StrideBytes = sizeof( float ) * 6;
    norAtt.DataOffset = reinterpret_cast<void*>( 3 * sizeof( float ) );
    vertData.Attributes.emplace_back( norAtt );

    vertData.primitiveType = LOGLW::PrimitiveType::TRIANGLES;

    m_vao->addVertexBuffer( vertData );
    m_vao->setProgram( m_engine->createProgram() );
    m_vao->toggleRenderOnMyOwn( true );
    m_transformComponent = std::make_unique<LOGLW::TransformComponent>( nullptr );
}

void ShaderEditor::timer()
{
}

void ShaderEditor::guiIteration( float x, float y )
{
    drawLeftWindow( x, y );
    drawRightWindow( x, y );
}

void ShaderEditor::drawLeftWindow( float x, float /*y*/ )
{
    const static CUL::String name = "Left";
    ImGui::Begin( name.cStr(), nullptr,
                  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar );

    auto winSize = m_engine->getMainWindow()->getSize();
    const auto targetHeight = (float)winSize.H * 1.0f;
    const float menuWidth = (float)winSize.W * 0.3f;
    const float editorWidth = menuWidth / 1.25f;
    const float editorHeight = targetHeight * 0.5f;
    // const float windowX = (float)winSize.w - menuWidth; // Left side.
    const float windowX = x;

    ImGui::SetWindowPos( { windowX, 2.f } );
    ImGui::SetWindowSize( { menuWidth, targetHeight } );

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    const std::int32_t editorY = 16;
    if( ImGui::BeginTabBar( "Editors Tabs", tab_bar_flags ) )
    {
        if( ImGui::BeginTabItem( "Pixel Shader" ) )
        {
            drawEditor( x, editorY, editorWidth, editorHeight, "pixel_shader" );
            ImGui::EndTabItem();
        }

        if( ImGui::BeginTabItem( "Vertex Shader" ) )
        {
            drawEditor( x, editorY, editorWidth, editorHeight, "vertex_shader" );
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    if( ImGui::Button( "Link" ) )
    {
        m_vao->getProgram()->link( LOGLW::EExecuteType::Now );
    }

    if( ImGui::Button( "Validate" ) )
    {
        m_vao->getProgram()->validate();
    }

    if( m_vao->getProgram()->getIsLinked() )
    {
        if( ImGui::TreeNode( "Uniforms" ) )
        {
            const std::vector<CUL::String> uniformNames = m_vao->getProgram()->getUniformsNames();
            for( const CUL::String& uniformName : uniformNames )
            {
                const auto& uniformVal = m_vao->getProgram()->getUniformValue( uniformName );
                ImGui::Text( "ID: %d, Name: %s, Type: %s", uniformVal.Id, uniformVal.Name.cStr(), uniformVal.TypeName.cStr() );
                if( uniformVal.Type == LOGLW::DataType::FLOAT )
                {
                    ImGui::SameLine();
                    ImGui::Text( "val: %f", std::get<float>( uniformVal.Value ) );
                }
                if( uniformVal.Type == LOGLW::DataType::FLOAT )
                {
                    ImGui::SameLine();
                    ImGui::Text( "val: %f", std::get<float>( uniformVal.Value ) );
                }
                if( uniformVal.Type == LOGLW::DataType::FLOAT_MAT4 )
                {
                    const glm::mat4 currentValue = std::get<glm::mat4>( uniformVal.Value );
                    char tableName[32];
                    sprintf_s( tableName, "%s - %s", *m_vao->getProgram()->getName(), *uniformName );

                    static ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable |
                                                   ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
                                                   ImGuiTableFlags_ContextMenuInBody;
                    if( ImGui::BeginTable( tableName, 4, flags ) )
                    {
                        for( std::int32_t row = 0; row < 4; ++row )
                        {
                            ImGui::TableNextRow();
                            for( std::int32_t column = 0; column < 4; ++column )
                            {
                                ImGui::TableSetColumnIndex( column );
                                float f0 = currentValue[row][column];
                                ImGui::DragFloat( "df", &f0, 0.005f );
                            }
                        }
                        ImGui::EndTable();
                    }
                }
            }
            ImGui::TreePop();
        }
    }

    float xx, yy;
    m_engine->drawObjectsInfo( xx, yy );
}

void ShaderEditor::drawRightWindow( float /*x*/, float /*y*/ )
{
    const static CUL::String name = "Right";
    ImGui::Begin( name.cStr(), nullptr,
                  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar );

    auto winSize = m_engine->getMainWindow()->getSize();
    const auto targetHeight = (float)winSize.H * 1.0f;
    const float menuWidth = (float)winSize.W * 0.3f;
    const float rightPadding = menuWidth + 0.f;
    const float windowX = (float)winSize.W - rightPadding;  // Right side.

    ImGui::SetWindowPos( { windowX, 2.f } );
    ImGui::SetWindowSize( { menuWidth, targetHeight } );

    LOGLW::Camera& camera = m_engine->getCamera();
    glm::vec3 eyePos = camera.getEye();

    ImGui::Text( "Eye pos: x: %f, y: %f, z: %f", eyePos.x, eyePos.y, eyePos.z );
    constexpr float toDeg = 180.f / static_cast<float>( M_PI );
    ImGui::Text( "Eye pos in spherical [rad]: theta: %f, phi: %f, r: %f", m_cameraPosSp.getTheta(), m_cameraPosSp.getPhi(),
                 m_cameraPosSp.getR() );
    ImGui::Text( "Eye pos in spherical [deg]: theta: %f, phi: %f, r: %f", m_cameraPosSp.getTheta() * toDeg, m_cameraPosSp.getPhi() * toDeg,
                 m_cameraPosSp.getR() );

    float theta = m_cameraPosSp.getTheta();
    if( ImGui::SliderFloat( "Theta Rad", &theta, 0.0f, CUL::MATH::SphericalCoord<float>::TwoPi * 0.5f ) )
    {
        m_cameraPosSp.setTheta( theta );
        m_cameraPosSp.toCarthezian( m_eye.x, m_eye.y, m_eye.z );
        m_engine->setEyePos( m_eye );
    }

    float thetaDeg = CUL::MATH::Converter<float>::toDeg( m_cameraPosSp.getTheta() );
    if( ImGui::SliderFloat( "Theta Deg", &thetaDeg, 0.0f, 180 ) )
    {
        const float convertedToRad = CUL::MATH::Converter<float>::toRad( thetaDeg );
        m_cameraPosSp.setTheta( convertedToRad );
        m_cameraPosSp.toCarthezian( m_eye.x, m_eye.y, m_eye.z );
        m_engine->setEyePos( m_eye );
    }

    float phi = m_cameraPosSp.getPhi();
    if( ImGui::SliderFloat( "Phi Rad", &phi, 0.f, CUL::MATH::SphericalCoord<float>::TwoPi * 0.5f ) )
    {
        m_cameraPosSp.setPhi( phi );
        m_cameraPosSp.toCarthezian( m_eye.x, m_eye.y, m_eye.z );
        m_engine->setEyePos( m_eye );
    }

    float phiDeg = CUL::MATH::Converter<float>::toDeg( m_cameraPosSp.getPhi() );
    if( ImGui::SliderFloat( "Phi Deg", &phiDeg, 0.f, 180 ) )
    {
        m_cameraPosSp.setPhi( CUL::MATH::Converter<float>::toRad( phiDeg ) );
        m_cameraPosSp.toCarthezian( m_eye.x, m_eye.y, m_eye.z );
        m_engine->setEyePos( m_eye );
    }

    float r = m_cameraPosSp.getR();
    if( ImGui::SliderFloat( "R", &r, 1.f, 32.f ) )
    {
        m_cameraPosSp.setR( r );
        m_cameraPosSp.toCarthezian( m_eye.x, m_eye.y, m_eye.z );
        m_engine->setEyePos( m_eye );
    }

    ImGui::End();
}

void ShaderEditor::drawEditor( float x, float y, float w, float h, const CUL::String& name )
{
    (void)x;
    (void)y;

    EditorState& editorState = *m_editors[name.string()];

    ImGuiWindowFlags window_flags{ 0 };
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;

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
            editorState.File->loadBackground( true, true,
                                              [&editorState, this]()
                                              {
                                                  editorState.CachedText = editorState.File->getAsOneString().string();
                                                  editorState.CachedText.removeTrailingLineEnd();
                                                  editorState.Editor.SetReadOnly( false );
                                                  editorState.Editor.SetText( editorState.CachedText.string() );
                                                  editorState.ShaderUnitState = EShaderUnitState::Loaded;
                                              } );
        }
    }

    ImGui::SameLine();

    if( !editorState.File || editorState.File->getPath().getIsEmpty() )
    {
        ImGui::Text( "Choose path." );
    }
    else
    {
        const CUL::String filePath = editorState.File->getPath().getPath();
        const char* tempChar = filePath.cStr();
        ImGui::Text( "%s", tempChar );
    }

    if( ImGui::Button( "Save" ) )
    {
        editorState.File->overwriteContents( editorState.CachedText );
        editorState.File->saveFile();
    }

    ImGui::SameLine();

    static CUL::String compilerError;
    if( ImGui::Button( "Compile" ) )
    {
        if( editorState.File )
        {
            editorState.File->overwriteContents( editorState.CachedText );
            editorState.File->saveFile();

            m_vao->getProgram()->reCompileShader( LOGLW::EExecuteType::Now, editorState.File->getPath(), false, compilerError );
            if( compilerError.empty() )
            {
                editorState.ShaderUnitState = EShaderUnitState::Compiled;
                compilerError.clear();
            }
            else
            {
                editorState.ShaderUnitState = EShaderUnitState::Error;
            }
        }
    }

    ImGui::Button( "Status:" );
    ImGui::SameLine();

    ImVec4 color;
    color.w = 0.5f;
    const float colorAmp = 1.0f;
    CUL::String status;

    if( editorState.ShaderUnitState == EShaderUnitState::Empty )
    {
        status = "Empty";
    }
    else if( editorState.ShaderUnitState == EShaderUnitState::Loaded )
    {
        color.x = colorAmp;
        color.y = colorAmp;
        status = "Loaded";
    }
    else if( editorState.ShaderUnitState == EShaderUnitState::Compiled )
    {
        color.y = colorAmp;
        status = "Compiled";
    }
    else if( editorState.ShaderUnitState == EShaderUnitState::Error )
    {
        color.x = colorAmp;
        status = "Error";
    }

    ImGui::PushStyleColor( ImGuiCol_Button, color );
    ImGui::PushStyleColor( ImGuiCol_ButtonHovered, color );
    ImGui::PushStyleColor( ImGuiCol_ButtonActive, color );
    ImGui::Button( status.cStr() );
    ImGui::PopStyleColor( 3 );

    if( editorState.CachedText.empty() == false )
    {
        CUL::String editorText = editorState.Editor.GetText();
        editorText.removeTrailingLineEnd();
        if( editorText != editorState.CachedText )
        {
            m_engine->getLoger()->log( "editorText:" );
            m_engine->getLoger()->log( editorText );
            m_engine->getLoger()->log( "--------" );

            m_engine->getLoger()->log( "editorState.CachedText:" );
            m_engine->getLoger()->log( editorState.CachedText );
            m_engine->getLoger()->log( "--------" );

            editorState.CachedText = editorText;
        }
    }
    LOGLW::ShaderProgram* program = m_vao->getProgram();
    if( program && program->getIsLinked() )
    {
        const LOGLW::Camera& camera = m_engine->getCamera();
        const glm::mat4 projectionMatrix = camera.getProjectionMatrix();
        const glm::mat4 viewMatrix = camera.getViewMatrix();

        m_transformComponent->setSize( { 1.f, 1.f, 1.f } );
        m_transformComponent->setPositionAbsolute( { 1.f, 1.f, 0.f } );
        const glm::mat4 model = m_transformComponent->getModel();

        const auto targetModel = projectionMatrix * viewMatrix * model;

        program->setUniform( LOGLW::EExecuteType::Now, "projection", projectionMatrix );
        program->setUniform( LOGLW::EExecuteType::Now, "view", viewMatrix );
        program->setUniform( LOGLW::EExecuteType::Now, "model", model );
    }

    editorState.Editor.Render( name.cStr(), ImVec2( w, h ) );

    if( editorState.ShaderUnitState == EShaderUnitState::Error )
    {
        ImGui::TextWrapped( compilerError.cStr() );
    }
}

void ShaderEditor::onMouseEvent( const LOGLW::MouseData& mouseData )
{
    if( mouseData.getEventType() == LOGLW::MouseData::EventType::MOUSEWHEEL )
    {
        const auto whellY = mouseData.getWheelY();
        m_engine->getLoger()->logVariable( CUL::LOG::Severity::Warn, "ShaderEditor::onMouseEvent MOUSEWHEEL y: %d", whellY );
        const float whellYf = 0.8f * static_cast<float>( whellY );
        m_cameraPosSp.incrementR( -whellYf );
        m_cameraPosSp.toCarthezian( m_eye.x, m_eye.y, m_eye.z );
        m_engine->setEyePos( m_eye );
    }

    if( mouseData.isButtonDown( 3 ) )
    {
        const auto& md = m_engine->getMouseData();
        const std::int32_t mouseX = md.getX();
        const std::int32_t mouseY = md.getY();

        if( m_mouseDown )
        {
            const std::int32_t diffX = m_lastMouseX - mouseX;
            const std::int32_t diffY = m_lastMouseY - mouseY;

            m_engine->getLoger()->logVariable( CUL::LOG::Severity::Warn, "ShaderEditor::onMouseEvent DIFF x: %d, y: %d", diffX, diffY );

            constexpr float divider{ 255.f };
            m_cameraPosSp.incrementTheta( static_cast<float>( diffX ) / divider );
            m_cameraPosSp.incrementPhi( -static_cast<float>( diffY ) / divider );
            m_cameraPosSp.toCarthezian( m_eye.x, m_eye.y, m_eye.z );

            m_engine->setEyePos( m_eye );

            m_lastMouseX = mouseX;
            m_lastMouseY = mouseY;
        }
        else
        {
            m_mouseDown = true;
            m_lastMouseX = mouseX;
            m_lastMouseY = mouseY;
            return;
        }
    }
    else if( mouseData.isButtonDown( 3 ) == false )
    {
        m_mouseDown = false;
    }
}

void ShaderEditor::onKeyBoardEvent( const LOGLW::KeyboardState& /*key*/ )
{
}

void ShaderEditor::onWindowEvent( const LOGLW::WindowEvent::Type type )
{
    if( LOGLW::WindowEvent::Type::CLOSE == type )
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
