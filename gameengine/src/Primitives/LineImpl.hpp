#pragma once

#include "gameengine/IUtilityUser.hpp"
#include "gameengine/Primitives/ILine.hpp"

NAMESPACE_BEGIN( LOGLW )

class TransformComponent;

class LineImpl final: public ILine, public IUtilityUser
{
public:
    LineData m_data;

    LineImpl( const LineImpl& ) = delete;
    LineImpl( LineImpl&& ) = delete;
    LineImpl& operator=( const LineImpl& ) = delete;
    LineImpl& operator=( LineImpl&& ) = delete;
    LineImpl();

    void setColor( const LineColors& colors ) override;
    void setColor( const ColorS& color ) override;

    ~LineImpl();

protected:
private:
    void setValues( const LineData& values ) override;
    void render() override;

    TransformComponent* m_transform = nullptr;
    LineColors m_colors;
};

NAMESPACE_END( LOGLW )