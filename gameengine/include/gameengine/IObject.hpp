#pragma once

#include "gameengine/IRenderable.hpp"
#include "gameengine/ITransformable.hpp"
#include "gameengine/IShaderFactory.hpp"

#include "CUL/Filesystem/IFile.hpp"
#include "CUL/STL_IMPORTS/STD_vector.hpp"
#include "CUL/STL_IMPORTS/STD_set.hpp"

NAMESPACE_BEGIN( LOGLW )


class GAME_ENGINE_API IObject:
    public IRenderable,
    public ITransformable
{
public:
    IObject();

    virtual void addShader( const CUL::FS::Path& filePath, IShaderFactory* sf );

    virtual const std::vector<float> getVertices() const;

    IObject* getParent();
    void setParent(IObject* parent);

    const std::set<IObject*>& getChildren() const;
    void addChild(IObject* child);

    virtual ~IObject();
protected:
private:
    IObject* m_parent = nullptr;
    std::set<IObject*> m_children;

// Deleted:
    IObject( const IObject& value ) = delete;
    IObject( IObject&& value ) = delete;
    IObject& operator=( const IObject& value ) = delete;
    IObject& operator=( IObject&& value ) = delete;
};

NAMESPACE_END( LOGLW )