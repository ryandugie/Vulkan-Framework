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
#include "Image.hpp"
#include "CommandBuffer.hpp"

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
  create an image
*/
/****************************************************************************/
void VK::Image::Create(VK::Device& device, unsigned width, unsigned height, VkFormat format, VkImageTiling tiling,
    VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags, VkImageLayout layout)
{
    UNUSED(layout);

    // make image
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = static_cast<uint32_t>(width);
    imageInfo.extent.height = static_cast<uint32_t>(height);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;

    imageInfo.format = format;//VK_FORMAT_R8G8B8A8_UNORM;
    imageInfo.tiling = tiling;//VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;//layout;
    imageInfo.usage = usage;//VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0; // Optional

    if (vkCreateImage(device.Get(), &imageInfo, nullptr, &mImage) != VK_SUCCESS)
    {
        DEBUG::log.Error("Image::Create: ", "failed to create image!");
        throw std::runtime_error("failed to create image!");
    }

    // allocate image memory
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device.Get(), mImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(device, memRequirements.memoryTypeBits, properties);// VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT

    if (vkAllocateMemory(device.Get(), &allocInfo, nullptr, &mImageMemory) != VK_SUCCESS)
    {
        DEBUG::log.Error("Image::Create: ", "failed to allocate image memory!");
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(device.Get(), mImage, mImageMemory, 0);

    /// create image view
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = mImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    mView.Create(device, viewInfo);
    ///
}

/****************************************************************************/
/*!
\brief
  cleanup
*/
/****************************************************************************/
void VK::Image::ShutDown(VK::Device& device)
{
    vkDestroyImage(device.Get(), mImage, nullptr);
    vkFreeMemory(device.Get(), mImageMemory, nullptr);
    mView.ShutDown(device);
}

/****************************************************************************/
/*!
\brief
  transition this image to a new layout
*/
/****************************************************************************/
void VK::Image::TransitionImageLayout(VK::Device& device, VK::CommandPool& commandPool, VkQueue& graphicsQueue, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    UNUSED(format);

    VkCommandBuffer commandBuffer = VK::BeginSingleTimeCommands(device, commandPool);

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = mImage;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL || newLayout == VK_IMAGE_LAYOUT_GENERAL))
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        DEBUG::log.Error("Image::TransitionImageLayout: ", "unsupported layout transition!");
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    VK::EndSingleTimeCommands(device, commandPool, graphicsQueue, commandBuffer);
}

/****************************************************************************/
/*!
\brief
  copy staging buffer data to the image
*/
/****************************************************************************/
void VK::Image::CopyBufferToImage(VK::Device& device, VK::CommandPool& commandPool, VkQueue& graphicsQueue, VK::Buffer data, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = VK::BeginSingleTimeCommands(device, commandPool);

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(commandBuffer, data.Get(), mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    VK::EndSingleTimeCommands(device, commandPool, graphicsQueue, commandBuffer);
}

/****************************************************************************/
/*!
\brief
  get the image
*/
/****************************************************************************/
VkImage VK::Image::Get() const
{
    return mImage;
}

/****************************************************************************/
/*!
\brief
  get a pointer to the image
*/
/****************************************************************************/
VkImage* VK::Image::GetPointerTo()
{
    return &mImage;
}

/****************************************************************************/
/*!
\brief
  get the image view
*/
/****************************************************************************/
VkImageView VK::Image::GetView() const
{
    return mView.Get();
}

/****************************************************************************/
/*!
\brief
  get a pointer to the image view
*/
/****************************************************************************/
VkImageView* VK::Image::GetPointerToView()
{
    return mView.GetPointerTo();
}

/*============================================================================*\
|| --------------------------- PRIVATE FUNCTIONS ---------------------------- ||
\*============================================================================*/

/****************************************************************************/
/*!
\brief
  find the type of memeory to store this on the gpu
*/
/****************************************************************************/
uint32_t VK::Image::FindMemoryType(VK::Device& device, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device.GetPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    DEBUG::log.Error("Image::FindMemoryType: ", "failed to find suitable memory type!");
    throw std::runtime_error("failed to find suitable memory type!");
}