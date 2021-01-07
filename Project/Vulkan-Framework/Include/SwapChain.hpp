/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H
#pragma once

#include "Instance.hpp"
#include "Device.hpp"
#include "ImageView.hpp"

namespace VK
{
    class SwapChain
    {
    public:

        void Create(VK::Surface& surface, VK::Device& device, GLFWwindow* window);
        void ShutDown(VK::Device& device);

        VkSwapchainKHR Get() const;
        VkSwapchainKHR* GetPointerTo();

        std::vector<VkImage>* Images();
        std::vector<VK::ImageView>* ImageViews();
        VkFormat Format() const;
        VkExtent2D Extent() const;

    private:
        VkSwapchainKHR mSwapChain = VK_NULL_HANDLE;
        VkExtent2D mExtent;
        std::vector<VkImage> mImages;
        std::vector<VK::ImageView> mImageViews;
        VkFormat mImageFormat;

        struct SwapChainSupportDetails
        {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        SwapChainSupportDetails QuerySwapChainSupport(VK::Surface& surface, VK::Device& device);
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
    };
}
#endif
