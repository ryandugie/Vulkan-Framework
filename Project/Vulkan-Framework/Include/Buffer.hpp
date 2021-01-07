/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
#ifndef BUFFER_H
#define BUFFER_H
#pragma once

#include "Device.hpp"
#include "CommandPool.hpp"

namespace VK
{
    class Buffer
    {
    public:
        void Create(VK::Device& device, VkBufferCreateInfo& bufferInfo, uint32_t memFlags);
        void ShutDown(VK::Device& device);
        void Map(VK::Device& device, size_t, void** data);
        void Fill(VK::Device& device, void* fillData, size_t size);
        void UnMap(VK::Device& device);
        void Copy(VK::Device& device, VK::CommandPool& commandPool, VkQueue& graphicsQueue, VK::Buffer src, VkDeviceSize size);

        VkBuffer Get() const;
        VkBuffer* GetPointerTo();

        VkDeviceMemory GetMemory() const;
        VkDeviceMemory* GetPointerToMemory();

        VkDeviceSize Size() const { return mSize; };

    private:
        uint32_t FindMemoryType(VK::Device& device, uint32_t typeFilter, VkMemoryPropertyFlags properties);

        VkDeviceSize mSize = 0;
        VkBuffer mBuffer = VK_NULL_HANDLE;
        VkDeviceMemory mBufferMemory = VK_NULL_HANDLE;
        VkBufferCreateInfo mInfo = {};
    };
}
#endif
