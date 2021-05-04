/**
 * @file
 * @author Max Godefroy
 * @date 04/05/2021.
 */

#include <fstream>
#include "Utils.hpp"
#include "Queue.hpp"

#include "SwapQueue.hpp"


using VulkanHelpers::assertResult;


SwapQueue::SwapQueue(const PhysicalDevice &physicalDevice,
                     const SurfaceKHR &surface,
                     GLFWwindow *window,
                     CommandPool *commandPool,
                     Device *device)
{
    this->commandPool = commandPool;
    this->device = device;

    this->initSwapChain(physicalDevice, surface, window);
    this->setUpImageViews();
    this->createRenderPass();
    this->createGraphicsPipeline();
    this->createFramebuffers();
    this->createCommandBuffers();
}


SwapQueue::~SwapQueue()
{
    this->device->freeCommandBuffers(*this->commandPool, this->commandBuffers);

    for (const auto &fb : this->scFramebuffers)
        this->device->destroyFramebuffer(fb);

    this->device->destroyPipeline(this->graphicsPipeline);
    this->device->destroyPipelineLayout(this->pipelineLayout);

    this->device->destroyRenderPass(this->renderPass);

    for (const auto &iv : this->scImageViews)
        this->device->destroyImageView(iv);

    this->device->destroy(this->swapchain);
}


SwapQueue::SwapChainSupportDetails SwapQueue::querySwapChainDetails(const PhysicalDevice &physicalDevice, const SurfaceKHR &surface)
{
    SwapChainSupportDetails details;

    details.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
    details.formats = physicalDevice.getSurfaceFormatsKHR(surface);
    details.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);

    return details;
}


SurfaceFormatKHR SwapQueue::chooseSwapSurfaceFormat(const std::vector<SurfaceFormatKHR> &availableFormats)
{
    for (const auto &format : availableFormats)
    {
        if (format.format == Format::eB8G8R8A8Srgb && format.colorSpace == ColorSpaceKHR::eSrgbNonlinear)
            return format;
    }
    return availableFormats[0];
}


PresentModeKHR SwapQueue::chooseSwapPresentMode(const std::vector<PresentModeKHR> &availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == PresentModeKHR::eMailbox)
            return availablePresentMode;
    }

    return PresentModeKHR::eFifo;
}


Extent2D SwapQueue::chooseSwapExtent(const SurfaceCapabilitiesKHR &capabilities, GLFWwindow *window)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;

    else
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
        };

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}


std::vector<char> SwapQueue::readFile(const char *filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("Unable to open file");

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}


void SwapQueue::initSwapChain(const PhysicalDevice &physicalDevice, const SurfaceKHR &surface, GLFWwindow *window)
{
    auto details = querySwapChainDetails(physicalDevice, surface);

    auto surfaceFormat = chooseSwapSurfaceFormat(details.formats);
    auto presentMode = chooseSwapPresentMode(details.presentModes);
    auto extent = chooseSwapExtent(details.capabilities, window);

    uint32_t imageCount = details.capabilities.minImageCount + 1;

    if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount)
        imageCount = details.capabilities.maxImageCount;

    const auto indices = VulkanHelpers::findQueueFamilies(physicalDevice, surface);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    bool concurrent = indices.graphicsFamily != indices.presentFamily;

    SwapchainCreateInfoKHR createInfo({}, surface,
                                      imageCount,
                                      surfaceFormat.format, surfaceFormat.colorSpace,
                                      extent,
                                      1, ImageUsageFlagBits::eColorAttachment,
                                      concurrent ? SharingMode::eConcurrent : SharingMode::eExclusive,
                                      concurrent ? 2 : 0,
                                      concurrent ? queueFamilyIndices : nullptr,
                                      details.capabilities.currentTransform,
                                      CompositeAlphaFlagBitsKHR::eOpaque,
                                      presentMode,
                                      true,
                                      {});

    assertResult(this->device->createSwapchainKHR(&createInfo, nullptr, &this->swapchain),
                 "Unable to create swapchain");

    this->scImages = this->device->getSwapchainImagesKHR(this->swapchain);

    this->scExtend = extent;
    this->scImageFormat = surfaceFormat.format;
}


void SwapQueue::setUpImageViews()
{
    this->scImageViews.resize(this->scImageViews.size());

    for (auto i = 0; i < this->scImageViews.size(); i++)
    {
        ImageViewCreateInfo createInfo({}, this->scImages[i],
                                       ImageViewType::e2D,
                                       this->scImageFormat,
                                       {},
                                       { ImageAspectFlagBits::eColor, 0, 1, 0, 1 });

        assertResult(this->device->createImageView(&createInfo, nullptr, &this->scImageViews[i]),
                     "Unable to initialize image view");
    }
}


