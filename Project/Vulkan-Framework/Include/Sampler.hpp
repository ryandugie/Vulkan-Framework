/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
#ifndef SAMPLER_H
#define SAMPLER_H
#pragma once

#include "Device.hpp"

namespace VK
{
    class Sampler
    {
    public:
        void Create(VK::Device& device, VkSamplerCreateInfo& samplerInfo);
        void ShutDown(VK::Device& device);

        VkSampler Get() const;
        VkSampler* GetPointerTo();

    private:

        VkSampler mSampler = VK_NULL_HANDLE;
    };
}
#endif
