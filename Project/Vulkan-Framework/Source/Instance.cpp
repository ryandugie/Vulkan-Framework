//****************************************************************************/
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
#include "Instance.hpp"
#include "DebugMessenger.hpp"

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
  Generate a new vulkan instance
*/
/****************************************************************************/
void VK::Instance::Create()
{
    if (mInstance != VK_NULL_HANDLE)
    {
        ShutDown();
    }

    if (mEnableValidationLayers && !CheckValidationLayerSupport())
    {
        DEBUG::log.Error("Instance::Create: validation layers requested, but not available!");
        throw std::runtime_error("validation layers requested, but not available!");
    }

    // popluate application information
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "LightCubes";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "LightCubesRenderer";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_MAKE_VERSION(1, 2, 0);

#ifdef _DEBUG
    {
        uint32_t version;
        if (vkEnumerateInstanceVersion(&version) == VK_SUCCESS)
        {
            DEBUG::log.Info("_______________");
            DEBUG::log.Info("Vulkan Version:");
            DEBUG::log.Info(" - available: ", VK_VERSION_MAJOR(version), VK_VERSION_MINOR(version), VK_VERSION_PATCH(version));
            DEBUG::log.Info(" - requesting: ", 1, 2, 0);
        }
    }
#endif // _DEBUG


    // populate instance info
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    if (mEnableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

#ifdef _DEBUG
    DEBUG::log.Info("_______________");
    DEBUG::log.Info("Instance Extentions:");
    DEBUG::log.Info(" - requesting:");
    for (const auto& extension : extensions)
    {
        DEBUG::log.Info(extension);
    }
#endif // _DEBUG

    // attach instance debug messenger
    if (mEnableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
        createInfo.ppEnabledLayerNames = mValidationLayers.data();
        VK::DebugMessengerInfo debugCreateInfo;
        createInfo.pNext = &debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    // create the instance
    if (vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS)
    {
        DEBUG::log.Error("Instance::Create: failed to create instance!");
        throw std::runtime_error("failed to create instance!");
    }
}

/****************************************************************************/
/*!
\brief
  clean up
*/
/****************************************************************************/
void VK::Instance::ShutDown()
{
    if (mInstance == VK_NULL_HANDLE)
        return;

    vkDestroyInstance(mInstance, nullptr);
    mInstance = VK_NULL_HANDLE;
}

/****************************************************************************/
/*!
\brief
  get if validation layers are enabled

\return
  bool
*/
/****************************************************************************/
bool VK::Instance::UsingValidationLayers() const
{
    return mEnableValidationLayers;
}

/****************************************************************************/
/*!
\brief
  get a pointer to the instance

\return
  a pointer to the instance
*/
/****************************************************************************/
VkInstance VK::Instance::Get() const
{
    return mInstance;
}

/****************************************************************************/
/*!
\brief
  get a pointer to a pointer to the instance

\return
  a pointer to a pointer to  the instance
*/
/****************************************************************************/
VkInstance* VK::Instance::GetPointerTo()
{
    return &mInstance;
}

/*============================================================================*\
|| ------------------------- PRIVATE FUNCTIONS ------------------------------ ||
\*============================================================================*/

/****************************************************************************/
/*!
\brief
  check that validation layers are installed on this computer

\return
  true if validation layers are installed
*/
/****************************************************************************/
bool VK::Instance::CheckValidationLayerSupport() const
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
#ifdef _DEBUG
        DEBUG::log.Info("_______________");
        DEBUG::log.Info("Instance Valadation Layers:");
        DEBUG::log.Info(" - available:");
        for (const auto& layerProperties : availableLayers)
        {
            DEBUG::log.Info(layerProperties.layerName);
        }
        DEBUG::log.Info(" - requesting:");
#endif // _DEBUG
    for (const char* layerName : mValidationLayers)
    {
#ifdef _DEBUG
        DEBUG::log.Info(layerName);
#endif // _DEBUG
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
#ifdef _DEBUG
            DEBUG::log.Info(layerName, "-- Not Found! Breaking operation.");
#endif // _DEBUG
            return false;
        }
    }

    return true;
}