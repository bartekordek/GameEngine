#if VULKAN_ENABLED

#include "Renderer/Vulkan/DeviceVulkan.hpp"

#include "gameengine/IGameEngine.hpp"
#include "gameengine/Windowing/IWindow.hpp"
#include "sdl2wrapper/IMPORT_SDL_vulkan.hpp"

namespace LOGLW
{
void* DeviceVulkan::getNativeDevice()
{
    return nullptr;
}

namespace GlobalsVK
{
VkInstance Instance;
std::vector<VkPhysicalDevice> PhysicalDevices;
}  // namespace GlobalsVK

DeviceVulkan::DeviceVulkan() : IRenderDevice( false )
{
}

ContextInfo DeviceVulkan::initContextVersion( LOGLW::IWindow* window )
{
    ContextInfo ci;
    uint32_t extensionCount{ 0u };
    const char** extensionNames{ nullptr };
    const VkInstanceCreateInfo instInfo = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,  // sType
        nullptr,                                 // pNext
        0,                                       // flags
        nullptr,                                 // pApplicationInfo
        0,                                       // enabledLayerCount
        nullptr,                                 // ppEnabledLayerNames
        extensionCount,                          // enabledExtensionCount
        extensionNames,                          // ppEnabledExtensionNames
    };

    const VkResult result = vkCreateInstance( &instInfo, nullptr, &GlobalsVK::Instance );
    CUL::Assert::simple( result == VK_SUCCESS, "Failed to create Vulkan instance" );

    uint32_t physicalDeviceCount;
    vkEnumeratePhysicalDevices( GlobalsVK::Instance, &physicalDeviceCount, nullptr );
    GlobalsVK::PhysicalDevices.resize( physicalDeviceCount );
    vkEnumeratePhysicalDevices(
        GlobalsVK::Instance, &physicalDeviceCount, GlobalsVK::PhysicalDevices.data() );
    VkPhysicalDevice physicalDevice = GlobalsVK::PhysicalDevices[0];

    uint32_t queueFamilyCount{ 0u };
    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice, &queueFamilyCount, nullptr );
    std::vector<VkQueueFamilyProperties> queueFamilies( queueFamilyCount );
    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice, &queueFamilyCount, queueFamilies.data() );

    IWindow* mainWindow = IGameEngine::getInstance()->getMainWindow();

    VkSurfaceKHR surface;
    SDL_Vulkan_CreateSurface( mainWindow->getSDLWindow(), GlobalsVK::Instance, &surface );

    uint32_t graphicsQueueIndex = UINT32_MAX;
    uint32_t presentQueueIndex = UINT32_MAX;
    VkBool32 support;
    uint32_t i = 0;
    for( VkQueueFamilyProperties queueFamily : queueFamilies )
    {
        if( graphicsQueueIndex == UINT32_MAX && queueFamily.queueCount > 0 &&
            queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT )
            graphicsQueueIndex = i;
        if( presentQueueIndex == UINT32_MAX )
        {
            vkGetPhysicalDeviceSurfaceSupportKHR( physicalDevice, i, surface, &support );
            if( support )
                presentQueueIndex = i;
        }
        ++i;
    }

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueInfo = {
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,  // sType
        nullptr,                                     // pNext
        0,                                           // flags
        graphicsQueueIndex,                          // graphicsQueueIndex
        1,                                           // queueCount
        &queuePriority,                              // pQueuePriorities
    };

    VkPhysicalDeviceFeatures deviceFeatures = {};
    const char* deviceExtensionNames[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    VkDeviceCreateInfo createInfo = {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,  // sType
        nullptr,                               // pNext
        0,                                     // flags
        1,                                     // queueCreateInfoCount
        &queueInfo,                            // pQueueCreateInfos
        0,                                     // enabledLayerCount
        nullptr,                               // ppEnabledLayerNames
        1,                                     // enabledExtensionCount
        deviceExtensionNames,                  // ppEnabledExtensionNames
        &deviceFeatures,                       // pEnabledFeatures
    };
    VkDevice device;
    vkCreateDevice( physicalDevice, &createInfo, nullptr, &device );

    VkQueue graphicsQueue;
    vkGetDeviceQueue( device, graphicsQueueIndex, 0, &graphicsQueue );

    VkQueue presentQueue;
    vkGetDeviceQueue( device, presentQueueIndex, 0, &presentQueue );

    while( true )
    {
    }

    return ci;
}

