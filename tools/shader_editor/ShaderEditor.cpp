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
#include "gameengine/Shaders/ShaderState.hpp"
#include "gameengine/VertexArray.hpp"

#include "gameengine/Windowing/IWindow.hpp"
#include "gameengine/Input/MouseData.hpp"
#include <gameengine/Primitives/Sphere.hpp>

#include "LOGLWAdditionalDeps/ImportImgui.hpp"

#include "CUL/ITimer.hpp"
#include "CUL/Filesystem/PathDialog.hpp"
#include "CUL/Filesystem/RegularFile.hpp"
#include "CUL/Math/Utils.hpp"
#include "CUL/Math/Constants.hpp"
#include "CUL/Threading/ThreadUtil.hpp"

#include "CUL/STL_IMPORTS/STD_cmath.hpp"
#include "CUL/STL_IMPORTS/STD_algorithm.hpp"
#include "CUL/STL_IMPORTS/STD_cctype.hpp"

CUL::MATH::Angle ang90( 90, CUL::MATH::Angle::Type::DEGREE );
CUL::MATH::Angle ang180( 180, CUL::MATH::Angle::Type::DEGREE );
CUL::MATH::Angle ang270( 270, CUL::MATH::Angle::Type::DEGREE );

struct EditorState
{
    String Name;
    TextEditor Editor;
    String Extension;
    std::unique_ptr<CUL::FS::RegularFile> File;
    String CachedText;
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
    m_engine->toggleDrawDebugInfo( true );
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
    m_editors.insert( { pixelShader->Name.getSTDString(), std::move( pixelShader ) } );

    std::unique_ptr<EditorState> vertexShader = std::make_unique<EditorState>();
    vertexShader->Extension = "vert";
    vertexShader->Name = "vertex_shader";
    vertexShader->Editor.SetReadOnly( true );
    m_editors.insert( { vertexShader->Name.getSTDString(), std::move( vertexShader ) } );

    for( auto& [currentName, currentPtr] : m_editors )
    {
        currentPtr->Editor.SetLanguageDefinition( languageDefinition );
    }

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

    LOGLW::ShaderData sd;
    sd.ShaderName = "ShaderEditor::shader";
    sd.FragmentShader = "fragmentShader.frag";
    sd.VertexShader = "vertexShader.vert";
    m_shader = m_engine->createProgram( LOGLW::EExecuteType::WaitForCompletion, sd );
    m_transformComponent = std::make_unique<LOGLW::TransformComponent>( nullptr );

    m_sphere = new LOGLW::CSphere( nullptr );
    m_sphere->setProgram( m_shader );
}

void ShaderEditor::timer()
{
}

void ShaderEditor::guiIteration( float x, float y )
{
    drawLeftWindow( x, y );
    //drawRightWindow( x, y );
}

