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
#include "UBO.hpp"
#include <stdexcept>

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
  create a new UBO
*/
/****************************************************************************/
void VK::UBO::Create(VK::Device& device, unsigned bufferCount, VkDeviceSize bufferSize, VkShaderStageFlags stage, std::vector<VK::Image*> images, std::vector<VK::Sampler*> samplers)
{
    mUniformBuffers.resize(bufferCount);

    if (bufferSize > 0)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferSize;
        bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        for (size_t i = 0; i < bufferCount; ++i)
        {
            mUniformBuffers[i].Create(device, bufferInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        }
    }

    mPool.Create(device, bufferCount, bufferSize, unsigned(images.size()));
    mSets.Create(device, mPool, mUniformBuffers, bufferCount, bufferSize, stage, images, samplers);
}

/****************************************************************************/
/*!
\brief
   cleanup
*/
/****************************************************************************/
void VK::UBO::ShutDown(VK::Device& device)
{
    mSets.ShutDown(device);
    for (size_t i = 0; i < mUniformBuffers.size(); ++i)
    {
        mUniformBuffers[i].ShutDown(device);
    }
    mPool.ShutDown(device);
}

/****************************************************************************/
/*!
\brief
  update the content of the buffer
*/
/****************************************************************************/
void VK::UBO::Update(VK::Device& device, uint32_t imageIndex, void* uniformData, size_t size)
{
    mUniformBuffers[imageIndex].Fill(device, uniformData, size);
}

/****************************************************************************/
/*!
\brief
  get the descriptor layout
*/
/****************************************************************************/
VkDescriptorSetLayout VK::UBO::Layout()
{
    return mSets.Layout();
}

/****************************************************************************/
/*!
\brief
  get a pointer to the descriptor layout
*/
/****************************************************************************/
VkDescriptorSetLayout* VK::UBO::GetPointerToLayout()
{
    return mSets.GetPointerToLayout();
}

/****************************************************************************/
/*!
\brief
  get the descriptor pool
*/
/****************************************************************************/
VkDescriptorPool VK::UBO::Pool()
{
    return mPool.Get();
}

/****************************************************************************/
/*!
\brief
  get a pointer to the descriptor pool
*/
/****************************************************************************/
VkDescriptorPool* VK::UBO::GetPointerToPool()
{
    return mPool.GetPointerTo();
}

/****************************************************************************/
/*!
\brief
  get a pointer to all the descriptor sets
*/
/****************************************************************************/
std::vector<VkDescriptorSet>* VK::UBO::Sets()
{
    return mSets.GetAll();
}

/****************************************************************************/
/*!
\brief
  get a specific descriptor sets
*/
/****************************************************************************/
VkDescriptorSet VK::UBO::Set(unsigned i)
{
    return mSets.Get(i);
}