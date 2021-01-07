/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
#ifndef SURFACE_H
#define SURFACE_H
#pragma once

#include "Instance.hpp"

namespace VK
{
    class Surface
    {
    public:
        void Create(VK::Instance& instance, GLFWwindow* window);
        void ShutDown(VK::Instance& instance);

        VkSurfaceKHR Get() const;
        VkSurfaceKHR* GetPointerTo();

    private:
        VkSurfaceKHR mSurface = VK_NULL_HANDLE;
    };
}
#endif