void ShaderEditor::drawLeftWindow( float x, float /*y*/ )
{
    const static String name = "Left";
    ImGui::Begin( *name,
                  nullptr,
                  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar );

    auto winSize = m_engine->getMainWindow()->getSize();
    const auto targetHeight = (float)winSize.H * 1.0f;
    const float menuWidth = (float)winSize.W * 0.4f;
    const float editorWidth = menuWidth / 1.f;
    const float editorHeight = targetHeight * 0.5f;
    // const float windowX = (float)winSize.w - menuWidth; // Left side.
    const float windowX = (float)winSize.W - editorWidth;

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
        m_shader->link( LOGLW::EExecuteType::Now );
    }

    if( ImGui::Button( "Validate" ) )
    {
        m_shader->validate();
    }

    if( m_shader->getIsLinked() )
    {
        if( ImGui::TreeNode( "Uniforms" ) )
        {
            if( ImGui::Button( "Add Uniform" ) )
            {
                ImGui::OpenPopup( "Create_Uniform" );
            }
            drawCreateUniformPopup();

            const std::vector<String> uniformNames = m_shader->getUniformsNames();
            for( const String& uniformName : uniformNames )
            {
                const auto& uniformVal = m_shader->getUniformValue( uniformName );
                ImGui::Text( "ID: %d, Name: %s, Type: %s",
                             uniformVal.Id,
                             *uniformVal.Name,
                             *uniformVal.TypeName );
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
                    const glm::mat4 currentValue =
                        std::get<glm::mat4>( uniformVal.Value );
                    constexpr std::size_t bufferSize{ 32u };
                    char tableName[bufferSize];
                    snprintf( tableName,
                              bufferSize,
                              "%s - %s",
                              *m_shader->getName(),
                              *uniformName );

                    static ImGuiTableFlags flags =
                        ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable |
                        ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
                        ImGuiTableFlags_ContextMenuInBody;
                    if( ImGui::BeginTable( tableName, 4, flags ) )
                    {
                        std::int32_t rowId{ 0 };
                        for( std::int32_t row = 0; row < 4; ++row )
                        {
                            ImGui::PushID( rowId++ );
                            ImGui::TableNextRow();
                            std::int32_t cellId{ 0 };
                            for( std::int32_t column = 0; column < 4; ++column )
                            {
                                ImGui::PushID( cellId++ );
                                ImGui::TableSetColumnIndex( column );
                                float f0 = currentValue[row][column];
                                ImGui::DragFloat( "df", &f0, 0.005f );
                                ImGui::PopID();
                            }
                            ImGui::PopID();
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

namespace
{
bool isIdentChar( char c )
{
    return std::isalnum( static_cast<unsigned char>( c ) ) != 0 || c == '_';
}

LOGLW::DataType glslTypeToDataType( const std::string& type )
{
    if( type == "float" )
        return LOGLW::DataType::FLOAT;
    if( type == "int" )
        return LOGLW::DataType::INT;
    if( type == "vec2" )
        return LOGLW::DataType::FLOAT_VEC2;
    if( type == "vec3" )
        return LOGLW::DataType::FLOAT_VEC3;
    if( type == "vec4" )
        return LOGLW::DataType::FLOAT_VEC4;
    if( type == "mat2" )
        return LOGLW::DataType::FLOAT_MAT2;
    if( type == "mat3" )
        return LOGLW::DataType::FLOAT_MAT3;
    if( type == "mat4" )
        return LOGLW::DataType::FLOAT_MAT4;
    if( type == "sampler1D" )
        return LOGLW::DataType::SAMPLER_1D;
    if( type == "sampler2D" )
        return LOGLW::DataType::SAMPLER_2D;
    return LOGLW::DataType::NONE;
}

void appendUniformsFromSource( const std::string& source,
                               std::vector<std::pair<std::string, std::string>>& out )
{
    std::size_t pos = 0;
    while( pos < source.size() )
    {
        const std::size_t u = source.find( "uniform", pos );
        if( u == std::string::npos )
            break;

        const bool boundaryBefore =
            u == 0 || !isIdentChar( source[u - 1] );
        const bool boundaryAfter =
            u + 7 >= source.size() || !isIdentChar( source[u + 7] );
        if( !boundaryBefore || !boundaryAfter )
        {
            pos = u + 7;
            continue;
        }

        // Skip if this "uniform" sits on a // comment line.
        const std::size_t lineStart = source.rfind( '\n', u );
        const std::size_t commentPos = source.find(
            "//", lineStart == std::string::npos ? 0 : lineStart );
        if( commentPos != std::string::npos && commentPos < u )
        {
            pos = u + 7;
            continue;
        }

        std::size_t i = u + 7;
        while( i < source.size() &&
               std::isspace( static_cast<unsigned char>( source[i] ) ) )
            ++i;

        const std::size_t typeStart = i;
        while( i < source.size() && isIdentChar( source[i] ) )
            ++i;
        const std::string type = source.substr( typeStart, i - typeStart );

        while( i < source.size() &&
               std::isspace( static_cast<unsigned char>( source[i] ) ) )
            ++i;

        const std::size_t nameStart = i;
        while( i < source.size() && isIdentChar( source[i] ) )
            ++i;
        const std::string name = source.substr( nameStart, i - nameStart );

        // Skip uniform blocks: uniform ... Name {
        std::size_t look = i;
        while( look < source.size() &&
               std::isspace( static_cast<unsigned char>( source[look] ) ) )
            ++look;
        const bool isBlock = look < source.size() && source[look] == '{';

        if( !isBlock && !type.empty() && !name.empty() )
        {
            const bool already =
                std::any_of( out.begin(),
                             out.end(),
                             [&name]( const auto& p )
                             {
                                 return p.second == name;
                             } );
            if( !already )
                out.emplace_back( type, name );
        }

        pos = i;
    }
}
}  // namespace

std::vector<std::pair<std::string, std::string>>
ShaderEditor::parseDeclaredUniforms() const
{
    std::vector<std::pair<std::string, std::string>> declared;
    for( const auto& [editorName, editorState] : m_editors )
    {
        (void)editorName;
        if( !editorState )
            continue;
        appendUniformsFromSource( editorState->CachedText.getSTDString(),
                                  declared );
    }
    return declared;
}

void ShaderEditor::drawCreateUniformPopup()
{
    static char nameBuf[64]{};
    static float floatVal{ 0.f };
    static int intVal{ 0 };
    static float vec3Val[3]{ 0.f, 0.f, 0.f };
    static float vec4Val[4]{ 1.f, 1.f, 1.f, 1.f };
    static char statusMsg[192]{};

    if( !ImGui::BeginPopupModal( "Create_Uniform",
                                 nullptr,
                                 ImGuiWindowFlags_AlwaysAutoResize ) )
    {
        return;
    }

    ImGui::TextUnformatted(
        "Set an active uniform (OpenGL drops unused ones)." );
    ImGui::Separator();

    const auto declared = parseDeclaredUniforms();
    const auto activeNames = m_shader->getUniformsNames();

    if( !declared.empty() &&
        ImGui::BeginCombo( "Declared",
                           nameBuf[0] != '\0' ? nameBuf : "Select..." ) )
    {
        for( const auto& [glslType, glslName] : declared )
        {
            const bool isActive =
                std::find( activeNames.begin(),
                           activeNames.end(),
                           String( glslName.c_str() ) ) != activeNames.end();
            char label[96];
            snprintf( label,
                      IM_ARRAYSIZE( label ),
                      "%s %s%s",
                      glslType.c_str(),
                      glslName.c_str(),
                      isActive ? "" : " (optimized out)" );
            const bool selected = ( glslName == nameBuf );
            if( ImGui::Selectable( label, selected ) )
            {
                snprintf( nameBuf, IM_ARRAYSIZE( nameBuf ), "%s", glslName.c_str() );
                statusMsg[0] = '\0';
            }
            if( selected )
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    const bool enterPressed = ImGui::InputText(
        "Name",
        nameBuf,
        IM_ARRAYSIZE( nameBuf ),
        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsNoBlank );

    const String uniformName = nameBuf;
    const bool isActive =
        nameBuf[0] != '\0' &&
        std::find( activeNames.begin(), activeNames.end(), uniformName ) !=
            activeNames.end();

    const auto declaredIt = std::find_if(
        declared.begin(),
        declared.end(),
        [&]( const auto& p )
        {
            return p.second == nameBuf;
        } );
    const bool isDeclared = declaredIt != declared.end();

    LOGLW::DataType dataType{ LOGLW::DataType::NONE };
    std::string typeLabel = "-";

    if( isActive )
    {
        const auto& uniformVal = m_shader->getUniformValue( uniformName );
        dataType = uniformVal.Type;
        typeLabel = *uniformVal.TypeName;
        ImGui::Text( "Type: %s (active)", typeLabel.c_str() );
    }
    else if( isDeclared )
    {
        typeLabel = declaredIt->first;
        dataType = glslTypeToDataType( declaredIt->first );
        ImGui::Text( "Type: %s (from source)", typeLabel.c_str() );
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.f, 0.75f, 0.2f, 1.f ) );
        ImGui::TextWrapped(
            "Declared but not active. OpenGL optimized it out "
            "(unused). Use it in the shader, then Compile + Link." );
        ImGui::PopStyleColor();
    }
    else if( nameBuf[0] != '\0' )
    {
        ImGui::TextDisabled( "Type: (not declared / not active)" );
    }
    else
    {
        ImGui::TextDisabled( "Type: -" );
    }

    const bool canEdit = isActive;
    if( canEdit )
    {
        switch( dataType )
        {
            case LOGLW::DataType::FLOAT:
                ImGui::DragFloat( "Value", &floatVal, 0.01f );
                break;
            case LOGLW::DataType::SAMPLER_1D:
            case LOGLW::DataType::SAMPLER_2D:
                ImGui::DragInt( "Value", &intVal, 1.f );
                break;
            case LOGLW::DataType::FLOAT_VEC3:
                ImGui::DragFloat3( "Value", vec3Val, 0.01f );
                break;
            case LOGLW::DataType::FLOAT_VEC4:
                ImGui::ColorEdit4( "Value",
                                   vec4Val,
                                   ImGuiColorEditFlags_Float |
                                       ImGuiColorEditFlags_HDR );
                break;
            case LOGLW::DataType::FLOAT_MAT4:
                ImGui::TextDisabled( "Uses identity matrix." );
                break;
            default:
                ImGui::TextDisabled( "Editing this type is not supported yet." );
                break;
        }
    }

    if( statusMsg[0] != '\0' )
    {
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.f, 0.35f, 0.35f, 1.f ) );
        ImGui::TextWrapped( "%s", statusMsg );
        ImGui::PopStyleColor();
    }

    const auto tryCreate = [&]()
    {
        statusMsg[0] = '\0';
        if( nameBuf[0] == '\0' )
        {
            snprintf( statusMsg, IM_ARRAYSIZE( statusMsg ), "Name is required." );
            return;
        }
        if( !isActive )
        {
            if( isDeclared )
            {
                snprintf( statusMsg,
                          IM_ARRAYSIZE( statusMsg ),
                          "'%s' was optimized out. Reference it in the shader, "
                          "then Compile + Link.",
                          nameBuf );
            }
            else
            {
                snprintf( statusMsg,
                          IM_ARRAYSIZE( statusMsg ),
                          "'%s' not found in source or linked program.",
                          nameBuf );
            }
            return;
        }

        LOGLW::UniformValue value;
        switch( dataType )
        {
            case LOGLW::DataType::FLOAT:
                value = floatVal;
                break;
            case LOGLW::DataType::SAMPLER_1D:
            case LOGLW::DataType::SAMPLER_2D:
                value = intVal;
                break;
            case LOGLW::DataType::FLOAT_VEC3:
                value = glm::vec3( vec3Val[0], vec3Val[1], vec3Val[2] );
                break;
            case LOGLW::DataType::FLOAT_VEC4:
                value = glm::vec4( vec4Val[0], vec4Val[1], vec4Val[2], vec4Val[3] );
                break;
            case LOGLW::DataType::FLOAT_MAT4:
                value = glm::mat4( 1.f );
                break;
            default:
                snprintf( statusMsg,
                          IM_ARRAYSIZE( statusMsg ),
                          "Unsupported type: %s",
                          typeLabel.c_str() );
                return;
        }

        m_shader->setUniform( LOGLW::EExecuteType::Now, uniformName, value );

        nameBuf[0] = '\0';
        floatVal = 0.f;
        intVal = 0;
        vec3Val[0] = vec3Val[1] = vec3Val[2] = 0.f;
        vec4Val[0] = vec4Val[1] = vec4Val[2] = vec4Val[3] = 1.f;
        ImGui::CloseCurrentPopup();
    };

    if( ImGui::Button( "Create", ImVec2( 120.f, 0.f ) ) || enterPressed )
        tryCreate();
    ImGui::SameLine();
    if( ImGui::Button( "Cancel", ImVec2( 120.f, 0.f ) ) )
    {
        statusMsg[0] = '\0';
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

void ShaderEditor::drawRightWindow( float /*x*/, float /*y*/ )
{
    const static String name = "Right";
    ImGui::Begin( *name,
                  nullptr,
                  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar );

    auto winSize = m_engine->getMainWindow()->getSize();
    const auto targetHeight = (float)winSize.H * 1.0f;
    const float menuWidth = (float)winSize.W * 0.1f;
    const float rightPadding = menuWidth + 0.f;
    const float windowX = (float)winSize.W - rightPadding;  // Right side.

    ImGui::SetWindowPos( { windowX, 2.f } );
    ImGui::SetWindowSize( { menuWidth, targetHeight } );

    LOGLW::Camera& camera = m_engine->getCamera();
    glm::vec3 eyePos = camera.getEye();

    ImGui::Text( "Eye pos: x: %f, y: %f, z: %f", eyePos.x, eyePos.y, eyePos.z );
    constexpr float toDeg = 180.f / CUL::Math::PI_F;
    ImGui::Text( "Eye pos in spherical [rad]: theta: %f, phi: %f, r: %f",
                 m_cameraPosSp.getTheta(),
                 m_cameraPosSp.getPhi(),
                 m_cameraPosSp.getR() );
    ImGui::Text( "Eye pos in spherical [deg]: theta: %f, phi: %f, r: %f",
                 m_cameraPosSp.getTheta() * toDeg,
                 m_cameraPosSp.getPhi() * toDeg,
                 m_cameraPosSp.getR() );

    float theta = m_cameraPosSp.getTheta();
    if( ImGui::SliderFloat(
            "Theta Rad", &theta, 0.0f, CUL::MATH::SphericalCoord<float>::TwoPi * 0.5f ) )
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
    if( ImGui::SliderFloat(
            "Phi Rad", &phi, 0.f, CUL::MATH::SphericalCoord<float>::TwoPi * 0.5f ) )
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

void ShaderEditor::drawEditor( float x, float y, float w, float h, const String& name )
{
    (void)x;
    (void)y;

    EditorState& editorState = *m_editors[name.getSTDString()];

    ImGuiWindowFlags window_flags{ 0 };
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;

    if( ImGui::Button( "Press to find file." ) )
    {
        CUL::FS::PathDialog::Filter filter;
        filter.Name = *editorState.Name;
        filter.Spec = *editorState.Extension;
        const CUL::StringWr choosenShader = CUL::FS::PathDialog::getInstance().openDialog( filter ).getString();
        if( choosenShader.empty() == false )
        {
            editorState.Editor.SetReadOnly( true );
            editorState.File = std::make_unique<CUL::FS::RegularFile>( choosenShader, m_engine->getCul() );
            editorState.File->setPath( choosenShader );
            editorState.File->loadBackground( true, true,
                                              [&editorState, this]()
                                              {
                                                  editorState.CachedText = editorState.File->getAsOneString().getValue();
                                                  editorState.Editor.SetReadOnly( false );
                                                  editorState.Editor.SetText( editorState.CachedText.getSTDString() );
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
        const String filePath = editorState.File->getPath().getPath();
        const char* tempChar = *filePath;
        ImGui::Text( "%s", tempChar );
    }

    if( ImGui::Button( "Save" ) )
    {
        editorState.File->overwriteContents( editorState.CachedText );
        editorState.File->saveFile();
    }

    ImGui::SameLine();

    static String compilerError;
    if( ImGui::Button( "Compile" ) )
    {
        if( editorState.File )
        {
            editorState.File->overwriteContents( editorState.CachedText );
            editorState.File->saveFile();

            auto result = m_shader->reCompileShader( LOGLW::EExecuteType::Now, editorState.File->getPath(), false );
            if( result.State == LOGLW::EShaderUnitState::Error )
            {
                editorState.ShaderUnitState = EShaderUnitState::Error;
                compilerError = result.ErrorInfo;
            }
            else
            {
                editorState.ShaderUnitState = EShaderUnitState::Compiled;
            }
        }
    }

    ImGui::Button( "Status:" );
    ImGui::SameLine();

    ImVec4 color;
    color.w = 0.5f;
    const float colorAmp = 1.0f;
    String status;

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
    ImGui::Button( *status );
    ImGui::PopStyleColor( 3 );

    if( editorState.CachedText.empty() == false )
    {
        String editorText = editorState.Editor.GetText();
        editorText.trim( CUL::TrimType::End );
        if( editorText != editorState.CachedText )
        {
            editorState.CachedText = editorText;
        }
    }
    if( m_shader->getIsLinked() )
    {
        const LOGLW::Camera& camera = m_engine->getCamera();
        const glm::mat4 projectionMatrix = camera.getProjectionMatrix();
        const glm::mat4 viewMatrix = camera.getViewMatrix();

        m_transformComponent->setSize( { 1.f, 1.f, 1.f } );
        m_transformComponent->setPositionAbsolute( { 1.f, 1.f, 0.f } );
        const glm::mat4 model = m_transformComponent->getModel();

        m_shader->setUniform( LOGLW::EExecuteType::Now, "projection", projectionMatrix );
        m_shader->setUniform( LOGLW::EExecuteType::Now, "view", viewMatrix );
        m_shader->setUniform( LOGLW::EExecuteType::Now, "model", model );
    }

    editorState.Editor.Render( *name, ImVec2( w, h ) );

    if( editorState.ShaderUnitState == EShaderUnitState::Error )
    {
        ImGui::TextWrapped( "%s", *compilerError );
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
