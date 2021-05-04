/**
 * @file
 * @author Max Godefroy
 * @date 07/05/2021.
 */

#pragma once


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include "SwapChain.hpp"


class HelloTriangleApp {

public:

    void run();

private:

    void initVulkan();

    void mainLoop();

    void cleanup();

private:

    void createInstance();

    void setupDebugMessenger();

    void pickPhysicalDevice();

    void initLogicalDevice();

    void createSurface();

    void createCommandPool();

    void createSyncObjects();

private:

    void drawFrame();

    void resetSwapChain();

private:

    static bool checkValidationLayerSupport();

    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

private:

    GLFWwindow *window = nullptr;

    VkInstance instance;

    VkDebugUtilsMessengerEXT debugMessenger;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    VkDevice device;
    Device device_hpp;

    VkSurfaceKHR surface;

    VkQueue graphicsQueue;

    VkQueue presentQueue;

    VkCommandPool commandPool;
    CommandPool commandPool_hpp;

    std::unique_ptr<SwapChain> swapChain;

    std::vector<Semaphore> imageAvailableSemaphores;

    std::vector<Semaphore> renderFinishedSemaphores;

    std::vector<Fence> inFlightFences;

    uint64_t currentFrame = 0;

    bool framebufferResized = false;

private:

    const uint64_t MAX_FRAMES_IN_FLIGHT = 2;

};


