#pragma once

#include "sdl2wrapper/Input/IKeyboardObserver.hpp"
#include "sdl2wrapper/Input/IMouseObserver.hpp"

#include "CUL/ITimer.hpp"
#include "CUL/Math/Angle.hpp"
#include "CUL/Math/Point.hpp"
#include "CUL/Math/Rotation.hpp"
#include "CUL/IMPORT_GLM.hpp"
#include "CUL/STL_IMPORTS/STD_memory.hpp"

#include <windows.h>
#include <windowsx.h>
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#pragma comment( lib, "d3d12.lib" )

#undef ERROR
#undef min
#undef max

namespace CUL
{
class ITimer;
}

namespace LOGLW
{
class IGameEngine;
class KeyboardState;
class Camera;
class Triangle;
class Sprite;
class Cube;
class Quad;
}  // namespace LOGLW

namespace LOGLW
{
class MouseData;
class IWindow;
namespace WindowEvent
{
enum class Type : short;
}
}  // namespace LOGLW

constexpr UINT g_bbCount = 4;  // define number of backbuffers to use

class Playground final: public LOGLW::IMouseObserver, public LOGLW::IKeyboardObserver
{
public:
    void run();
    void closeApp();

protected:
private:
    void afterInit();
    void onMouseEvent( const LOGLW::MouseData& mouseData );
    void onKeyBoardEvent( const LOGLW::KeyboardState& key );
    void onWindowEvent( const LOGLW::WindowEvent::Type type );
    void timer();

    LOGLW::IGameEngine* m_engine = nullptr;
    LOGLW::IWindow* m_mainWindow = nullptr;
    LOGLW::Camera* m_camera = nullptr;

    LOGLW::Triangle* m_triangleModern = nullptr;
    LOGLW::Triangle* m_triangleLegacy = nullptr;

    CUL::MATH::Rotation m_triangleRotation;

    LOGLW::Quad* m_quadModern = nullptr;
    LOGLW::Quad* m_quadLegacy = nullptr;

    float blueTriangleZ = -1.0f;
    float redTriangleZ = 1.0f;

    LOGLW::Sprite* g_sprite = nullptr;
    LOGLW::Cube* m_cubeModern = nullptr;
    LOGLW::Cube* m_cubeLegacy = nullptr;

    CUL::MATH::Angle m_angle;
    CUL::MATH::Point g_cubePos;
    CUL::MATH::Rotation m_cubeRotation;

    glm::vec3 g_eyePos;

    std::unique_ptr<CUL::ITimer> m_timer;

    float m_time = 0.f;

    Microsoft::WRL::ComPtr<ID3D12Device> m_device;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandListAllocator;  // d3d12 command list allocator
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;               // d3d12 command queue
    Microsoft::WRL::ComPtr<IDXGIDevice2> mDXGIDevice;                       // DXGI device
    Microsoft::WRL::ComPtr<IDXGISwapChain3> mSwapChain;                     // the pointer to the swap chain interface
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;         // d3d12 command list
    Microsoft::WRL::ComPtr<ID3D12Fence> mFence;  // fence used by GPU to signal when command queue execution has finished

    Microsoft::WRL::ComPtr<ID3D12Resource>
        mRenderTarget[g_bbCount];  // backbuffer resource, like d3d11's ID3D11Texture2D, array of 2 for flip_sequential support
};