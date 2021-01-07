/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
#ifndef DESCRIPTORPOOL_H
#define DESCRIPTORPOOL_H
#pragma once

#include "Device.hpp"
#include "Buffer.hpp"

namespace VK
{
    class DescriptorPool
    {
    public:
        void Create(VK::Device& device, unsigned bufferCount, VkDeviceSize bufferSize, unsigned textureCount = 0);
        void ShutDown(VK::Device& device);

        VkDescriptorPool Get();
        VkDescriptorPool* GetPointerTo();

    private:
        VkDescriptorPool mPool = VK_NULL_HANDLE;
    };
}
#endif
