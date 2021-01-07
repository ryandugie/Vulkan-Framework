/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#pragma once

#include "Device.hpp"

namespace VK
{
    class Semaphore
    {
    public:
        void Create(VK::Device& device, VkSemaphoreCreateInfo& info);
        void ShutDown(VK::Device& device);

        VkSemaphore Get() const;
        VkSemaphore* GetPointerTo();

    private:
        VkSemaphore mSemaphore = VK_NULL_HANDLE;
    };
}
#endif
