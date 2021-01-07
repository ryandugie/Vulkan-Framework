/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/

#ifndef INSTANCE_H
#define INSTANCE_H
#pragma once

#pragma warning(push, 0)
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>
#pragma warning(pop)

namespace VK 
{

    class Instance
    {
    public:

        Instance() = default;
        ~Instance() = default;

        void Create();
        void ShutDown();
        bool UsingValidationLayers() const;
        VkInstance Get() const;
        VkInstance* GetPointerTo();

    private:
        VkInstance mInstance = VK_NULL_HANDLE;

        bool CheckValidationLayerSupport() const;

#ifdef NDEBUG
        const bool mEnableValidationLayers = false;
        const std::vector<const char*> mValidationLayers = {};
#else
        const bool mEnableValidationLayers = true;
        const std::vector<const char*> mValidationLayers = { "VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_monitor" };
#endif
    };

}


#endif // RENDERER_H