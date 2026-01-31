#pragma once

#include "gameengine/IUtilityUser.hpp"
#include "gameengine/Shaders/ShaderTypes.hpp"
#include "gameengine/BasicTypes.hpp"
#include "gameengine/IRenderDevice.hpp"

#include "CUL/IName.hpp"
#include "CUL/IRegisteredObject.hpp"
#include "CUL/Filesystem/Path.hpp"
#include "CUL/Filesystem/IFile.hpp"
#include "CUL/Task/TaskAccumulator.hpp"

#include "CUL/STL_IMPORTS/STD_vector.hpp"
#include "CUL/STL_IMPORTS/STD_variant.hpp"

#include "CUL/IMPORT_GLM.hpp"

NAMESPACE_BEGIN( LOGLW )

class IGameEngine;
enum class EExecuteType : std::int8_t;
enum class EShaderUnitState : std::int8_t;
struct ShaderUnit;

using String = CUL::StringWr;
enum class EShaderProgramState : std::int8_t
{
    Unkown = -1,
    Ok,
    LinkError
};

class GAME_ENGINE_API ShaderProgram final:
    private IUtilityUser,
    public CUL::IName,
    public CUL::IRegisterdObject
{
public:
    struct ShaderVariable
    {
        String Name;
        UniformValue Value;
        std::uint32_t Id{ 0u };
        DataType Type{ DataType::NONE };
        String TypeName;
        std::int32_t Size{ 0 };
        bool Applied{ false };
    };

    struct ShadersData
    {
        String VertexShader;
        String FragmentShader;
    };

    ShaderProgram();
    void createFrom( EExecuteType inEt, const ShadersData& inShaderData );

    unsigned int getId() const;

    void compileLinkValidate( EExecuteType inEt );
    void compileShader( EExecuteType inEt, const String& shaderPath );
    void compileShader( EExecuteType inEt, const String& shaderPath, bool assertOnErrors, String& errorMessage );
    void reCompileShader( EExecuteType inEt, const String& shaderPath );
    void reCompileShader( EExecuteType inEt, const String& shaderPath, bool assertOnErrors, String& errorMessage );
    void reCompileWholeShader( EExecuteType inEt );

    void setUniform( EExecuteType inEt, const String& inName, UniformValue inValue );

    String getAttributeStr( const String& name );
    float getAttributeF( const String& name );
    unsigned int getAttributeUi( const String& name );
    int getAttributeI( const String& name );

    CShaderTypes::ShaderType getType() const;
    void attachShaders( EExecuteType inEt );
    void attachShader( EExecuteType inEt, std::uint32_t id );
    void detachShader( EExecuteType inEt, std::uint32_t id );
    void link( EExecuteType inEt );
    bool getIsLinked() const;

    void validate();

    void enable();
    void disable();

    EShaderUnitState getShaderUnitState( CShaderTypes::ShaderType inType ) const;
    const ShaderVariable& getUniformValue( const String& name );
    std::vector<String> getUniformsNames();
    void runOnRenderingThread( std::function<void( void )> inFunction );

    ~ShaderProgram();

protected:
    void onNameChange( const String& newName ) override;

private:
    void create();
    void createFromImpl( EExecuteType inEt, const ShadersData& inShaderData );
    void reCompileWholeShaderImpl( EExecuteType inEt );
    void releaseShaderUnits();
    void reCompileShaderImpl( EExecuteType inEt, const String& shaderPath, bool assertOnErrors, String& errorMessage );
    void setUniformImpl( const String& inName, UniformValue inValue );
    void linkImpl();
    void release();
    std::int32_t getUniformLocation( const String& name ) const;

    CShaderTypes::ShaderType m_type{ CShaderTypes::ShaderType::Unkown };
    std::uint32_t m_shaderProgramId{ 0u };

    class Shader;
    std::unordered_map<CShaderTypes::ShaderType, ShaderUnit*> m_shaders;

    IGameEngine& m_engine;

    mutable std::unordered_map<std::string, ShaderVariable> m_uniformMapping;
    mutable std::unordered_map<std::string, ShaderVariable> m_attributeMapping;

    bool m_linked{ false };
    CUL::CTaskAccumulator m_tasks;

    ShaderProgram( const ShaderProgram& arg ) = delete;
    ShaderProgram( ShaderProgram&& arg ) = delete;
    ShaderProgram& operator=( const ShaderProgram& rhv ) = delete;
    ShaderProgram& operator=( ShaderProgram&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )
