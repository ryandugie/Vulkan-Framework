/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
#ifndef RENDERPASS_H
#define RENDERPASS_H
#pragma once

#include "Device.hpp"

namespace VK
{
    class RenderPass
    {
    public:
        void Create(VK::Device& device, VkRenderPassCreateInfo renderPassInfo);
        void ShutDown(VK::Device& device);

        VkRenderPass Get() const;
        VkRenderPass* GetPointerTo();

    private:
        VkRenderPass mRenderPass = VK_NULL_HANDLE;
    };
}
#endif
