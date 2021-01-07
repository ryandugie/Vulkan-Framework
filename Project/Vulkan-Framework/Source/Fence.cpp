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
#include "Fence.hpp"

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
  create a new Fence
*/
/****************************************************************************/
void VK::Fence::Create(VK::Device& device, VkFenceCreateInfo& info)
{
    if (mFence != VK_NULL_HANDLE)
        ShutDown(device);

    if (vkCreateFence(device.Get(), &info, nullptr, &mFence) != VK_SUCCESS)
    {
        DEBUG::log.Error("Fence::Create: failed to create logical device!");
        throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
}

/****************************************************************************/
/*!
\brief
  cleanup
*/
/****************************************************************************/
void VK::Fence::ShutDown(VK::Device& device)
{
    if (mFence == VK_NULL_HANDLE)
        return;

    vkDestroyFence(device.Get(), mFence, nullptr);
    mFence = VK_NULL_HANDLE;
}

/****************************************************************************/
/*!
\brief
  get this fence
*/
/****************************************************************************/
VkFence VK::Fence::Get() const
{
    return mFence;
}

/****************************************************************************/
/*!
\brief
  get a pointer to this fence
*/
/****************************************************************************/
VkFence* VK::Fence::GetPointerTo()
{
    return &mFence;
}