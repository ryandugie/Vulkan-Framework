/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H
#pragma once

#include "Device.hpp"

namespace VK
{
    class ImageView
    {
    public:
        void Create(VK::Device& device, VkImageViewCreateInfo& createInfo);
        void ShutDown(VK::Device& device);

        VkImageView Get() const;
        VkImageView* GetPointerTo();

    private:
        VkImageView mImageView = VK_NULL_HANDLE;
    };
}
#endif
