/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
#ifndef DEBUGMESSENGER_H
#define DEBUGMESSENGER_H
#pragma once

#include "Instance.hpp"

namespace VK
{
    struct DebugMessengerInfo : public VkDebugUtilsMessengerCreateInfoEXT
    {
        DebugMessengerInfo();
    };

    class DebugMessenger
    {
    public:

        DebugMessenger() = default;
        ~DebugMessenger() = default;

        void ShutDown(VK::Instance& instance);
        void Create(VK::Instance& instance);

        VkDebugUtilsMessengerEXT Get() const;
        VkDebugUtilsMessengerEXT* GetPointerTo();

        DebugMessengerInfo GetInfo() const;
        DebugMessengerInfo* GetPointerToInfo();

    private:
        VkDebugUtilsMessengerEXT  mMessenger = VK_NULL_HANDLE;

        DebugMessengerInfo mMessengerInfo = {};
    };
}

#endif