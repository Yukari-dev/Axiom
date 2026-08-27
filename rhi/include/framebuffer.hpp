#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Axiom{

class Framebuffer{
public:
  Framebuffer(VkDevice device, const std::vector<VkImageView>& imageViews, const VkExtent2D& extent, VkRenderPass renderPass);
  ~Framebuffer();
private:
  void Create(const std::vector<VkImageView>& imageViews, const VkExtent2D& extent, VkRenderPass renderPass);
private:
  VkDevice m_device;
  std::vector<VkFramebuffer> m_framebuffers;
};

}