bool DeviceVulkan::isLegacy()
{
    return false;
}

size_t DeviceVulkan::getFrameBufferCount() const
{
    CUL::Assert::simple( false, "Not implemented." );

    return 0u;
}

void DeviceVulkan::initDebugUI()
{
    CUL::Assert::simple( false, "Not implemented." );
}

void DeviceVulkan::finishFrame()
{
    CUL::Assert::simple( false, "Not implemented." );
}

void DeviceVulkan::setUniformValue( int uniformLocation, const glm::vec3& val )
{
    CUL::Assert::simple( false, "Not implemented." );
}

void DeviceVulkan::setUniformValue( int uniformLocation, const glm::vec4& val )
{
    CUL::Assert::simple( false, "Not implemented." );
}

void DeviceVulkan::setUniformValue( int uniformLocation, const glm::mat2& val )
{
    CUL::Assert::simple( false, "Not implemented." );
}

void DeviceVulkan::setUniformValue( int uniformLocation, const glm::mat3& val )
{
    CUL::Assert::simple( false, "Not implemented." );
}

void DeviceVulkan::setUniformValue( int uniformLocation, const glm::mat4& val )
{
    CUL::Assert::simple( false, "Not implemented." );
}

void DeviceVulkan::setProjectionAndModelToIdentity()
{
    CUL::Assert::simple( false, "Not implemented." );
}

void DeviceVulkan::clearColorAndDepthBuffer()
{
    CUL::Assert::simple( false, "Not implemented." );
}

void DeviceVulkan::clearColorTo( const ColorS color )
{
    CUL::Assert::simple( false, "Not implemented." );
}

unsigned int DeviceVulkan::generateVertexArray( const int size )
{
    CUL::Assert::simple( false, "Not implemented." );
    return 0;
}

void DeviceVulkan::setClientState( ClientStateTypes cs, bool enabled )
{
    CUL::Assert::simple( false, "Not implemented." );
}

void DeviceVulkan::texCoordPointer( int coordinatesPerElement,
                                    DataType dataType,
                                    int stride,
                                    void* pointer )
{
    CUL::Assert::simple( false, "Not implemented." );
}

void DeviceVulkan::vertexPointer( int coordinatesPerElement,
                                  DataType dataType,
                                  int stride,
                                  void* pointer )
{
    CUL::Assert::simple( false, "Not implemented." );
}

void DeviceVulkan::setVertexArrayClientState( const bool enable )
{
    CUL::Assert::simple( false, "Not implemented." );
}

void DeviceVulkan::setColorClientState( bool enable )
{
    CUL::Assert::simple( false, "Not implemented." );
}

void DeviceVulkan::deleteBuffer( BufferTypes bufferType, unsigned& id )
{
    CUL::Assert::simple( false, "Not implemented." );
}

void DeviceVulkan::enableVertexAttribiute( unsigned programId, const String& attribName )
{
    CUL::Assert::simple( false, "Not implemented." );
}

void DeviceVulkan::disableVertexAttribiute( unsigned programId, const String& attribName )
{
    CUL::Assert::simple( false, "Not implemented." );
}

int DeviceVulkan::getAttribLocation( unsigned programId, const String& attribName )
{
    CUL::Assert::simple( false, "Not implemented." );
    return 0;
}

int DeviceVulkan::getUniformLocation( unsigned programId, const String& attribName )
{
    CUL::Assert::simple( false, "Not implemented." );
    return 0;
}

void DeviceVulkan::unbindBuffer( const BufferTypes bufferType )
{
    CUL::Assert::simple( false, "Not implemented." );
}

void DeviceVulkan::bindBuffer( const BufferTypes bufferType, unsigned bufferId )
{
    CUL::Assert::simple( false, "Not implemented." );
}

