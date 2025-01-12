#pragma once

#include "gameengine/IUtilityUser.hpp"
#include "gameengine/Shaders/ShaderTypes.hpp"
#include "gameengine/BasicTypes.hpp"
#include "gameengine/IRenderDevice.hpp"

#include "CUL/IName.hpp"
#include "CUL/IRegisteredObject.hpp"
#include "CUL/Filesystem/Path.hpp"
#include "CUL/Filesystem/IFile.hpp"

#include "CUL/STL_IMPORTS/STD_vector.hpp"
#include "CUL/STL_IMPORTS/STD_variant.hpp"

#include "CUL/IMPORT_GLM.hpp"

NAMESPACE_BEGIN( LOGLW )

class IGameEngine;
enum class EShaderUnitState : std::int8_t;
struct ShaderUnit;

using String = CUL::String;
enum class EShaderProgramState : std::int8_t
{
    Unkown = -1,
    Ok,
    LinkError
};

class GAME_ENGINE_API ShaderProgram final: private IUtilityUser, public CUL::IName, public CUL::IRegisterdObject
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

    ShaderProgram();

    unsigned int getId() const;
    void useShader() const;
    void reload();

    void compileLinkValidate();
    void compileShader( const String& shaderPath );
    void compileShader( const String& shaderPath, bool assertOnErrors, CUL::String& errorMessage );
    void reCompileShader( const String& shaderPath );
    void reCompileShader( const String& shaderPath, bool assertOnErrors, CUL::String& errorMessage );
    void reCompileWholeShader();

    void setUniform( const String& inName, UniformValue inValue );

    String getAttributeStr( const String& name );
    float getAttributeF( const String& name );
    unsigned int getAttributeUi( const String& name );
    int getAttributeI( const String& name );

    CShaderTypes::ShaderType getType() const;
    void attachShaders();
    void attachShader( std::uint32_t id );
    void detachShader( std::uint32_t id );
    void link();
    bool getIsLinked() const;

    void validate();

    void enable();
    void disable();

    EShaderUnitState getShaderUnitState( CShaderTypes::ShaderType inType ) const;
    const ShaderVariable& getUniformValue( const CUL::String& name );
    std::vector<CUL::String> getUniformsNames();

    ~ShaderProgram();

protected:
private:
    void create();
    void setUniformImpl( const String& inName, UniformValue inValue );
    void linkImpl();
    void release();
    std::int32_t getUniformLocation( const String& name ) const;

    CShaderTypes::ShaderType m_type{ CShaderTypes::ShaderType::Unkown };
    std::int32_t m_shaderProgramId{ -1 };

    class Shader;
    std::unordered_map<CShaderTypes::ShaderType, ShaderUnit*> m_shaders;

    IGameEngine& m_engine;

    mutable std::unordered_map<String, ShaderVariable, CUL::StringHash> m_uniformMapping;
    mutable std::unordered_map<String, ShaderVariable, CUL::StringHash> m_attributeMapping;

    bool m_linked{ false };

    ShaderProgram( const ShaderProgram& arg ) = delete;
    ShaderProgram( ShaderProgram&& arg ) = delete;
    ShaderProgram& operator=( const ShaderProgram& rhv ) = delete;
    ShaderProgram& operator=( ShaderProgram&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )
