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
#include "DescriptorPool.hpp"
#include <array>

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
  create a new Descriptor pool
*/
/****************************************************************************/
void VK::DescriptorPool::Create(VK::Device& device, unsigned bufferCount, VkDeviceSize bufferSize, unsigned textureCount)
{
    VkDescriptorPoolCreateInfo poolInfo = {};
    if (bufferSize > 0)
    {
        std::array<VkDescriptorPoolSize, 2> poolSizes = {};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(bufferCount);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(bufferCount)* (textureCount > 0 ? textureCount : 1);
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(bufferCount);
    }
    else
    {
        std::array<VkDescriptorPoolSize, 1> poolSizes = {};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(bufferCount)* (textureCount > 0 ? textureCount : 1);
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(bufferCount);
    }

    if (vkCreateDescriptorPool(device.Get(), &poolInfo, nullptr, &mPool) != VK_SUCCESS)
    {
        DEBUG::log.Error("DescriptorPool::Create: failed to create descriptor pool!");
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

/****************************************************************************/
/*!
\brief
  cleanup
*/
/****************************************************************************/
void VK::DescriptorPool::ShutDown(VK::Device& device)
{
    vkDestroyDescriptorPool(device.Get(), mPool, nullptr);
}


/****************************************************************************/
/*!
\brief
  get the descirptor pool
*/
/****************************************************************************/
VkDescriptorPool VK::DescriptorPool::Get()
{
    return mPool;
}

/****************************************************************************/
/*!
\brief
  get a pointer to the descirptor pool
*/
/****************************************************************************/
VkDescriptorPool* VK::DescriptorPool::GetPointerTo()
{
    return &mPool;
}