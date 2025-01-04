#pragma once

#include "gameengine/IUtilityUser.hpp"
#include "gameengine/Shaders/ShaderTypes.hpp"

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
        std::variant<float, std::uint32_t, std::int32_t> Value;
        std::uint32_t Id = 0u;
    };

    ShaderProgram();

    unsigned int getId() const;
    void useShader() const;
    void reload();

    void loadShader( const String& shaderPath );

    void setUniform( const String& name, const char* value );
    void setUniform( const String& name, float value );
    void setUniform( const String& name, unsigned value );
    void setUniform( const String& name, int value );
    void setUniform( const String& name, bool value );
    void setUniform( const String& name, const glm::mat2& mat );
    void setUniform( const String& name, const glm::mat3& mat );
    void setUniform( const String& name, const glm::mat4& mat );
    void setUniform( const String& name, const glm::vec3& value );
    void setUniform( const String& name, const glm::vec4& value );

    String getAttributeStr( const String& name );
    float getAttributeF( const String& name );
    unsigned int getAttributeUi( const String& name );
    int getAttributeI( const String& name );

    CShaderTypes::ShaderType getType() const;
    void attachShaders();
    void attachShader( std::uint32_t id );
    void link();
    void validate();

    void enable();
    void disable();

    EShaderUnitState getShaderUnitState( CShaderTypes::ShaderType inType ) const;

    ~ShaderProgram();

protected:
private:
    void create();
    void release();
    std::int32_t getUniformLocation( const String& name ) const;

    CShaderTypes::ShaderType m_type{ CShaderTypes::ShaderType::Unkown };
    uint8_t m_shaderProgramId = 0u;

    class Shader;
    std::unordered_map<CShaderTypes::ShaderType, std::unique_ptr<ShaderUnit>> m_shaders;

    IGameEngine& m_engine;
    std::uint32_t m_id = 0;

    mutable std::unordered_map<String, ShaderVariable, CUL::StringHash> m_uniformMapping;

    ShaderProgram( const ShaderProgram& arg ) = delete;
    ShaderProgram( ShaderProgram&& arg ) = delete;
    ShaderProgram& operator=( const ShaderProgram& rhv ) = delete;
    ShaderProgram& operator=( ShaderProgram&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )
