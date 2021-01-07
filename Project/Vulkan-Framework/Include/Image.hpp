/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
#ifndef IMAGE_H
#define IMAGE_H
#pragma once

#include "Device.hpp"
#include "Buffer.hpp"
#include "ImageView.hpp"
#include "Sampler.hpp"
#include <map>

namespace VK
{
    class Image
    {
    public:
        void Create(VK::Device& device, unsigned width, unsigned height, VkFormat format, VkImageTiling tiling,
            VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags, VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED);
        void ShutDown(VK::Device& device);

        void TransitionImageLayout(VK::Device& device, VK::CommandPool& commandPool, VkQueue& graphicsQueue, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
        void CopyBufferToImage(VK::Device& device, VK::CommandPool& commandPool, VkQueue& graphicsQueue, VK::Buffer data, uint32_t width, uint32_t height);

        VkImage Get() const;
        VkImage* GetPointerTo();

        VkImageView GetView() const;
        VkImageView* GetPointerToView();

    private:
        uint32_t FindMemoryType(VK::Device& device, uint32_t typeFilter, VkMemoryPropertyFlags properties);

        VK::ImageView mView;

        VkImage mImage = VK_NULL_HANDLE;
        VkDeviceMemory mImageMemory = VK_NULL_HANDLE;
    };
}
#endif
