#pragma once

#include "gameengine/IRenderable.hpp"

#include "CUL/Filesystem/IFile.hpp"
#include "CUL/IName.hpp"
#include "CUL/IRegisteredObject.hpp"

#include "CUL/STL_IMPORTS/STD_map.hpp"
#include "CUL/STL_IMPORTS/STD_vector.hpp"
#include "CUL/STL_IMPORTS/STD_set.hpp"
#include "CUL/STL_IMPORTS/STD_mutex.hpp"

NAMESPACE_BEGIN( LOGLW )

class IComponent;
class Name;
class ShaderProgram;
class TransformComponent;
class VertexArray;

class IObject:
    public IRenderable,
    public CUL::IName
{
public:
    GAME_ENGINE_API IObject( const CUL::String& name, IGameEngine* engine, bool forceLegacy );

    GAME_ENGINE_API virtual const std::vector<float> getVertices() const;

    GAME_ENGINE_API IObject* getOuter();
    GAME_ENGINE_API IObject* getParent();
    GAME_ENGINE_API void setParent( IObject* parent );

    GAME_ENGINE_API const std::set<IObject*>& getChildren() const;
    GAME_ENGINE_API void addChild( IObject* child );

    GAME_ENGINE_API IComponent* getComponent( const CUL::String& name );
    GAME_ENGINE_API void addComponent( const CUL::String& name, IComponent* component );

    GAME_ENGINE_API TransformComponent* getTransform();

    GAME_ENGINE_API void removeByParent( bool enable );
    GAME_ENGINE_API void createProgram();
    GAME_ENGINE_API ShaderProgram* getProgram();

    GAME_ENGINE_API VertexArray* getVao();
    GAME_ENGINE_API void setVao( VertexArray* inVao );

    GAME_ENGINE_API void deleteVao();
    GAME_ENGINE_API void createVao();

    GAME_ENGINE_API virtual ~IObject();

protected:
    GAME_ENGINE_API void onNameChange( const CUL::String& newName ) override;
    IGameEngine& getEngine();

    bool getForceLegacy() const;

private:
    void addParent( IObject* parent );
    void removeChild( IObject* child, bool lock );
    void createVaoImpl();
    ShaderProgram* m_shaderProgram = nullptr;
    VertexArray* m_vao = nullptr;
    bool m_removeByParent = false;
    IGameEngine& m_engine;
    bool m_forceLegacy = false;

    IObject* m_parent = nullptr;

    std::map<CUL::String, IComponent*> m_components;

    TransformComponent* m_transform = nullptr;

    std::mutex m_childrenMtx;
    std::set<IObject*> m_children;



    // Deleted:
    IObject( const IObject& value ) = delete;
    IObject( IObject&& value ) = delete;
    IObject& operator=( const IObject& value ) = delete;
    IObject& operator=( IObject&& value ) = delete;
};


NAMESPACE_END( LOGLW )