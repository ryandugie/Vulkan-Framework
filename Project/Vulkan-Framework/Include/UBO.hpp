/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
#ifndef UBO_H
#define UBO_H
#pragma once

#include "Device.hpp"
#include "Buffer.hpp"
#include "DescriptorSet.hpp"
#include "DescriptorPool.hpp"
#include "Image.hpp"
#include "Sampler.hpp"

namespace VK
{
    class UBO
    {
    public:
        void Create(VK::Device& device, unsigned bufferCount, VkDeviceSize bufferSize, VkShaderStageFlags stage, std::vector<VK::Image*> images = {}, std::vector<VK::Sampler*> samplers = {});
        void ShutDown(VK::Device& device);
        void Update(VK::Device& device, uint32_t imageIndex, void* uniformData, size_t size);

        VkDescriptorSetLayout Layout();
        VkDescriptorSetLayout* GetPointerToLayout();
        VkDescriptorPool Pool();
        VkDescriptorPool* GetPointerToPool();
        std::vector<VkDescriptorSet>* Sets();
        VkDescriptorSet Set(unsigned i);

    private:
        VK::DescriptorPool mPool;
        VK::DescriptorSet mSets;
        std::vector<VK::Buffer> mUniformBuffers;
    };
}
#endif
