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
#include "RenderPass.hpp"

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
  Create a new render pass
*/
/****************************************************************************/
void VK::RenderPass::Create(VK::Device& device, VkRenderPassCreateInfo renderPassInfo)
{
    if (mRenderPass != VK_NULL_HANDLE)
        ShutDown(device);

    if (vkCreateRenderPass(device.Get(), &renderPassInfo, nullptr, &mRenderPass) != VK_SUCCESS)
    {
        DEBUG::log.Error("RenderPass::Create: failed to create render pass!");
        throw std::runtime_error("failed to create render pass!");
    }
}

/****************************************************************************/
/*!
\brief
  cleanup
*/
/****************************************************************************/
void VK::RenderPass::ShutDown(VK::Device& device)
{
    if (mRenderPass == VK_NULL_HANDLE)
        return;
    vkDestroyRenderPass(device.Get(), mRenderPass, nullptr);
    mRenderPass = VK_NULL_HANDLE;
}

/****************************************************************************/
/*!
\brief
  get this render pass
*/
/****************************************************************************/
VkRenderPass VK::RenderPass::Get() const
{
    return mRenderPass;
}

/****************************************************************************/
/*!
\brief
  get a pointer to this render pass
*/
/****************************************************************************/
VkRenderPass* VK::RenderPass::GetPointerTo()
{
    return &mRenderPass;
}