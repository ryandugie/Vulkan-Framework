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
#include "DescriptorSet.hpp"
#include <array>

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
  create a new Descriptor Set

  This is omega trash
*/
/****************************************************************************/
void VK::DescriptorSet::Create(VK::Device& device, VK::DescriptorPool& pool, std::vector<VK::Buffer>& buffers,
    unsigned bufferCount, VkDeviceSize bufferSize, VkShaderStageFlags stage, std::vector<VK::Image*>& images, std::vector<VK::Sampler*> samplers)
{   
    // create bindings
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    if (bufferSize > 0)
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding = {};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = stage;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        bindings = { uboLayoutBinding };
    }

    unsigned bsize = unsigned(bindings.size());
    for (unsigned j = bsize; j < images.size() + bsize; ++j)
    {
        VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
        samplerLayoutBinding.binding = j;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = stage;
        bindings.push_back(samplerLayoutBinding);
    }

    // layout info
    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(device.Get(), &layoutInfo, nullptr, &mLayout) != VK_SUCCESS)
    {
        DEBUG::log.Error("DescriptorSet::Create: failed to allocate descriptor sets layout!");
        throw std::runtime_error("failed to allocate descriptor sets layout!");
    }

    std::vector<VkDescriptorSetLayout> layouts(bufferCount, mLayout);
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool.Get();
    allocInfo.descriptorSetCount = static_cast<uint32_t>(bufferCount);
    allocInfo.pSetLayouts = layouts.data();

    mSets.resize(bufferCount);
    if (vkAllocateDescriptorSets(device.Get(), &allocInfo, mSets.data()) != VK_SUCCESS)
    {
        DEBUG::log.Error("DescriptorSet::Create: failed to allocate descriptor sets!");
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    // for each buffer
    for (size_t i = 0; i < bufferCount; ++i)
    {
        std::vector<VkDescriptorImageInfo> imageInfo = {};
        imageInfo.resize(images.size());
        for (unsigned j = 0; j < images.size(); ++j)
        {
            imageInfo[j].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            imageInfo[j].imageView = images[j]->GetView();
            imageInfo[j].sampler = samplers[j]->Get();
        }

        std::vector<VkWriteDescriptorSet> descriptorWrites = {};
        if (bufferSize > 0)
        {
            // fill out each buffer and images info
            VkDescriptorBufferInfo bufferInfo = {};
            bufferInfo.buffer = buffers[i].Get();
            bufferInfo.offset = 0;
            bufferInfo.range = bufferSize;

            descriptorWrites.resize(1 + imageInfo.size());
            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = mSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            for (unsigned j = 1; j <= images.size(); ++j)
            {
                descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[j].dstSet = mSets[i];
                descriptorWrites[j].dstBinding = j;
                descriptorWrites[j].dstArrayElement = 0;
                descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrites[j].descriptorCount = 1;
                descriptorWrites[j].pImageInfo = &imageInfo[j - 1];
            }
        }
        else
        {
            descriptorWrites.resize(imageInfo.size());
            for (unsigned j = 0; j < images.size(); ++j)
            {
                descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[j].dstSet = mSets[i];
                descriptorWrites[j].dstBinding = j;
                descriptorWrites[j].dstArrayElement = 0;
                descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrites[j].descriptorCount = 1;
                descriptorWrites[j].pImageInfo = &imageInfo[j];
            }
        }


        // fill the set
        vkUpdateDescriptorSets(device.Get(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

/****************************************************************************/
/*!
\brief
  cleanup
*/
/****************************************************************************/
void VK::DescriptorSet::ShutDown(VK::Device& device)
{
    vkDestroyDescriptorSetLayout(device.Get(), mLayout, nullptr);
}

/****************************************************************************/
/*!
\brief
  get the desciptor layout
*/
/****************************************************************************/
VkDescriptorSetLayout VK::DescriptorSet::Layout()
{
    return mLayout;
}

/****************************************************************************/
/*!
\brief
  get a pointer to the desciptor layout
*/
/****************************************************************************/
VkDescriptorSetLayout* VK::DescriptorSet::GetPointerToLayout()
{
    return &mLayout;
}

/****************************************************************************/
/*!
\brief
  get a pointer to the desciptor sets
*/
/****************************************************************************/
std::vector<VkDescriptorSet>* VK::DescriptorSet::GetAll()
{
    return &mSets;
}

/****************************************************************************/
/*!
\brief
  get a specific desciptor set
*/
/****************************************************************************/
VkDescriptorSet VK::DescriptorSet::Get(unsigned i)
{
    return mSets[i];
}