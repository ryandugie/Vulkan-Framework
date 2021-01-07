/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
#ifndef COMMANDPOOL_H
#define COMMANDPOOL_H
#pragma once

#include "Device.hpp"
#include "Surface.hpp"

namespace VK
{
    class CommandPool
    {
    public:
        void Create(VK::Device& device, VK::Surface surface);
        void ShutDown(VK::Device& device);

        VkCommandPool Get() const;
        VkCommandPool* GetPointerTo();

    private:
        VkCommandPool mCommandPool = VK_NULL_HANDLE;
    };
}
#endif
