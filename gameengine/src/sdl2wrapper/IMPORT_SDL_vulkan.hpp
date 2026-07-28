#pragma once

#if VULKAN_ENABLED
    #include "CUL/GenericUtils/DisableErros.hpp"
CUL_MSVC_SKIP_DEFAULT_WARNING_BEGIN

    #include <SDL_vulkan.h>
    #include <vulkan/vulkan.h>

    #ifdef _MSC_VER
        #pragma warning( pop )
    #endif
#endif  // VULKAN_ENABLED
