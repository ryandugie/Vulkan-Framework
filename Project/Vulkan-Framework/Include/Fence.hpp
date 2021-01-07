/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
#ifndef FENCE_H
#define FENCE_H
#pragma once

#include "Device.hpp"

namespace VK
{
    class Fence
    {
    public:
        void Create(VK::Device& device, VkFenceCreateInfo& info);
        void ShutDown(VK::Device& device);

        VkFence Get() const;
        VkFence* GetPointerTo();

    private:

        VkFence mFence = VK_NULL_HANDLE;
    };
}
#endif
