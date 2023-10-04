#pragma once

#include "gameengine/IRenderDevice.hpp"
#include "gameengine/Shader.hpp"

#include "CUL/Filesystem/Path.hpp"

#include "CUL/STL_IMPORTS/STD_map.hpp"
#include "CUL/STL_IMPORTS/STD_vector.hpp"
#include "CUL/STL_IMPORTS/STD_variant.hpp"
#include "CUL/STL_IMPORTS/STD_deque.hpp"
#include "CUL/STL_IMPORTS/STD_mutex.hpp"

#include "CUL/IMPORT_GLM.hpp"

NAMESPACE_BEGIN( CUL )
NAMESPACE_BEGIN( LOG )

class ILogger;

NAMESPACE_END( LOG )
NAMESPACE_END( CUL )

NAMESPACE_BEGIN( LOGLW )

using String = CUL::String;

class VertexArray;
class IGameEngine;

class GAME_ENGINE_API Program final: private IUtilityUser
{
public:
    struct Pair
    {
        String name;
        std::variant<float, unsigned, int> value;
    };
    using ValueToSet = Pair;

    Program( IGameEngine& engine );

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

    Shader* loadShader( const char* path );

    String getAttributeStr( const String& name );
    float getAttributeF( const String& name );
    unsigned int getAttributeUi( const String& name );
    int getAttributeI( const String& name );

    void attachShader( Shader* shader );
    void dettachShader( Shader* shader );
    void link();
    void enable();
    void disable();
    void validate();

    void reloadShader();

    void bufferData(
        std::vector<float>& data,
        const BufferTypes type );

    unsigned int getProgramId();

    bool initialized() const;
    void initialize();

    void render();
    void goThroughTasks();

    ~Program();
protected:

private:
    void reloadShaderImpl();
    void attachShaderImpl( Shader* shader );
    void dettachShaderImpl( Shader* shader );
    void release();
    void releaseProgram();

    void pushTask( ValueToSet& task );
    
    void processTask( const ValueToSet& task );

    int getUniformLocation( const String& name );

    IGameEngine& m_engine;

    unsigned int m_dataBufferId = 0u;
    unsigned int m_id = 0u;

    std::map<ShaderTypes::ShaderType, Shader*> m_attachedShaders;

    std::map<String, unsigned> m_uniformMap;

    std::deque<ValueToSet> m_tasks;

    CUL::LOG::ILogger* m_logger = nullptr;

    bool m_initialized = false;

private:
    Program() = delete;
    Program( const Program& arg ) = delete;
    Program( Program&& arg ) = delete;
    Program& operator=( const Program& rhv ) = delete;
    Program& operator=( Program&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )