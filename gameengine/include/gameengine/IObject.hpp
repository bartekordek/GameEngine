#pragma once

#include "gameengine/IRenderable.hpp"

#include "CUL/Filesystem/IFile.hpp"

#include "CUL/STL_IMPORTS/STD_map.hpp"
#include "CUL/STL_IMPORTS/STD_vector.hpp"
#include "CUL/STL_IMPORTS/STD_set.hpp"
#include "CUL/STL_IMPORTS/STD_mutex.hpp"

NAMESPACE_BEGIN( LOGLW )

class IComponent;
class TransformComponent;
class Name;

class IObject: public IRenderable
{
public:
    GAME_ENGINE_API IObject( IGameEngine* engine, bool forceLegacy );

    GAME_ENGINE_API virtual const std::vector<float> getVertices() const;

    GAME_ENGINE_API IObject* getParent();
    GAME_ENGINE_API void setParent( IObject* parent );

    GAME_ENGINE_API const std::set<IObject*>& getChildren() const;
    GAME_ENGINE_API void addChild( IObject* child );

    GAME_ENGINE_API IComponent* getComponent( const CUL::String& name );
    GAME_ENGINE_API void addComponent( const CUL::String& name, IComponent* component );

    GAME_ENGINE_API TransformComponent* getTransform();

    GAME_ENGINE_API void setName( const CUL::String& name );
    GAME_ENGINE_API const CUL::String& getName() const;


    GAME_ENGINE_API virtual ~IObject();

protected:
    IGameEngine& getEngine();

    bool getForceLegacy() const;

private:
    void addParent( IObject* parent );
    void removeChild( IObject* child );

    CUL::String m_name;
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