unsigned int DeviceVulkan::generateBuffer( const BufferTypes type, const int size )
{
    CUL::Assert::simple( false, "Not implemented." );
    return 0;
}
void DeviceVulkan::drawElementsFromLastBuffer( const PrimitiveType primitiveType,
                                               const DataType dataType,
                                               unsigned count )
{
}
void DeviceVulkan::drawArrays( unsigned vaoId,
                               const PrimitiveType primitiveType,
                               unsigned first,
                               unsigned count )
{
}
void DeviceVulkan::vertexAttribPointer( const VertexData& meta )
{
}
void DeviceVulkan::enableVertexAttribArray( unsigned attributeId )
{
}
void DeviceVulkan::setVertexPointer( int coordinatesPerVertex,
                                     DataType dataType,
                                     int stride,
                                     const void* data )
{
}
void DeviceVulkan::draw( const QuadCUL& quad, const QuadCUL& texQuad )
{
}
void DeviceVulkan::draw( const QuadCUL& quad, const ColorS& color )
{
}
void DeviceVulkan::draw( const QuadCUL& quad,
                         const Point& translation,
                         const CUL::MATH::Rotation& rotation,
                         const ColorS& color )
{
}
void DeviceVulkan::draw( const QuadCUL& quad, const QuadColors& color )
{
}
void DeviceVulkan::draw( const QuadData& quad, const ColorS& color )
{
}
void DeviceVulkan::draw( const QuadData& quad, const std::array<ColorS, 4>& color )
{
}
void DeviceVulkan::draw( const TriangleCUL& triangle, const ColorS& color )
{
}
void DeviceVulkan::draw( const TriangleCUL& quad, const std::array<ColorS, 4>& color )
{
}
void DeviceVulkan::draw( const TriangleData& values, const std::array<ColorS, 3>& color )
{
}
void DeviceVulkan::draw( const LineData& values, const ColorS& color )
{
}
void DeviceVulkan::draw( const LineData& values, const LineColors& color )
{
}
void DeviceVulkan::draw( const CUL::MATH::Primitives::Line& values, const ColorS& color )
{
}
void DeviceVulkan::draw( const Point& position, const ColorS& color )
{
}
void DeviceVulkan::draw( const QuadCUL& quad,
                         const glm::mat4& model,
                         const ColorS& color )
{
}
void DeviceVulkan::draw( const TriangleCUL& triangle,
                         const glm::mat4& model,
                         const ColorS& color )
{
}
void DeviceVulkan::translate( const Point& point )
{
}
void DeviceVulkan::translate( const float x, const float y, const float z )
{
}
void DeviceVulkan::rotate( const CUL::MATH::Rotation& rotation )
{
}
void DeviceVulkan::rotate( const float angleDeg,
                           const float x,
                           const float y,
                           const float z )
{
}
void DeviceVulkan::scale( const CUL::MATH::Vector3Df& scale )
{
}
void DeviceVulkan::scale( const float scale )
{
}
void DeviceVulkan::setDepthTest( const bool enabled )
{
}
void DeviceVulkan::setBackfaceCUll( const bool enabled )
{
}
void DeviceVulkan::setTexuring( const bool enabled )
{
}
void DeviceVulkan::setActiveTextureUnit( ETextureUnitIndex textureUnitIndex )
{
    CUL::Assert::simple( false, "Not implemented." );
}
void DeviceVulkan::bindTexture( const unsigned int textureId )
{
    CUL::Assert::simple( false, "Not implemented." );
}
void DeviceVulkan::setTextureParameter( uint8_t textureId,
                                        const TextureParameters type,
                                        const TextureFilterType val )
{
    CUL::Assert::simple( false, "Not implemented." );
}

void DeviceVulkan::updateTextureData( const TextureInfo& ti, void* data )
{
    CUL::Assert::simple( false, "Not implemented." );
}

const String& DeviceVulkan::getName() const
{
    CUL::Assert::simple( false, "Not implemented." );
    static String tmp;
    return tmp;
}

LOGLW::RenderTypes::RendererType DeviceVulkan::getType() const
{
    return LOGLW::RenderTypes::RendererType::VULKAN;
}

DeviceVulkan::~DeviceVulkan()
{
}

}  // namespace LOGLW

#endif  // #if VULKAN_ENABLED