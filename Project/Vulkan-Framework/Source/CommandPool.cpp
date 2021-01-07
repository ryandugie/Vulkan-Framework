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
#include "CommandPool.hpp"

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
  create a new CommandPool
*/
/****************************************************************************/
void VK::CommandPool::Create(VK::Device& device, VK::Surface surface)
{
    if (mCommandPool != VK_NULL_HANDLE)
        ShutDown(device);

    VK::Device::QueueFamilyIndices queueFamilyIndices = device.GetQueueFamilies(surface);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    if (vkCreateCommandPool(device.Get(), &poolInfo, nullptr, &mCommandPool) != VK_SUCCESS)
    {
        DEBUG::log.Error("CommandPool::Create: failed to create command pool!");
        throw std::runtime_error("failed to create command pool!");
    }
}

/****************************************************************************/
/*!
\brief
  cleanup
*/
/****************************************************************************/
void VK::CommandPool::ShutDown(VK::Device& device)
{
    if (mCommandPool == VK_NULL_HANDLE)
        return;

    vkDestroyCommandPool(device.Get(), mCommandPool, nullptr);
    mCommandPool = VK_NULL_HANDLE;
}

/****************************************************************************/
/*!
\brief
  get this commandpool
*/
/****************************************************************************/
VkCommandPool VK::CommandPool::Get() const
{
    return mCommandPool;
}

/****************************************************************************/
/*!
\brief
  get a pointer to this commandpool
*/
/****************************************************************************/
VkCommandPool* VK::CommandPool::GetPointerTo()
{
    return &mCommandPool;
}