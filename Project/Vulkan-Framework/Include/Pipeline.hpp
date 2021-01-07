/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/
#ifndef PIPELINE_H
#define PIPELINE_H
#pragma once

#include "Device.hpp"
#include "SwapChain.hpp"
#include "RenderPass.hpp"
#include "Mesh.hpp"
#include "UBO.hpp"
#include <string>

namespace VK
{
    class PipeLine
    {
    public:
        void Create(VK::Device& device, VK::RenderPass& renderPass, VK::SwapChain& swapChain, VK::Mesh mesh,
            std::vector<VkDescriptorSetLayout> uniformBuffer, std::string vertexPath, std::string fragmentPath, unsigned colorAttachCount,
            VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT, bool depthTest = true, bool depthWrite = true, VkPolygonMode polyMode = VK_POLYGON_MODE_FILL);
        void ShutDown(VK::Device& device);

        VkPipeline Get() const;
        VkPipeline* GetPointerTo();

        VkPipelineLayout Layout() const;

    private:

        VkPipeline mPipeline = VK_NULL_HANDLE;
        VkPipelineLayout mLayout = VK_NULL_HANDLE;

        VkShaderModule CreateShaderModule(const std::vector<char>& code, VK::Device device);
    };
}
#endif
