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
#include "CommandBuffer.hpp"

/*============================================================================*\
|| --------------------------- GLOBAL VARIABLES ----------------------------- ||
\*============================================================================*/

/*============================================================================*\
|| -------------------------- STATIC FUNCTIONS ------------------------------ ||
\*============================================================================*/
/****************************************************************************/
/*!
\brief
  create and begin a single-use command buffer
*/
/****************************************************************************/
VkCommandBuffer VK::BeginSingleTimeCommands(VK::Device& device, VK::CommandPool& commandPool)
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool.Get();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device.Get(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

/****************************************************************************/
/*!
\brief
  submit and delete a single-use command buffer
*/
/****************************************************************************/
void  VK::EndSingleTimeCommands(VK::Device& device, VK::CommandPool& commandPool, VkQueue& graphicsQueue, VkCommandBuffer& commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkDeviceWaitIdle(device.Get());
    vkFreeCommandBuffers(device.Get(), commandPool.Get(), 1, &commandBuffer);
}

/*============================================================================*\
|| -------------------------- PUBLIC FUNCTIONS ------------------------------ ||
\*============================================================================*/

/****************************************************************************/
/*!
\brief
  create a new CommandBuffer
*/
/****************************************************************************/
void VK::CommandBuffer::Create(VK::Device& device, VK::CommandPool& commandPool, unsigned size) 
{
    this->resize(size);
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool.Get();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)this->size();

    if (vkAllocateCommandBuffers(device.Get(), &allocInfo, this->data()) != VK_SUCCESS)
    {
        DEBUG::log.Error("CommandBuffer::Create:failed to allocate command buffers!");
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

/****************************************************************************/
/*!
\brief
  cleanup
*/
/****************************************************************************/
void VK::CommandBuffer::ShutDown(VK::Device& device, VK::CommandPool& commandPool)
{
    vkFreeCommandBuffers(device.Get(), commandPool.Get(), static_cast<uint32_t>(this->size()), this->data());
}

/****************************************************************************/
/*!
\brief
  start recording commands
*/
/****************************************************************************/
void VK::CommandBuffer::Begin(unsigned i, std::vector<VK::FrameBuffer>& frameBuffers, VK::RenderPass& renderPass, VK::SwapChain& swapChain, unsigned colorCount, unsigned depthCount)
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer((*this)[i], &beginInfo) != VK_SUCCESS)
    {
        DEBUG::log.Error("CommandBuffer::Begin: failed to begin recording command buffer!");
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass.Get();
    renderPassInfo.framebuffer = frameBuffers[i].Get();
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChain.Extent();

    std::vector<VkClearValue> clearValues = {};
    clearValues.resize(colorCount + depthCount);
    for (unsigned j = 0; j < colorCount; ++j)
        clearValues[j].color = { 0.2f, 0.2f, 0.2f, 1.0f };

    for (unsigned j = colorCount; j < colorCount + depthCount; ++j)
        clearValues[j].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass((*this)[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

/****************************************************************************/
/*!
\brief
  start recording commands
*/
/****************************************************************************/
void VK::CommandBuffer::Begin(unsigned i)
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer((*this)[i], &beginInfo) != VK_SUCCESS)
    {
        DEBUG::log.Error("CommandBuffer::Begin: failed to begin recording command buffer!");
        throw std::runtime_error("failed to begin recording command buffer!");
    }
}

/****************************************************************************/
/*!
\brief
  stop recording commands
*/
/****************************************************************************/
void VK::CommandBuffer::End(unsigned i)
{
    vkCmdEndRenderPass((*this)[i]);

    if (vkEndCommandBuffer((*this)[i]) != VK_SUCCESS)
    {
        DEBUG::log.Error("CommandBuffer::End: failed to record command buffer!");
        throw std::runtime_error("failed to record command buffer!");
    }
}

/****************************************************************************/
/*!
\brief
  stop recording commands
*/
/****************************************************************************/
void VK::CommandBuffer::EndRT(unsigned i)
{
    if (vkEndCommandBuffer((*this)[i]) != VK_SUCCESS)
    {
        DEBUG::log.Error("CommandBuffer::End: failed to record command buffer!");
        throw std::runtime_error("failed to record command buffer!");
    }
}

/****************************************************************************/
/*!
\brief
  set the active pipeline
*/
/****************************************************************************/
void VK::CommandBuffer::BindPipeline(unsigned i, VK::PipeLine& pipeLine)
{
    vkCmdBindPipeline((*this)[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeLine.Get());
}

/****************************************************************************/
/*!
\brief
  set the active pipeline
*/
/****************************************************************************/
void VK::CommandBuffer::BindPipelineRT(unsigned i, VK::PipeLine& pipeLine)
{
    vkCmdBindPipeline((*this)[i], VK_PIPELINE_BIND_POINT_RAY_TRACING_NV, pipeLine.Get());
}

/****************************************************************************/
/*!
\brief
  set the vertex buffer
*/
/****************************************************************************/
void VK::CommandBuffer::BindVertexBufferes(unsigned i, std::vector<VkBuffer> buffers, std::vector<VkDeviceSize> offsets)
{
    vkCmdBindVertexBuffers((*this)[i], 0, uint32_t(buffers.size()), buffers.data(), offsets.data());
}

/****************************************************************************/
/*!
\brief
  set the active descriptor set
*/
/****************************************************************************/
void VK::CommandBuffer::BindDescriptorSet(unsigned i, VK::PipeLine& pipeLine, std::vector<VkDescriptorSet> dSet)
{
    vkCmdBindDescriptorSets((*this)[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeLine.Layout(), 0, uint32_t(dSet.size()), dSet.data(), 0, nullptr);
}

/****************************************************************************/
/*!
\brief
  draw with an EBO
*/
/****************************************************************************/
void VK::CommandBuffer::DrawIndexed(unsigned i, unsigned instanceCount, VkBuffer* indexBuffer, uint32_t indexCount)
{
    vkCmdBindIndexBuffer((*this)[i], *indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed((*this)[i], indexCount, instanceCount, 0, 0, 0);
}