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
#include "DebugMessenger.hpp"

/*============================================================================*\
|| --------------------------- GLOBAL VARIABLES ----------------------------- ||
\*============================================================================*/

/*============================================================================*\
|| -------------------------- STATIC FUNCTIONS ------------------------------ ||
\*============================================================================*/

namespace VK
{
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        UNUSED(messageType);
        UNUSED(pUserData);

        if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT || messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            DEBUG::log.Error("\nvalidation layer: ", pCallbackData->pMessage, "\n");
            std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        }
#ifdef _DEBUG
        else
        {
            DEBUG::log.Info("\nvalidation layer: ", pCallbackData->pMessage);
        }
#endif // _DEBUG

        return VK_FALSE;
    }
}

/*============================================================================*\
|| -------------------------- PUBLIC FUNCTIONS ------------------------------ ||
\*============================================================================*/

/****************************************************************************/
/*!
\brief
  Fill the messenger info
*/
/****************************************************************************/
VK::DebugMessengerInfo::DebugMessengerInfo()
{
    sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    pNext = nullptr;
    flags = 0;
    messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    pfnUserCallback = VK::debugCallback;
}

/****************************************************************************/
/*!
\brief
  cleanup
*/
/****************************************************************************/
void VK::DebugMessenger::ShutDown(VK::Instance& instance)
{
    if (mMessenger == VK_NULL_HANDLE)
        return;

    if (instance.UsingValidationLayers())
    {
        PFN_vkDestroyDebugUtilsMessengerEXT func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance.Get(), "vkDestroyDebugUtilsMessengerEXT"));
        if (func != nullptr)
        {
            func(instance.Get(), mMessenger, nullptr);
        }
    }

    mMessenger = VK_NULL_HANDLE;
}

/****************************************************************************/
/*!
\brief
  allocate a new vulkan debug messenger
*/
/****************************************************************************/
void VK::DebugMessenger::Create(VK::Instance& instance)
{
    if (mMessenger != VK_NULL_HANDLE)
        ShutDown(instance);

    if (!instance.UsingValidationLayers())
        return;

    PFN_vkCreateDebugUtilsMessengerEXT func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance.Get(), "vkCreateDebugUtilsMessengerEXT"));

    if (func != nullptr)
    {
        func(instance.Get(), &mMessengerInfo, nullptr, &mMessenger);
    }
    else
    {

        DEBUG::log.Error("DebugMessenger::Create: failed to set up debug messenger!");
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

/****************************************************************************/
/*!
\brief
  get a pointer to the debug messenger
*/
/****************************************************************************/
VkDebugUtilsMessengerEXT VK::DebugMessenger::Get() const
{
    return mMessenger;
}

/****************************************************************************/
/*!
\brief
  get a pointer to a pointer to the debug messenger
*/
/****************************************************************************/
VkDebugUtilsMessengerEXT* VK::DebugMessenger::GetPointerTo()
{
    return &mMessenger;
}

/****************************************************************************/
/*!
\brief
  get a pointer to the debug messenger info
*/
/****************************************************************************/
VK::DebugMessengerInfo VK::DebugMessenger::GetInfo() const
{
    return mMessengerInfo;
}

/****************************************************************************/
/*!
\brief
  get a pointer to a pointer to the debug messenger info
*/
/****************************************************************************/
VK::DebugMessengerInfo* VK::DebugMessenger::GetPointerToInfo()
{
    return &mMessengerInfo;
}