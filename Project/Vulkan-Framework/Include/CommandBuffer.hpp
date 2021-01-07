/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
#ifndef COMMANDBUFFERS_H
#define COMMANDBUFFERS_H
#pragma once


#include "Device.hpp"
#include "SwapChain.hpp"
#include "CommandPool.hpp"
#include "FrameBuffer.hpp"
#include "RenderPass.hpp"
#include "PipeLine.hpp"
#include "UBO.hpp"
#include <vector>

namespace VK
{
    class CommandBuffer : public std::vector<VkCommandBuffer>
    {
    public:
        void Create(VK::Device& device, VK::CommandPool& commandPool, unsigned size);
        void ShutDown(VK::Device& device, VK::CommandPool& commandPool);

        void Begin(unsigned index, std::vector<VK::FrameBuffer>& frameBuffers, VK::RenderPass& renderPass, VK::SwapChain& swapChain, unsigned colorCount, unsigned depthCount);
        void Begin(unsigned i);
        void End(unsigned index);
        void EndRT(unsigned i);
        void BindPipeline(unsigned index, VK::PipeLine& pipeLine);
        void BindPipelineRT(unsigned i, VK::PipeLine& pipeLine);
        void BindVertexBufferes(unsigned index, std::vector<VkBuffer> buffers, std::vector<VkDeviceSize>);
        void BindDescriptorSet(unsigned i, VK::PipeLine& pipeLine, std::vector<VkDescriptorSet> dSet);
        void DrawIndexed(unsigned index, unsigned instanceCount, VkBuffer* Indexbuffers, uint32_t indexCount);

    private:

    };

    VkCommandBuffer BeginSingleTimeCommands(VK::Device& device, VK::CommandPool& commandPool);
    void EndSingleTimeCommands(VK::Device& device, VK::CommandPool& commandPool, VkQueue& graphicsQueue, VkCommandBuffer& commandBuffer);
}
#endif
