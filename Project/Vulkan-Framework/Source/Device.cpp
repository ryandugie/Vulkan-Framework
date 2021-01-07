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
#include "Device.hpp"
#include "Surface.hpp"
#include <stdexcept>
#include <set>

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
  create a new logical device and choose a physical device
*/
/****************************************************************************/
void VK::Device::Create(VK::Instance& instance, VK::Surface& surface, VkQueue& graphicsQueue, VkQueue& presentationQueue)
{
    if (mDevice != VK_NULL_HANDLE)
        ShutDown();

    // physical device
    mPhysicalDevice.Create(instance, surface);

    mPhysicalDevice.FindQueueFamilies(mPhysicalDevice.mPhysicalDevice, surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { mPhysicalDevice.mQueueFamilyIndicies.graphicsFamily.value(),  mPhysicalDevice.mQueueFamilyIndicies.presentFamily.value() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.fillModeNonSolid = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(mPhysicalDevice.mDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = mPhysicalDevice.mDeviceExtensions.data();

    // validation layers
    if (instance.UsingValidationLayers())
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(mPhysicalDevice.mDeviceExtensions.size());
        createInfo.ppEnabledLayerNames = mPhysicalDevice.mDeviceExtensions.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    // make the device
    if (vkCreateDevice(mPhysicalDevice.mPhysicalDevice, &createInfo, nullptr, &mDevice) != VK_SUCCESS)
    {
        DEBUG::log.Error("Device::Create: failed to create logical device!");
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(mDevice, mPhysicalDevice.mQueueFamilyIndicies.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(mDevice, mPhysicalDevice.mQueueFamilyIndicies.presentFamily.value(), 0, &presentationQueue);
}

/****************************************************************************/
/*!
\brief
  cleanup
*/
/****************************************************************************/
void VK::Device::ShutDown()
{
    if (mDevice == VK_NULL_HANDLE)
        return;

    vkDestroyDevice(mDevice, nullptr);
    mDevice = VK_NULL_HANDLE;
}

/****************************************************************************/
/*!
\brief
  Get a the format from the physical device that fits the passed in parameters
*/
/****************************************************************************/
VkFormat VK::Device::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(GetPhysicalDevice(), format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    DEBUG::log.Error("debugLog.txt", "FindSupportedFormat: failed to find supported format!");
    throw std::runtime_error("failed to find supported format!");
}

/****************************************************************************/
/*!
\brief
  Get a pointer to the logical device
*/
/****************************************************************************/
VkDevice VK::Device::Get() const
{
    return mDevice;
}

/****************************************************************************/
/*!
\brief
  Get a pointer to pointer to the logical device
*/
/****************************************************************************/
VkDevice* VK::Device::GetPointerTo()
{
    return &mDevice;
}

/****************************************************************************/
/*!
\brief
  Get a pointer to the physical device
*/
/****************************************************************************/
VkPhysicalDevice VK::Device::GetPhysicalDevice() const
{
    return mPhysicalDevice.mPhysicalDevice;
}

/****************************************************************************/
/*!
\brief
  Get a pointer to a pointer to the physical device
*/
/****************************************************************************/
VkPhysicalDevice* VK::Device::GetPointerToPhysicalDevice()
{
    return &mPhysicalDevice.mPhysicalDevice;
}

/****************************************************************************/
/*!
\brief
  Get the graphics and present queue family indicies
*/
/****************************************************************************/
VK::Device::QueueFamilyIndices VK::Device::GetQueueFamilies(VK::Surface& surface)
{
    if (!mPhysicalDevice.mQueueFamilyIndicies.IsComplete())
    {
        mPhysicalDevice.FindQueueFamilies(mPhysicalDevice.mPhysicalDevice, surface);
    }

    return mPhysicalDevice.mQueueFamilyIndicies;
}

/****************************************************************************/
/*!
\brief
  Get the graphics family indicies
*/
/****************************************************************************/
uint32_t VK::Device::GraphicsFamily() const
{
    return mPhysicalDevice.mQueueFamilyIndicies.graphicsFamily.value();
}

/****************************************************************************/
/*!
\brief
  Get the index of the desired memory type
*/
/****************************************************************************/
uint32_t VK::Device::GetMemoryType(uint32_t typeBits, const VkMemoryPropertyFlags& properties)
{

    for (uint32_t i = 0; i < mPhysicalDevice.mMemoryProperties.memoryTypeCount; i++)
    {
        if (((typeBits & (1 << i)) > 0) && (mPhysicalDevice.mMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    assert(0);
    return ~0u;
}

/*============================================================================*\
|| ------------------------- PRIVATE FUNCTIONS ------------------------------ ||
\*============================================================================*/

/****************************************************************************/
/*!
\brief
  choose the physical device
*/
/****************************************************************************/
void VK::Device::PhysicalDevice::Create(VK::Instance& instance, VK::Surface& surface)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance.Get(), &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        DEBUG::log.Error("PhysicalDevice::Create: failed to find GPUs with Vulkan support!");
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance.Get(), &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        if (IsDeviceSuitable(device, surface))
        {
            mPhysicalDevice = device;
            break;
        }
    }

    if (mPhysicalDevice == VK_NULL_HANDLE)
    {
        DEBUG::log.Error("PhysicalDevice::Create: failed to find a suitable GPU!");
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &mMemoryProperties);
}

/****************************************************************************/
/*!
\brief
  cleanup
*/
/****************************************************************************/
void VK::Device::PhysicalDevice::ShutDown()
{
}

/****************************************************************************/
/*!
\brief
  determine if the current physical device canidate is Suitable for use
  with this program

\return
   true if suitable
*/
/****************************************************************************/
bool VK::Device::PhysicalDevice::IsDeviceSuitable(VkPhysicalDevice device, VK::Surface& surface)
{
    FindQueueFamilies(device, surface);

    bool extensionsSupported = CheckDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    return mQueueFamilyIndicies.IsComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

/****************************************************************************/
/*!
\brief
  determine if the queue family is complete

\return
   true if complete
*/
/****************************************************************************/
bool VK::Device::QueueFamilyIndices::IsComplete()
{
    return graphicsFamily.has_value() && presentFamily.has_value();
}

/****************************************************************************/
/*!
\brief
  find the indices for the queue families for this physical deivce
*/
/****************************************************************************/
void VK::Device::PhysicalDevice::FindQueueFamilies(VkPhysicalDevice device, VK::Surface& surface)
{
    if (mQueueFamilyIndicies.IsComplete())
        return;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            mQueueFamilyIndicies.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface.Get(), &presentSupport);

        if (presentSupport)
        {
            mQueueFamilyIndicies.presentFamily = i;
        }

        if (mQueueFamilyIndicies.IsComplete())
        {
            break;
        }

        ++i;
    }
}

/****************************************************************************/
/*!
\brief
  check if this device supports the required extensions
*/
/****************************************************************************/
bool VK::Device::PhysicalDevice::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(mDeviceExtensions.begin(), mDeviceExtensions.end());

#ifdef _DEBUG
    DEBUG::log.Info("_______________");
    DEBUG::log.Info("Vulkan Device Extensions:");
    DEBUG::log.Info(" - available:");
#endif // _DEBUG
    for (const auto& extension : availableExtensions)
    {
#ifdef _DEBUG
        DEBUG::log.Info(extension.extensionName);
#endif // _DEBUG
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

/****************************************************************************/
/*!
\brief
  get the physical device's support detailes for a swap chain
*/
/****************************************************************************/
VK::Device::PhysicalDevice::SwapChainSupportDetails VK::Device::PhysicalDevice::QuerySwapChainSupport(VkPhysicalDevice device, VK::Surface& surface)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface.Get(), &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.Get(), &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.Get(), &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.Get(), &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.Get(), &presentModeCount, details.presentModes.data());
    }

    return details;
}