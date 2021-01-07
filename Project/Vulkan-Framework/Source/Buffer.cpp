/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
/*============================================================================*\
|| ------------------------------ INCLUDES ---------------------------------- ||
\*============================================================================*/

#include "VULKANPCH.hpp"
#include "Buffer.hpp"
#include "CommandBuffer.hpp"

/*============================================================================*\
|| --------------------------- GLOBAL VARIABLES ----------------------------- ||
\*============================================================================*/

/*============================================================================*\
|| -------------------------- STATIC FUNCTIONS ------------------------------ ||
\*============================================================================*/

/*============================================================================*\
|| -------------------------- PUBLIC FUNCTIONS ------------------------------ ||
\*============================================================================*/

/****************************************************************************/
/*!
\brief
  create the buffer
*/
/****************************************************************************/
void VK::Buffer::Create(VK::Device& device, VkBufferCreateInfo& bufferInfo, uint32_t memFlags)
{
    mSize = bufferInfo.size;
    if (vkCreateBuffer(device.Get(), &bufferInfo, nullptr, &mBuffer) != VK_SUCCESS)
    {
        DEBUG::log.Error("Buffer::Create: failed to create VK::Buffer!");
        throw std::runtime_error("failed to create VK::Buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device.Get(), mBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(device, memRequirements.memoryTypeBits, memFlags);

    if (vkAllocateMemory(device.Get(), &allocInfo, nullptr, &mBufferMemory) != VK_SUCCESS)
    {
        DEBUG::log.Error("Buffer::Create: failed to allocate VK::Buffer memory!");
        throw std::runtime_error("failed to allocate VK::Buffer memory!");
    }

    vkBindBufferMemory(device.Get(), mBuffer, mBufferMemory, 0);
    mInfo = bufferInfo;
}

/****************************************************************************/
/*!
\brief
  cleanup
*/
/****************************************************************************/
void VK::Buffer::ShutDown(VK::Device& device)
{
    if (mBuffer == VK_NULL_HANDLE)
        return;

    vkDestroyBuffer(device.Get(), mBuffer, nullptr);
    vkFreeMemory(device.Get(), mBufferMemory, nullptr);

    mBuffer = VK_NULL_HANDLE;
    mBufferMemory = VK_NULL_HANDLE;
}

/****************************************************************************/
/*!
\brief
  Map this memory
*/
/****************************************************************************/
void VK::Buffer::Map(VK::Device& device, size_t size, void** data)
{
    vkMapMemory(device.Get(), mBufferMemory, 0, size, 0, data);
}

/****************************************************************************/
/*!
\brief
  Fill with data
*/
/****************************************************************************/
void VK::Buffer::Fill(VK::Device& device, void* fillData, size_t size)
{
    void* data = nullptr;
    Map(device, size, &data);
    memcpy(data, fillData, size);
    UnMap(device);
}

/****************************************************************************/
/*!
\brief
  UnMap this memory
*/
/****************************************************************************/
void VK::Buffer::UnMap(VK::Device& device)
{
    vkUnmapMemory(device.Get(), mBufferMemory);
}

/****************************************************************************/
/*!
\brief
  copy the data from src into this buffer
*/
/****************************************************************************/
void VK::Buffer::Copy(VK::Device& device, VK::CommandPool& commandPool, VkQueue& graphicsQueue, VK::Buffer src, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = VK::BeginSingleTimeCommands(device, commandPool);

    VkBufferCopy copyRegion = {};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, src.Get(), mBuffer, 1, &copyRegion);

    VK::EndSingleTimeCommands(device, commandPool, graphicsQueue, commandBuffer);
}

/****************************************************************************/
/*!
\brief
  get the buffer
*/
/****************************************************************************/
VkBuffer VK::Buffer::Get() const
{
    return mBuffer;
}

/****************************************************************************/
/*!
\brief
  get pointer to the buffer
*/
/****************************************************************************/
VkBuffer* VK::Buffer::GetPointerTo()
{
    return &mBuffer;
}

/****************************************************************************/
/*!
\brief
  get the data
*/
/****************************************************************************/
VkDeviceMemory VK::Buffer::GetMemory() const
{
    return mBufferMemory;
}

/****************************************************************************/
/*!
\brief
  get pointer to the data
*/
/****************************************************************************/
VkDeviceMemory* VK::Buffer::GetPointerToMemory()
{
    return &mBufferMemory;
}

/*============================================================================*\
|| --------------------------- PRIVATE FUNCTIONS ---------------------------- ||
\*============================================================================*/

/****************************************************************************/
/*!
\brief
  find the type of memeory to store this on the gpu
*/
/****************************************************************************/
uint32_t VK::Buffer::FindMemoryType(VK::Device& device, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device.GetPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    DEBUG::log.Error("Buffer::FindMemoryType: failed to find suitable memory type!");
    throw std::runtime_error("failed to find suitable memory type!");
}