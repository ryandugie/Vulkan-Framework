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
#include "Semaphore.hpp"
#include <stdexcept>
#include "Log.hpp"

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
  create a new Semaphore
*/
/****************************************************************************/
void VK::Semaphore::Create(VK::Device& device, VkSemaphoreCreateInfo& info)
{
    if (mSemaphore != VK_NULL_HANDLE)
        ShutDown(device);

    if (vkCreateSemaphore(device.Get(), &info, nullptr, &mSemaphore) != VK_SUCCESS)
    {
        DEBUG::log.Error("Semaphore::Create: failed to create semaphore for a frame!");
        throw std::runtime_error("failed to create semaphore for a frame!");
    }
}

/****************************************************************************/
/*!
\brief
  cleanup
*/
/****************************************************************************/
void VK::Semaphore::ShutDown(VK::Device& device)
{
    if (mSemaphore == VK_NULL_HANDLE)
        return;

    vkDestroySemaphore(device.Get(), mSemaphore, nullptr);
    mSemaphore = VK_NULL_HANDLE;
}

/****************************************************************************/
/*!
\brief
  get this semaphore
*/
/****************************************************************************/
VkSemaphore VK::Semaphore::Get() const
{
    return mSemaphore;
}

/****************************************************************************/
/*!
\brief
  get a pointer to this semaphore
*/
/****************************************************************************/
VkSemaphore* VK::Semaphore::GetPointerTo()
{
    return &mSemaphore;
}