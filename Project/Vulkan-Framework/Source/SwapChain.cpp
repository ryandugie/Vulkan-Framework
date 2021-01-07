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
#include "SwapChain.hpp"
#include "Surface.hpp"

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
  create a new swap chain
*/
/****************************************************************************/
void VK::SwapChain::Create(VK::Surface& surface, VK::Device& device, GLFWwindow* window)
{
    if (mSwapChain != VK_NULL_HANDLE)
        ShutDown(device);

    // make swap chain
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(surface, device);

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, window);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface.Get();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VK::Device::QueueFamilyIndices indices = device.GetQueueFamilies(surface);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(device.Get(), &createInfo, nullptr, &mSwapChain) != VK_SUCCESS)
    {
        DEBUG::log.Error("SwapChain::Create: failed to create swap chain!");
        throw std::runtime_error("failed to create swap chain!");
    }

    // create swap chain images
    vkGetSwapchainImagesKHR(device.Get(), mSwapChain, &imageCount, nullptr);
    mImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device.Get(), mSwapChain, &imageCount, mImages.data());

    mImageFormat = surfaceFormat.format;
    mExtent = extent;

    mImageViews.resize(mImages.size());

    for (size_t i = 0; i < mImages.size(); ++i)
    {
        VkImageViewCreateInfo createImageViewInfo = {};
        createImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createImageViewInfo.image = mImages[i];
        createImageViewInfo.format = mImageFormat;
        createImageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createImageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createImageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createImageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createImageViewInfo.subresourceRange.baseMipLevel = 0;
        createImageViewInfo.subresourceRange.levelCount = 1;
        createImageViewInfo.subresourceRange.baseArrayLayer = 0;
        createImageViewInfo.subresourceRange.layerCount = 1;

        mImageViews[i].Create(device, createImageViewInfo);
    }
}

/****************************************************************************/
/*!
\brief
  cleanup
*/
/****************************************************************************/
void VK::SwapChain::ShutDown(VK::Device& device)
{
    if (mSwapChain == VK_NULL_HANDLE)
        return;

    for (auto imageView : mImageViews)
    {
        imageView.ShutDown(device);
    }

    vkDestroySwapchainKHR(device.Get(), mSwapChain, nullptr);
    mSwapChain = VK_NULL_HANDLE;
}

/****************************************************************************/
/*!
\brief
  get the swapchain
*/
/****************************************************************************/
VkSwapchainKHR VK::SwapChain::Get() const
{
    return mSwapChain;
}

/****************************************************************************/
/*!
\brief
  get a pointer to the swap chain
*/
/****************************************************************************/
VkSwapchainKHR* VK::SwapChain::GetPointerTo()
{
    return &mSwapChain;
}

/****************************************************************************/
/*!
\brief
  get the image format
*/
/****************************************************************************/
VkFormat VK::SwapChain::Format() const
{
    return mImageFormat;
}

/****************************************************************************/
/*!
\brief
  get the swapchain extent
*/
/****************************************************************************/
VkExtent2D VK::SwapChain::Extent() const
{
    return mExtent;
}

/****************************************************************************/
/*!
\brief
  get the images used in the swap chain
*/
/****************************************************************************/
std::vector<VkImage>* VK::SwapChain::Images()
{
    return &mImages;
}

/****************************************************************************/
/*!
\brief
  get the imageviews used in swapchain
*/
/****************************************************************************/
std::vector<VK::ImageView>* VK::SwapChain::ImageViews()
{
    return &mImageViews;
}

/*============================================================================*\
|| ------------------------- PRIVATE FUNCTIONS ------------------------------ ||
\*============================================================================*/

/****************************************************************************/
/*!
\brief
  Query for support for the swap chain
*/
/****************************************************************************/
VK::SwapChain::SwapChainSupportDetails VK::SwapChain::QuerySwapChainSupport(VK::Surface& surface, VK::Device& device)
{
    SwapChainSupportDetails details;
    VkPhysicalDevice mDevice = device.GetPhysicalDevice();
    VkSurfaceKHR mSurface = surface.Get();

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice, mSurface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice, mSurface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice, mSurface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice, mSurface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice, mSurface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

/****************************************************************************/
/*!
\brief
  choose the format for the images in the swap chain
*/
/****************************************************************************/
VkSurfaceFormatKHR VK::SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

/****************************************************************************/
/*!
\brief
  choose the present mode for the swap chain
*/
/****************************************************************************/
VkPresentModeKHR VK::SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

/****************************************************************************/
/*!
\brief
  choose the extent for the swap chain
*/
/****************************************************************************/
VkExtent2D VK::SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent =
        {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}