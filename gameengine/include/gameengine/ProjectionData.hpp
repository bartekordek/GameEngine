#pragma once

#include "gameengine/Import.hpp"

#include "SDL2Wrapper/IWindow.hpp"

#include "CUL/GenericUtils/IOnChange.hpp"
#include "CUL/GenericUtils/ValueChangeHook.hpp"

#include "CUL/Graphics/Pos3D.hpp"
#include "CUL/Graphics/Pos2D.hpp"
#include "CUL/Graphics/Size2D.hpp"

NAMESPACE_BEGIN( LOGLW )

using Pos3Di = CUL::Graphics::Pos3Di;
using Pos3Df = CUL::Graphics::Pos3Df;
using Pos2Di = CUL::Graphics::Pos2Di;
using WindowSize = SDL2W::WindowSize;

class GAME_ENGINE_API ProjectionData final:
    public CUL::GUTILS::IOnChange
{
public:
    ProjectionData();
    ProjectionData( const ProjectionData& val );
    ProjectionData( ProjectionData&& val );

    ProjectionData& operator=( const ProjectionData& rhv );
    ProjectionData& operator=( ProjectionData&& rhv );

    void setSize( const WindowSize& winSize );
    void setCenter( const Pos3Df& pos );
    void setEyePos( const Pos3Df& pos );
    void setUp( const Pos3Df& pos );
    void setZnear( const float val );
    void setZfar( const float val );
    void setFov( const float val );

    const WindowSize& getSize() const;
    float getLeft() const;
    float getRight() const;
    float getTop() const;
    float getBottom() const;
    float getZnear() const;
    float getZfar() const;
    float getFov() const;
    float getAspectRatio() const;

    const Pos3Df& getCenter() const;
    const Pos3Df& getEye() const;
    const Pos3Df& getUp() const;

    ~ProjectionData();

    CUL::GUTILS::ValueChangeHook<bool> m_depthTest = true;
    Pos3Df m_center = { 0.0, 0.0, 32.0 };
    Pos3Df m_eye;
    Pos3Df m_up = { 0.0, 1.0, 0.0 };
    WindowSize m_size;
    float m_fov = 90.0f;
    float m_zNear = 64.0f;
    float m_zFar = -64.0f;
    float m_left = 0.0f;
    float m_right = 0.0f;
    float m_top = 0.0f;
    float m_bottom = 0.0f;


protected:
private:

};

NAMESPACE_END( LOGLW )