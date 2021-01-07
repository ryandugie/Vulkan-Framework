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
#include "Renderer.hpp"
#include <array>
/*============================================================================*\
|| --------------------------- GLOBAL VARIABLES ----------------------------- ||
\*============================================================================*/

/*============================================================================*\
|| -------------------------- STATIC FUNCTIONS ------------------------------ ||
\*============================================================================*/

namespace VK {
    /****************************************************************************/
    /*!
    \brief
      Callback for all GLFW errors

    \param error
      The GLFW error code

    \param description
      Error description
    */
    /****************************************************************************/
    static void GLFWErrorCallback(int error, const char* description)
    {
        DEBUG::log.Error("GLFW Error ", error, " : ", description);
    }

    /****************************************************************************/
    /*!
    \brief
      Called when our window gets resized,
      signal the renderer to resize its buffers

    \param window
      The window that got resized

    \param width
      The new width

    \param height
      The new height
    */
    /****************************************************************************/
    static void FramebufferResizeCallback(WindowPtr window, int width, int height)
    {
        VK::Renderer* renderer = reinterpret_cast<VK::Renderer*>(glfwGetWindowUserPointer(window));
        renderer->mFramebufferResized = true;
        renderer->mWindowWidth = width;
        renderer->mWindowHeight = height;
    }
}

/*============================================================================*\
|| -------------------------- PUBLIC FUNCTIONS ------------------------------ ||
\*============================================================================*/

/****************************************************************************/
/*!
\brief
  Initialize the renderer
*/
/****************************************************************************/
VK::Renderer::Renderer()
{
    // Setup GLFW
    InitGLFW();

    // Setup Vulkan
    InitVulkan();

    // Camera
    float y = -0.1f;
    glm::vec3 position = { 0, y, 1 };
    glm::vec3 up = { 0, 1, 0 };
    float fov = 0.42173f;
    float aspectRatio = float(mWindowWidth) / mWindowHeight;
    float nearPlane = 0.1f;
    float farPlane = 250.f;

    mMatrixBufferData.proj = glm::perspective(fov, aspectRatio, nearPlane, farPlane);
    mMatrixBufferData.view = glm::lookAt(position, glm::vec3(0.0f, y, 0.0f), up);
}

/****************************************************************************/
/*!
\brief
  Clean up
*/
/****************************************************************************/
VK::Renderer::~Renderer()
{
    WaitIdle();

    // Cleanup Vulkan
    ShutdownVulkan();

    // All GLFW related clean up
    ShutdownGLFW();
}

/****************************************************************************/
/*!
\brief
  Render

\param dt
  Delta-Time
*/
/****************************************************************************/
void VK::Renderer::Draw(float dt)
{
    static float mAngle = 0;
    mAngle -= dt;
    mMatrixBufferData.world = glm::mat4(1);
    mMatrixBufferData.world = glm::rotate(mMatrixBufferData.world, mAngle, { 0, 1, 0 });

    glfwPollEvents();
    DrawFrame(dt);
}

/****************************************************************************/
/*!
\brief
  wait for all threads to finish rendering
*/
/****************************************************************************/
void VK::Renderer::WaitIdle() const
{
    vkDeviceWaitIdle(mDevice.Get());
}

/****************************************************************************/
/*!
\brief
  Get the application window

\return
  Pointer to the window

*/
/****************************************************************************/
WindowPtr VK::Renderer::Window() const
{
    return mWindow;
}

/****************************************************************************/
/*!
\brief
  Get the vulkan device
*/
/****************************************************************************/
VK::Device VK::Renderer::Device() const
{
    return mDevice;
}

/****************************************************************************/
/*!
\brief
  Get the main commandpool
*/
/****************************************************************************/
VK::CommandPool VK::Renderer::CommandPool() const
{
    return mCommandPool;
}

/****************************************************************************/
/*!
\brief
  Get the graphics VkQueue
*/
/****************************************************************************/
VkQueue VK::Renderer::GraphicsQueue() const
{
    return mGraphicsQueue;
}

/*============================================================================*\
|| ------------------------- PRIVATE FUNCTIONS ------------------------------ ||
\*============================================================================*/

/****************************************************************************/
/*!
\brief
  Initialize GLFW
*/
/****************************************************************************/
void VK::Renderer::InitGLFW()
{
    glfwSetErrorCallback(VK::GLFWErrorCallback);
    if (!glfwInit())
    {
        throw std::runtime_error("GLFW: glfwInit() failed! Check error callback!\n");
    }

    // create the GLFW window
    InitWindow();
}

/****************************************************************************/
/*!
\brief
  Create the application window
*/
/****************************************************************************/
void VK::Renderer::InitWindow()
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    mWindow = glfwCreateWindow(mWindowWidth, mWindowHeight, "Vulkan-Framework", nullptr, nullptr);
    glfwSetWindowUserPointer(mWindow, this);
    glfwSetFramebufferSizeCallback(mWindow, VK::FramebufferResizeCallback);
}

