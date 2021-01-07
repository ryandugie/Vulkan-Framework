/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
#ifndef DESCRIPTORSET_H
#define DESCRIPTORSET_H
#pragma once

#include "Device.hpp"
#include "Buffer.hpp"
#include "DescriptorPool.hpp"
#include "Image.hpp"

namespace VK
{
    class DescriptorSet
    {
    public:
        void Create(VK::Device& device, VK::DescriptorPool& pool, std::vector<VK::Buffer>& buffers,
            unsigned bufferCount, VkDeviceSize bufferSize, VkShaderStageFlags stage, std::vector<VK::Image*>& images, std::vector<VK::Sampler*> samplers);

        void ShutDown(VK::Device& device);

        VkDescriptorSetLayout Layout();
        VkDescriptorSetLayout* GetPointerToLayout();
        std::vector<VkDescriptorSet>* GetAll();
        VkDescriptorSet Get(unsigned i);

    private:
        VkDescriptorSetLayout mLayout = VK_NULL_HANDLE;
        std::vector<VkDescriptorSet> mSets;
    };
}
#endif