void SwapQueue::createRenderPass()
{
    AttachmentDescription colorAttachment({}, this->scImageFormat, SampleCountFlagBits::e1,
                                          AttachmentLoadOp::eClear, AttachmentStoreOp::eStore,
                                          AttachmentLoadOp::eDontCare, AttachmentStoreOp::eDontCare,
                                          ImageLayout::eUndefined, ImageLayout::ePresentSrcKHR);

    AttachmentReference colorRef(0, ImageLayout::eColorAttachmentOptimal);

    SubpassDescription spDesc({}, PipelineBindPoint::eGraphics,
                              0, nullptr,
                              1, &colorRef);

    SubpassDependency spDep(VK_SUBPASS_EXTERNAL, 0,
                            PipelineStageFlagBits::eColorAttachmentOutput, PipelineStageFlagBits::eColorAttachmentOutput,
                            AccessFlagBits::eNoneKHR, AccessFlagBits::eColorAttachmentWrite);

    RenderPassCreateInfo rpcInfo({},
                                 1, &colorAttachment,
                                 1, &spDesc,
                                 1, &spDep);

    assertResult(this->device->createRenderPass(&rpcInfo, nullptr, &this->renderPass),
                 "Unable to create render pass");
}


ShaderModule SwapQueue::createShaderModule(const std::vector<char> &code)
{
    ShaderModuleCreateInfo createInfo({}, code.size(), reinterpret_cast<const uint32_t*>(code.data()));

    ShaderModule shaderModule;
    assertResult(this->device->createShaderModule(&createInfo, nullptr, &shaderModule),
                 "Unable to create shader module");

    return shaderModule;
}


void SwapQueue::createGraphicsPipeline()
{
    auto vertShader = readFile("Resources/Shaders/Vulkan/TriangleV.spv");
    auto fragShader = readFile("Resources/Shaders/Vulkan/TriangleF.spv");

    auto vModule = this->createShaderModule(vertShader);
    auto fModule = this->createShaderModule(fragShader);

    PipelineShaderStageCreateInfo vsInfo({}, ShaderStageFlagBits::eVertex, vModule, "main");
    PipelineShaderStageCreateInfo fsInfo({}, ShaderStageFlagBits::eFragment, fModule, "main");

    PipelineShaderStageCreateInfo shaderStages[] = { vsInfo, fsInfo };

    PipelineVertexInputStateCreateInfo vcInfo({}, 0, nullptr, 0, nullptr);
    PipelineInputAssemblyStateCreateInfo asmInfo({}, PrimitiveTopology::eTriangleList, false);

    Viewport vp(0, 0, this->scExtend.width, this->scExtend.height, 0, 1);

    Rect2D scissor({0, 0}, this->scExtend);

    PipelineViewportStateCreateInfo vpInfo({}, 1, &vp, 1, &scissor);

    PipelineRasterizationStateCreateInfo rastInfo({}, false, false,
                                                  PolygonMode::eFill,
                                                  CullModeFlagBits::eBack, FrontFace::eClockwise,
                                                  false, 0, 0, 0,
                                                  1);

    PipelineMultisampleStateCreateInfo msInfo;

    PipelineColorBlendStateCreateInfo cbInfo({}, false);

    PipelineLayoutCreateInfo layoutInfo;
    this->pipelineLayout = this->device->createPipelineLayout(layoutInfo);

    GraphicsPipelineCreateInfo creationInfo({}, 2, shaderStages, &vcInfo, &asmInfo,  nullptr,
                                            &vpInfo, &rastInfo, &msInfo, nullptr, &cbInfo, nullptr,
                                            this->pipelineLayout, this->renderPass, 0,
                                            {}, -1);

    const auto resultValue = this->device->createGraphicsPipeline({}, creationInfo);
    assertResult(resultValue.result, "Unable to create graphics pipeline");
    this->graphicsPipeline = resultValue.value;
}


void SwapQueue::createFramebuffers()
{
    this->scFramebuffers.resize(this->scImageViews.size());

    for (size_t i = 0; i < this->scImageViews.size(); i++)
    {
        ImageView attachments[] = { this->scImageViews[i] };

        FramebufferCreateInfo framebufferInfo( {}, this->renderPass, 1, attachments,
                                               this->scExtend.width, this->scExtend.height, 1 );

        this->scFramebuffers[i] = this->device->createFramebuffer(framebufferInfo);
    }
}


void SwapQueue::createCommandBuffers()
{
    CommandBufferAllocateInfo allocInfo(*this->commandPool, CommandBufferLevel::ePrimary,
                                        this->scFramebuffers.size());

    this->commandBuffers = this->device->allocateCommandBuffers(allocInfo);

    for (auto i = 0; i < this->commandBuffers.size(); i++)
    {
        CommandBufferBeginInfo beginInfo;

        this->commandBuffers[i].begin(beginInfo);

        std::array<float, 4> ccArray = { 0, 0, 0, 1};
        ClearValue clearColor(ccArray);

        RenderPassBeginInfo rpInfo(this->renderPass, this->scFramebuffers[i],
                                   { {0, 0}, this->scExtend },
                                   1, &clearColor);

        this->commandBuffers[i].beginRenderPass(rpInfo, SubpassContents::eInline);
        this->commandBuffers[i].bindPipeline(PipelineBindPoint::eGraphics, this->graphicsPipeline);
        this->commandBuffers[i].draw(3, 1, 0, 0);
        this->commandBuffers[i].endRenderPass();

        this->commandBuffers[i].end();
    }
}