/****************************************************************************/
/*!
\brief
  Initialize Vulkan
*/
/****************************************************************************/
void VK::Renderer::InitVulkan()
{
    if (!glfwVulkanSupported())
    {
        throw std::runtime_error("GLFW: Vulkan Not Supported\n");
    }

    mInstance.Create();
    mDebugMessenger.Create(mInstance);
    mSurface.Create(mInstance, mWindow);
    mDevice.Create(mInstance, mSurface, mGraphicsQueue, mPresentQueue);

    mSwapChain.Create(mSurface, mDevice, mWindow);
    mCommandPool.Create(mDevice, mSurface);
    mCommandBuffer.Create(mDevice, mCommandPool, unsigned(mSwapChain.Images()->size()));
    InitSyncObjects();
    InitRenderPass();
    InitDepthResources();
    InitFramebuffers();
    InitPipelines();

    UpdateCommandBuffers();
}

/****************************************************************************/
/*!
\brief
  Create Vulkan Semaphores and Fences
*/
/****************************************************************************/
void VK::Renderer::InitSyncObjects()
{
    mImageAvailableSemaphores.resize(mMaxFramesInFlight);
    mRenderFinishedSemaphores.resize(mMaxFramesInFlight);
    mInFlightFences.resize(mMaxFramesInFlight);
    mImagesInFlight.resize(mSwapChain.Images()->size());

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < mMaxFramesInFlight; ++i)
    {
        mImageAvailableSemaphores[i].Create(mDevice, semaphoreInfo);
        mRenderFinishedSemaphores[i].Create(mDevice, semaphoreInfo);
        mInFlightFences[i].Create(mDevice, fenceInfo);
    }
}

/****************************************************************************/
/*!
\brief
  Create all renderpass
*/
/****************************************************************************/
void VK::Renderer::InitRenderPass()
{
    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = FindDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = mSwapChain.Format();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };;
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    mRenderPass.Create(mDevice, renderPassInfo);
}

/****************************************************************************/
/*!
\brief
  Create depth textures
*/
/****************************************************************************/
void VK::Renderer::InitDepthResources()
{
    VkFormat depthFormat = FindDepthFormat();

    mDepthTexture.Create(mDevice, mSwapChain.Extent().width, mSwapChain.Extent().height,
        depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_UNDEFINED);
}

/****************************************************************************/
/*!
\brief
  Create all pipelines
*/
/****************************************************************************/
void VK::Renderer::InitPipelines()
{
    mMesh.Create(mDevice, mCommandPool, mGraphicsQueue, "../Resource/Models/StanfordBunny.obj");
    mMatrixBuffer.Create(mDevice, unsigned(mSwapChain.Images()->size()), sizeof(MatrixBuffer), VK_SHADER_STAGE_VERTEX_BIT);

    mPipeline.Create(mDevice, mRenderPass, mSwapChain, mMesh, { mMatrixBuffer.Layout() },
          "../Resource/Shaders/Simple.vert.spv", "../Resource/Shaders/Simple.frag.spv", 1, VK_CULL_MODE_BACK_BIT, true, true);
}

/****************************************************************************/
/*!
\brief
  Create all framebuffers
*/
/****************************************************************************/
void VK::Renderer::InitFramebuffers()
{
    VkExtent2D extent = mSwapChain.Extent();
    std::vector<VK::ImageView>* imageViews = mSwapChain.ImageViews();
    mFrameBuffers.resize(imageViews->size());

    for (size_t i = 0; i < imageViews->size(); ++i)
    {
        std::array<VkImageView, 2> attachments =
        {
            (*imageViews)[i].Get(), mDepthTexture.GetView()
        };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = mRenderPass.Get();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        mFrameBuffers[i].Create(mDevice, framebufferInfo);
    }
}

