/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
#ifndef DEVICE_H
#define DEVICE_H
#pragma once

#include <optional>
#include <vector>
#include "Instance.hpp"

namespace VK
{
    class Surface;

    class Device
    {
    public:
        struct QueueFamilyIndices
        {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;
            bool IsComplete();
        };

    private:
        class PhysicalDevice
        {
        public:

            void Create(VK::Instance& instance, VK::Surface& surface);
            void ShutDown();

            struct SwapChainSupportDetails
            {
                VkSurfaceCapabilitiesKHR capabilities;
                std::vector<VkSurfaceFormatKHR> formats;
                std::vector<VkPresentModeKHR> presentModes;
            };

            QueueFamilyIndices mQueueFamilyIndicies;

            const std::vector<const char*> mDeviceExtensions =
            {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            };

            VkPhysicalDeviceMemoryProperties mMemoryProperties = {};

            bool IsDeviceSuitable(VkPhysicalDevice device, VK::Surface& surface);
            void FindQueueFamilies(VkPhysicalDevice device, VK::Surface& surface);
            bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
            SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VK::Surface& surface);

            VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
        } mPhysicalDevice;

    public:

        void Create(VK::Instance& instance, VK::Surface& surface, VkQueue& graphicsQueue, VkQueue& presentationQueue);
        void ShutDown();

        VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        VkDevice Get() const;
        VkDevice* GetPointerTo();

        VkPhysicalDevice GetPhysicalDevice() const;
        VkPhysicalDevice* GetPointerToPhysicalDevice();
        
        QueueFamilyIndices GetQueueFamilies(VK::Surface& surface);
        uint32_t GraphicsFamily() const;
        uint32_t GetMemoryType(uint32_t typeBits, const VkMemoryPropertyFlags& properties);

    private:

        VkDevice mDevice = VK_NULL_HANDLE;
    };
}
#endif
