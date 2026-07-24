#include <iostream>
#include <vector>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.hpp>
#include <optional>
#include <set>
#include <fstream>

#if defined( WIN32 )
    #pragma comment( linker, "/subsystem:windows" )
    #define VK_USE_PLATFORM_WIN32_KHR
    #define PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#endif  // #if defined(WIN32)

#define APP_NAME "hello-triangle"
#define ARRAY_SIZE( a ) ( sizeof( a ) / sizeof( a[0] ) )
#ifndef UINT32_MAX
    #define UINT32_MAX 0xffffffff
#endif  // #ifndef UINT32_MAX

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance vkInst,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger )
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        vkInst, "vkCreateDebugUtilsMessengerEXT" );
    if( func != nullptr )
    {
        return func( vkInst, pCreateInfo, pAllocator, pDebugMessenger );
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT( VkInstance vkInst,
                                    VkDebugUtilsMessengerEXT debugMessenger,
                                    const VkAllocationCallbacks* pAllocator )
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        vkInst, "vkDestroyDebugUtilsMessengerEXT" );
    if( func != nullptr )
    {
        func( vkInst, debugMessenger, pAllocator );
    }
}

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class HelloTriangleApplication
{
public:
    void run()
    {
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    void initVulkan()
    {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createRenderPass();
        createGraphicsPipeline();
        createFramebuffers();
        createCommandPool();
        createCommandBuffers();
        createSyncObjects();
    }

    void createInstance()
    {
        if( enableValidationLayers && !checkValidationLayerSupport() )
        {
            throw std::runtime_error( "validation layers requested, but not available!" );
        }

        SDL_Init( SDL_INIT_VIDEO );
        SDL_Vulkan_LoadLibrary( nullptr );
        m_window = SDL_CreateWindow( APP_NAME,
                                     SDL_WINDOWPOS_UNDEFINED,
                                     SDL_WINDOWPOS_UNDEFINED,
                                     640,
                                     360,
                                     SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN );

        uint32_t extensionCount;
        const char** extensionNames = 0;
        SDL_Vulkan_GetInstanceExtensions( m_window, &extensionCount, nullptr );
        extensionNames = new const char*[extensionCount];
        SDL_Vulkan_GetInstanceExtensions( m_window, &extensionCount, extensionNames );
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
        vkCreateInstance( &instInfo, nullptr, &m_vkInst );
        SDL_Log( "Initialized with errors: %s", SDL_GetError() );
    }

    void setupDebugMessenger()
    {
        if( isInstanceExtensionSupported( VK_EXT_DEBUG_UTILS_EXTENSION_NAME ) )
        {
            std::cout << "VK_EXT_debug_utils is supported.\n";
        }
        else
        {
            std::cout << "VK_EXT_debug_utils is NOT supported.\n";
        }

        if( !enableValidationLayers )
            return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo( createInfo );

        if( CreateDebugUtilsMessengerEXT(
                m_vkInst, &createInfo, nullptr, &m_debugMessenger ) != VK_SUCCESS )
        {
            throw std::runtime_error( "failed to set up debug messenger!" );
        }
    }

    void populateDebugMessengerCreateInfo(
        VkDebugUtilsMessengerCreateInfoEXT& createInfo )
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    void createSurface()
    {
        if( SDL_Vulkan_CreateSurface( m_window, m_vkInst, &m_screenSurface ) !=
            VK_SUCCESS )
        {
            throw std::runtime_error( "failed to create window surface!" );
        }
    }

    void pickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices( m_vkInst, &deviceCount, nullptr );

        if( deviceCount == 0 )
        {
            throw std::runtime_error( "failed to find GPUs with Vulkan support!" );
        }

        m_physicalDevices.resize( deviceCount );
        vkEnumeratePhysicalDevices( m_vkInst, &deviceCount, m_physicalDevices.data() );

        for( const auto& device : m_physicalDevices )
        {
            if( isDeviceSuitable( device ) )
            {
                m_mainPhysicalDevice = device;
                break;
            }
        }

        if( m_mainPhysicalDevice == VK_NULL_HANDLE )
        {
            throw std::runtime_error( "failed to find a suitable GPU!" );
        }
    }

    void createLogicalDevice()
    {
        QueueFamilyIndices indices = findQueueFamilies( m_mainPhysicalDevice );

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(),
                                                   indices.presentFamily.value() };

        float queuePriority = 1.0f;
        for( uint32_t queueFamily : uniqueQueueFamilies )
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back( queueCreateInfo );
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount =
            static_cast<uint32_t>( queueCreateInfos.size() );
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount =
            static_cast<uint32_t>( deviceExtensions.size() );
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if( enableValidationLayers )
        {
            createInfo.enabledLayerCount =
                static_cast<uint32_t>( validationLayers.size() );
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        if( vkCreateDevice( m_mainPhysicalDevice, &createInfo, nullptr, &m_device ) !=
            VK_SUCCESS )
        {
            throw std::runtime_error( "failed to create logical device!" );
        }

        vkGetDeviceQueue( m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue );
        vkGetDeviceQueue( m_device, indices.presentFamily.value(), 0, &m_presentQueue );
    }

    void createSwapChain()
    {
        SwapChainSupportDetails swapChainSupport =
            querySwapChainSupport( m_mainPhysicalDevice );

        VkSurfaceFormatKHR surfaceFormat =
            chooseSwapSurfaceFormat( swapChainSupport.formats );
        VkPresentModeKHR presentMode =
            chooseSwapPresentMode( swapChainSupport.presentModes );
        VkExtent2D extent = chooseSwapExtent( swapChainSupport.capabilities );

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if( swapChainSupport.capabilities.maxImageCount > 0 &&
            imageCount > swapChainSupport.capabilities.maxImageCount )
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = findQueueFamilies( m_mainPhysicalDevice );
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(),
                                          indices.presentFamily.value() };

        if( indices.graphicsFamily != indices.presentFamily )
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        if( vkCreateSwapchainKHR( m_device, &createInfo, nullptr, &m_swapChain ) !=
            VK_SUCCESS )
        {
            throw std::runtime_error( "failed to create swap chain!" );
        }

        vkGetSwapchainImagesKHR( m_device, m_swapChain, &imageCount, nullptr );
        m_swapChainImages.resize( imageCount );
        vkGetSwapchainImagesKHR(
            m_device, m_swapChain, &imageCount, m_swapChainImages.data() );

        m_swapChainImageFormat = surfaceFormat.format;
        m_swapChainExtent = extent;
    }

    void createImageViews()
    {
        m_swapChainImageViews.resize( m_swapChainImages.size() );

        for( size_t i = 0; i < m_swapChainImages.size(); i++ )
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_swapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if( vkCreateImageView(
                    m_device, &createInfo, nullptr, &m_swapChainImageViews[i] ) !=
                VK_SUCCESS )
            {
                throw std::runtime_error( "failed to create image views!" );
            }
        }
    }

    void createRenderPass()
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = m_swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if( vkCreateRenderPass( m_device, &renderPassInfo, nullptr, &m_renderPass ) !=
            VK_SUCCESS )
        {
            throw std::runtime_error( "failed to create render pass!" );
        }
    }

    void createGraphicsPipeline()
    {
        auto vertShaderCode = readFile( "shaders/vert.spv" );
        auto fragShaderCode = readFile( "shaders/frag.spv" );

        VkShaderModule vertShaderModule = createShaderModule( vertShaderCode );
        VkShaderModule fragShaderModule = createShaderModule( fragShaderCode );

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo,
                                                           fragShaderStageInfo };

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT,
                                                      VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>( dynamicStates.size() );
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        if( vkCreatePipelineLayout(
                m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout ) !=
            VK_SUCCESS )
        {
            throw std::runtime_error( "failed to create pipeline layout!" );
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = m_pipelineLayout;
        pipelineInfo.renderPass = m_renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if( vkCreateGraphicsPipelines( m_device,
                                       VK_NULL_HANDLE,
                                       1,
                                       &pipelineInfo,
                                       nullptr,
                                       &m_graphicsPipeline ) != VK_SUCCESS )
        {
            throw std::runtime_error( "failed to create graphics pipeline!" );
        }

        vkDestroyShaderModule( m_device, fragShaderModule, nullptr );
        vkDestroyShaderModule( m_device, vertShaderModule, nullptr );
    }

    void createFramebuffers()
    {
        m_swapChainFramebuffers.resize( m_swapChainImageViews.size() );

        for( size_t i = 0; i < m_swapChainImageViews.size(); i++ )
        {
            VkImageView attachments[] = { m_swapChainImageViews[i] };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = m_swapChainExtent.width;
            framebufferInfo.height = m_swapChainExtent.height;
            framebufferInfo.layers = 1;

            if( vkCreateFramebuffer(
                    m_device, &framebufferInfo, nullptr, &m_swapChainFramebuffers[i] ) !=
                VK_SUCCESS )
            {
                throw std::runtime_error( "failed to create framebuffer!" );
            }
        }
    }

    void createCommandPool()
    {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies( m_mainPhysicalDevice );

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if( vkCreateCommandPool( m_device, &poolInfo, nullptr, &m_commandPool ) !=
            VK_SUCCESS )
        {
            throw std::runtime_error( "failed to create command pool!" );
        }
    }

    void createCommandBuffers()
    {
        m_commandBuffers.resize( MAX_FRAMES_IN_FLIGHT );

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();

        if( vkAllocateCommandBuffers( m_device, &allocInfo, m_commandBuffers.data() ) !=
            VK_SUCCESS )
        {
            throw std::runtime_error( "failed to allocate command buffers!" );
        }
    }

    void recordCommandBuffer( VkCommandBuffer commandBuffer, uint32_t imageIndex )
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if( vkBeginCommandBuffer( commandBuffer, &beginInfo ) != VK_SUCCESS )
        {
            throw std::runtime_error( "failed to begin recording command buffer!" );
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderPass;
        renderPassInfo.framebuffer = m_swapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_swapChainExtent;

        VkClearValue clearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(
            commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );

        vkCmdBindPipeline(
            commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline );

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)m_swapChainExtent.width;
        viewport.height = (float)m_swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport( commandBuffer, 0, 1, &viewport );

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = m_swapChainExtent;
        vkCmdSetScissor( commandBuffer, 0, 1, &scissor );

        vkCmdDraw( commandBuffer, 3, 1, 0, 0 );

        vkCmdEndRenderPass( commandBuffer );

        if( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS )
        {
            throw std::runtime_error( "failed to record command buffer!" );
        }
    }

    void createSyncObjects()
    {
        m_imageAvailableSemaphores.resize( MAX_FRAMES_IN_FLIGHT );
        m_renderFinishedSemaphores.resize( MAX_FRAMES_IN_FLIGHT );
        m_inFlightFences.resize( MAX_FRAMES_IN_FLIGHT );

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
        {
            if( vkCreateSemaphore(
                    m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i] ) !=
                    VK_SUCCESS ||
                vkCreateSemaphore(
                    m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i] ) !=
                    VK_SUCCESS ||
                vkCreateFence( m_device, &fenceInfo, nullptr, &m_inFlightFences[i] ) !=
                    VK_SUCCESS )
            {
                throw std::runtime_error(
                    "failed to create synchronization objects for a frame!" );
            }
        }
    }

    VkShaderModule createShaderModule( const std::vector<char>& code )
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>( code.data() );

        VkShaderModule shaderModule;
        if( vkCreateShaderModule( m_device, &createInfo, nullptr, &shaderModule ) !=
            VK_SUCCESS )
        {
            throw std::runtime_error( "failed to create shader module!" );
        }

        return shaderModule;
    }

    void mainLoop()
    {
        bool running = true;
        while( running )
        {
            SDL_Event windowEvent;
            while( SDL_PollEvent( &windowEvent ) )
            {
                if( windowEvent.type == SDL_QUIT )
                {
                    running = false;
                    break;
                }
            }
            drawFrame();
        }
    }

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& availableFormats )
    {
        for( const auto& availableFormat : availableFormats )
        {
            if( availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR>& availablePresentModes )
    {
        for( const auto& availablePresentMode : availablePresentModes )
        {
            if( availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR )
            {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D chooseSwapExtent( const VkSurfaceCapabilitiesKHR& capabilities )
    {
        if( capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() )
        {
            return capabilities.currentExtent;
        }

        int width = 0;
        int height = 0;
        SDL_Vulkan_GetDrawableSize( m_window, &width, &height );

        VkExtent2D actualExtent = { static_cast<uint32_t>( width ),
                                    static_cast<uint32_t>( height ) };

        actualExtent.width = std::clamp( actualExtent.width,
                                         capabilities.minImageExtent.width,
                                         capabilities.maxImageExtent.width );

        actualExtent.height = std::clamp( actualExtent.height,
                                          capabilities.minImageExtent.height,
                                          capabilities.maxImageExtent.height );

        return actualExtent;
    }

    SwapChainSupportDetails querySwapChainSupport( VkPhysicalDevice device )
    {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            device, m_surface, &details.capabilities );

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR( device, m_surface, &formatCount, nullptr );

        if( formatCount != 0 )
        {
            details.formats.resize( formatCount );
            vkGetPhysicalDeviceSurfaceFormatsKHR(
                device, m_surface, &formatCount, details.formats.data() );
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device, m_surface, &presentModeCount, nullptr );

        if( presentModeCount != 0 )
        {
            details.presentModes.resize( presentModeCount );
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                device, m_surface, &presentModeCount, details.presentModes.data() );
        }

        return details;
    }

    bool isDeviceSuitable( VkPhysicalDevice device )
    {
        QueueFamilyIndices indices = findQueueFamilies( device );

        bool extensionsSupported = checkDeviceExtensionSupport( device );

        bool swapChainAdequate = false;
        if( extensionsSupported )
        {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport( device );
            swapChainAdequate = !swapChainSupport.formats.empty() &&
                                !swapChainSupport.presentModes.empty();
        }

        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }

    bool checkDeviceExtensionSupport( VkPhysicalDevice device )
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, nullptr );

        std::vector<VkExtensionProperties> availableExtensions( extensionCount );
        vkEnumerateDeviceExtensionProperties(
            device, nullptr, &extensionCount, availableExtensions.data() );

        std::set<std::string> requiredExtensions( deviceExtensions.begin(),
                                                  deviceExtensions.end() );

        for( const auto& extension : availableExtensions )
        {
            requiredExtensions.erase( extension.extensionName );
        }


        return requiredExtensions.empty();
    }

    bool isInstanceExtensionSupported( const char* extensionName )
    {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, nullptr );

        std::vector<VkExtensionProperties> extensions( extensionCount );
        vkEnumerateInstanceExtensionProperties(
            nullptr, &extensionCount, extensions.data() );

        for( const auto& extension : extensions )
        {
            if( strcmp( extension.extensionName, extensionName ) == 0 )
            {
                return true;
            }
        }

        return false;
    }

    QueueFamilyIndices findQueueFamilies( VkPhysicalDevice device )
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, nullptr );

        std::vector<VkQueueFamilyProperties> queueFamilies( queueFamilyCount );
        vkGetPhysicalDeviceQueueFamilyProperties(
            device, &queueFamilyCount, queueFamilies.data() );

        int i = 0;
        for( const auto& queueFamily : queueFamilies )
        {
            if( queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT )
            {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR( device, i, m_surface, &presentSupport );

            if( presentSupport )
            {
                indices.presentFamily = i;
            }

            if( indices.isComplete() )
            {
                break;
            }

            i++;
        }

        return indices;
    }

    std::vector<const char*> getRequiredExtensions()
    {
        uint32_t extensionCount = 0;
        SDL_Vulkan_GetInstanceExtensions( m_window, &extensionCount, nullptr );

        std::vector<const char*> extensions( extensionCount );

        SDL_Vulkan_GetInstanceExtensions( m_window, &extensionCount, extensions.data() );

        if( enableValidationLayers )
        {
            extensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
        }

        return extensions;
    }

    bool checkValidationLayerSupport()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

        std::vector<VkLayerProperties> availableLayers( layerCount );
        vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data() );

        for( const char* layerName : validationLayers )
        {
            bool layerFound = false;

            for( const auto& layerProperties : availableLayers )
            {
                if( strcmp( layerName, layerProperties.layerName ) == 0 )
                {
                    layerFound = true;
                    break;
                }
            }

            if( !layerFound )
            {
                return false;
            }
        }

        return true;
    }

    static std::vector<char> readFile( const std::string& filename )
    {
        std::ifstream file( filename, std::ios::ate | std::ios::binary );

        if( !file.is_open() )
        {
            throw std::runtime_error( "failed to open file!" );
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer( fileSize );

        file.seekg( 0 );
        file.read( buffer.data(), fileSize );

        file.close();

        return buffer;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                   VkDebugUtilsMessageTypeFlagsEXT messageType,
                   const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                   void* pUserData )
    {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    void drawFrame()
    {
        vkWaitForFences(
            m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX );

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR( m_device,
                                                 m_swapChain,
                                                 UINT64_MAX,
                                                 m_imageAvailableSemaphores[m_currentFrame],
                                                 VK_NULL_HANDLE,
                                                 &imageIndex );

        if( result == VK_ERROR_OUT_OF_DATE_KHR )
        {
            recreateSwapChain();
            return;
        }
        else if( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR )
        {
            throw std::runtime_error( "failed to acquire swap chain image!" );
        }

        vkResetFences( m_device, 1, &m_inFlightFences[m_currentFrame] );

        vkResetCommandBuffer( m_commandBuffers[m_currentFrame],
                              /*VkCommandBufferResetFlagBits*/ 0 );
        recordCommandBuffer( m_commandBuffers[m_currentFrame], imageIndex );

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame] };
        VkPipelineStageFlags waitStages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];

        VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if( vkQueueSubmit(
                m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame] ) !=
            VK_SUCCESS )
        {
            throw std::runtime_error( "failed to submit draw command buffer!" );
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { m_swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR( m_presentQueue, &presentInfo );

        if( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
            m_framebufferResized )
        {
            m_framebufferResized = false;
            recreateSwapChain();
        }
        else if( result != VK_SUCCESS )
        {
            throw std::runtime_error( "failed to present swap chain image!" );
        }

        m_currentFrame = ( m_currentFrame + 1 ) % MAX_FRAMES_IN_FLIGHT;
    }

    void recreateSwapChain()
    {
        int width = 0;
        int height = 0;

        SDL_Vulkan_GetDrawableSize( m_window, &width, &height );

        while( width == 0 || height == 0 )
        {
            SDL_WaitEvent( nullptr );
            SDL_Vulkan_GetDrawableSize( m_window, &width, &height );
        }

        vkDeviceWaitIdle( m_device );

        cleanupSwapChain();

        createSwapChain();
        createImageViews();
        createFramebuffers();
    }

    void cleanup()
    {
        vkDestroyDevice( m_device, nullptr );
        vkDestroyInstance( m_vkInst, nullptr );
        SDL_DestroyWindow( m_window );

        SDL_Vulkan_UnloadLibrary();
        SDL_Quit();

        SDL_Log( "Cleaned up with errors: %s", SDL_GetError() );
    }

        void cleanupSwapChain()
    {
        for( auto framebuffer : m_swapChainFramebuffers )
        {
            vkDestroyFramebuffer( m_device, framebuffer, nullptr );
        }

        for( auto imageView : m_swapChainImageViews )
        {
            vkDestroyImageView( m_device, imageView, nullptr );
        }

        vkDestroySwapchainKHR( m_device, m_swapChain, nullptr );
    }

private:
    SDL_Window* m_window{ nullptr };
    VkDevice m_device{ VK_NULL_HANDLE };
    VkInstance m_vkInst{ VK_NULL_HANDLE };
    VkDebugUtilsMessengerEXT m_debugMessenger{ VK_NULL_HANDLE };
    VkSurfaceKHR m_screenSurface{ VK_NULL_HANDLE };
    std::vector<VkPhysicalDevice> m_physicalDevices;
    VkPhysicalDevice m_mainPhysicalDevice{ VK_NULL_HANDLE };
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
    VkSurfaceKHR m_surface;
    VkSwapchainKHR m_swapChain;
    std::vector<VkImage> m_swapChainImages;
    std::vector<VkImageView> m_swapChainImageViews;
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;
    VkRenderPass m_renderPass;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;
    std::vector<VkFramebuffer> m_swapChainFramebuffers;
    VkCommandPool m_commandPool;
    std::vector<VkCommandBuffer> m_commandBuffers;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    uint32_t m_currentFrame{ 0u };
    bool m_framebufferResized{ false };
};

void appMain()
{
    HelloTriangleApplication app;
    app.run();
}

#if defined( WIN32 )
int WinMain( HINSTANCE /*hInst*/,
             HINSTANCE /*hPrevInst*/,
             const char* /*cmd*/,
             int /*showmode*/ )
{
    appMain();
}
#endif  // #if defined(WIN32)

int main( int /*argc*/, char** /*argv*/ )
{
    appMain();
    return 0;
}