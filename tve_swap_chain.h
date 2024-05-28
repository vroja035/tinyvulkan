/*

    Swap Chain: Series of framebuffers used to display images to our window
        - Uses at least two buffers at once or double buffering 
          (font -> displayed, back -> rendered to)
        - Coordinates with display to swap buffers prior to V-sync
            - V-sync -> moment at which displays begin drawing new image -> tied to refresh rate
            - eg. 60hz, 60 times per second V-sync occurs and buffers are swpaped
            - No V-sync -> tearing -> shows info from multiple frames in a single screen draw
        - Double vs Triple buffering
            - Double buffering
                - GPU idles until next Vsync and frame buffer swap -> fixed with an extra buffer
                  (triple buffering) but utilizes more memory
            - Triple buffering
                - Always provides GPU with a frame buffer to work on
            - GSYNC tackles this by changing behavior and not making display wait entire period

    Present Modes Comparison
    - FIFO
        + VSynch bound
        + Good for mobile
        + Always supported
        - Latency

    - Mailbox
        + Low latency
        - Not always supported
        - High power consumption since GPU doesn't idle

    - Immediate
         + Low Latency
         + Usually supported
         - High power consumption
         - Tearing

    RenderPass
        - Describes structure and format of frame buffer obj and attachments
*/

#pragma once

#include "tve_device.h"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <memory>
#include <string>
#include <vector>

namespace tve {

    class TveSwapChain {
    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        TveSwapChain(TveDevice& deviceRef, VkExtent2D windowExtent);
        TveSwapChain(TveDevice& deviceRef, VkExtent2D windowExtent, std::shared_ptr<TveSwapChain> previous);
        ~TveSwapChain();

        TveSwapChain(const TveSwapChain&) = delete;
        TveSwapChain& operator=(const TveSwapChain&) = delete;

        VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
        VkRenderPass getRenderPass() { return renderPass; }
        VkImageView getImageView(int index) { return swapChainImageViews[index]; }
        size_t imageCount() { return swapChainImages.size(); }
        VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
        VkExtent2D getSwapChainExtent() { return swapChainExtent; }
        uint32_t width() { return swapChainExtent.width; }
        uint32_t height() { return swapChainExtent.height; }

        float extentAspectRatio() {
            return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
        }
        VkFormat findDepthFormat();

        VkResult acquireNextImage(uint32_t* imageIndex);
        VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

    private:
        void init();
        void createSwapChain();
        void createImageViews();
        void createDepthResources();
        void createRenderPass(); // describes structure and format of framebuffer obj + attachments
        void createFramebuffers();
        void createSyncObjects();

        // Helper functions
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(
            const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(
            const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;

        std::vector<VkFramebuffer> swapChainFramebuffers;
        VkRenderPass renderPass;

        std::vector<VkImage> depthImages;
        std::vector<VkDeviceMemory> depthImageMemorys;
        std::vector<VkImageView> depthImageViews;
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;

        TveDevice& device;
        VkExtent2D windowExtent;

        VkSwapchainKHR swapChain;
        std::shared_ptr<TveSwapChain> oldSwapChain;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        size_t currentFrame = 0;
    };

}  // namespace tve
