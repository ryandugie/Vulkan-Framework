/****************************************************************************/
/*!
\Author
   Ryan Dugie
\brief
    Copyright (c) Ryan Dugie. All rights reserved.
    Licensed under the Apache License 2.0
*/
/****************************************************************************/

#ifndef RENDERER_H
#define RENDERER_H
#pragma once

#include "Instance.hpp"
#include "DebugMessenger.hpp"
#include "Device.hpp"
#include "Surface.hpp"
#include "SwapChain.hpp"
#include "RenderPass.hpp"
#include "CommandPool.hpp"
#include "Pipeline.hpp"
#include "UBO.hpp"
#include "CommandBuffer.hpp"
#include "Semaphore.hpp"
#include "Fence.hpp"
#include "Image.hpp"
#include "CommandBuffer.hpp"
#include "Mesh.hpp"
#include <unordered_map>
#include <vector>
#include "Sampler.hpp"

struct GLFWwindow;
typedef GLFWwindow* WindowPtr;

namespace VK 
{
    struct MatrixBuffer
    {
        glm::mat4 world = {};
        glm::mat4 view = {};
        glm::mat4 proj = {};
    };
  
    class Renderer 
    {

    public:

        Renderer();
        ~Renderer();
        void Draw(float dt);
        void WaitIdle() const;


        WindowPtr Window() const;
        VK::Device Device() const;
        VK::CommandPool CommandPool() const;
        VkQueue GraphicsQueue() const;

    private:

        /* friends */
        friend static void FramebufferResizeCallback(WindowPtr window, int width, int height);

        /* Functions */
        void InitGLFW();
        void InitWindow();
        void InitVulkan();
        void InitSyncObjects();
        void InitRenderPass();
        void InitDepthResources();
        void InitPipelines();
        void InitFramebuffers();

        void UpdateCommandBuffers();
        void RecreateSwapChain();

        void ShutdownGLFW();
        void ShutdownWindow();
        void ShutdownVulkan();
        void ShutdownSwapChain();

        /* helpers */
        VkFormat FindDepthFormat();
        void DrawFrame(float dt);

        /* Variables */
        VK::Instance mInstance;
        VK::Device mDevice;
        VK::DebugMessenger mDebugMessenger;

        VK::Surface mSurface;
        VK::SwapChain mSwapChain;
        std::vector<VK::FrameBuffer> mFrameBuffers;

        VK::CommandPool mCommandPool;
        VK::CommandBuffer mCommandBuffer;

        VK::Image mDepthTexture;
        VK::RenderPass mRenderPass;

        VK::MatrixBuffer mMatrixBufferData;
        VK::UBO mMatrixBuffer;
        VK::PipeLine mPipeline;

        std::vector<VK::Semaphore> mImageAvailableSemaphores;
        std::vector<VK::Semaphore> mRenderFinishedSemaphores;
        std::vector<VK::Fence> mInFlightFences;
        std::vector<VK::Fence> mImagesInFlight;

        VkQueue mGraphicsQueue = 0;
        VkQueue mPresentQueue = 0;

        WindowPtr mWindow = nullptr;
        int mWindowWidth = 900;
        int mWindowHeight = 900;
        const int mMaxFramesInFlight = 2;
        size_t mCurrentFrame = 0;
        bool mFramebufferResized = false;

        /* Test scene */
        VK::Mesh mMesh;  


    };
}
#endif // RENDERER_H