/****************************************************************************/
/*!
\brief
  fill all command buffers
*/
/****************************************************************************/
void VK::Renderer::UpdateCommandBuffers()
{
    for (unsigned i = 0; i < mCommandBuffer.size(); ++i)
    {
        vkResetCommandBuffer(mCommandBuffer[i], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
        mCommandBuffer.Begin(i, mFrameBuffers,mRenderPass, mSwapChain, 1, 1);

        mCommandBuffer.BindVertexBufferes(i, { mMesh.Buffer()->Get() }, { 0,0 });
        mCommandBuffer.BindPipeline(i, mPipeline);
        mCommandBuffer.BindDescriptorSet(i, mPipeline, { mMatrixBuffer.Set(i) });

        mCommandBuffer.DrawIndexed(i, 1, mMesh.IndexBuffer()->GetPointerTo(), mMesh.IndexCount());
        mCommandBuffer.End(i);
    }
}

/****************************************************************************/
/*!
\brief
  Recreated everything that needs to be resized upon window resize.
*/
/****************************************************************************/
void VK::Renderer::RecreateSwapChain()
{
    WaitIdle();

    ShutdownSwapChain();

    mSwapChain.Create(mSurface, mDevice, mWindow);
    mCommandPool.Create(mDevice, mSurface);
    mCommandBuffer.Create(mDevice, mCommandPool, unsigned(mSwapChain.Images()->size()));

    InitRenderPass();
    InitDepthResources();
    InitFramebuffers();
    InitPipelines();
    UpdateCommandBuffers();
}

/****************************************************************************/
/*!
\brief
  all GLFW related cleanup
*/
/****************************************************************************/
void VK::Renderer::ShutdownGLFW()
{
    ShutdownWindow();
    glfwTerminate();
}

/****************************************************************************/
/*!
\brief
  destroy the application window
*/
/****************************************************************************/
void VK::Renderer::ShutdownWindow()
{
    glfwDestroyWindow(mWindow);
}

/****************************************************************************/
/*!
\brief
  all Vulkan related cleanup
*/
/****************************************************************************/
void VK::Renderer::ShutdownVulkan()
{
    WaitIdle();
    ShutdownSwapChain();

    for (size_t i = 0; i < mMaxFramesInFlight; ++i)
    {
        mRenderFinishedSemaphores[i].ShutDown(mDevice);
        mImageAvailableSemaphores[i].ShutDown(mDevice);
        mInFlightFences[i].ShutDown(mDevice);
    }


    mDevice.ShutDown();
    mSurface.ShutDown(mInstance);
    mDebugMessenger.ShutDown(mInstance);
    mInstance.ShutDown();
}

/****************************************************************************/
/*!
\brief
  shutdown everything swapchain related
*/
/****************************************************************************/
void VK::Renderer::ShutdownSwapChain()
{
    size_t frameBufferCount = mFrameBuffers.size();
    for (size_t i = 0; i < frameBufferCount; ++i)
    {
        mFrameBuffers[i].ShutDown(mDevice);
    }

    mMesh.ShutDown(mDevice);
    mMatrixBuffer.ShutDown(mDevice);

    mPipeline.ShutDown(mDevice);
    mRenderPass.ShutDown(mDevice);
    mDepthTexture.ShutDown(mDevice);
    mSwapChain.ShutDown(mDevice);
    mCommandBuffer.ShutDown(mDevice, mCommandPool);
    mCommandPool.ShutDown(mDevice);
}

/*============================================================================*\
|| ------------------------- PRIVATE FUNCTIONS ------------------------------ ||
\*============================================================================*/

/****************************************************************************/
/*!
\brief
  get the desired depth format
*/
/****************************************************************************/
VkFormat VK::Renderer::FindDepthFormat()
{
    return mDevice.FindSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

/****************************************************************************/
/*!
\brief
  Render a frame
*/
/****************************************************************************/
void VK::Renderer::DrawFrame(float dt)
{
    // wait for active frame
    vkWaitForFences(mDevice.Get(), 1, mInFlightFences[mCurrentFrame].GetPointerTo(), VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(mDevice.Get(), mSwapChain.Get(), UINT64_MAX, mImageAvailableSemaphores[mCurrentFrame].Get(), VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
       RecreateSwapChain();
       return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
       DEBUG::log.Error("DrawFrame: failed to acquire swap chain image!");
       throw std::runtime_error("failed to acquire swap chain image!");
    }

    if (mImagesInFlight[imageIndex].Get() != VK_NULL_HANDLE)
    {
        vkWaitForFences(mDevice.Get(), 1, mImagesInFlight[imageIndex].GetPointerTo(), VK_TRUE, UINT64_MAX);
    }
    mImagesInFlight[imageIndex] = mInFlightFences[mCurrentFrame];

    // update buffers
    mMatrixBuffer.Update(mDevice, imageIndex, &mMatrixBufferData, sizeof(MatrixBuffer));

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame].Get() };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    std::vector<VkCommandBuffer> buffers;
    buffers = { mCommandBuffer[imageIndex] };

    submitInfo.commandBufferCount = uint32_t(buffers.size());
    submitInfo.pCommandBuffers = buffers.data();

    VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrentFrame].Get() };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(mDevice.Get(), 1, mInFlightFences[mCurrentFrame].GetPointerTo());

    // display this frame
    if (vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, mInFlightFences[mCurrentFrame].Get()) != VK_SUCCESS)
    {
        DEBUG::log.Error("DrawFrame: failed to submit draw command buffer!");
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { mSwapChain.Get() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(mPresentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mFramebufferResized)
    {
        mFramebufferResized = false;
        RecreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
        DEBUG::log.Error("debugLog.txt", "DrawFrame: failed to present swap chain image!");
        throw std::runtime_error("failed to present swap chain image!");
    }

    mCurrentFrame = (mCurrentFrame + 1) % mMaxFramesInFlight;
}
