/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#pragma once

#include "Device.hpp"
#include "SwapChain.hpp"
#include "RenderPass.hpp"

namespace VK
{
    class FrameBuffer
    {
    public:
        void Create(VK::Device& device, VkFramebufferCreateInfo& info);
        void ShutDown(VK::Device& device);

        VkFramebuffer Get() const;
        VkFramebuffer* GetPointerTo();

    private:
        VkFramebuffer mFrameBuffer = VK_NULL_HANDLE;
    };
}
#endif
