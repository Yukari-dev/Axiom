#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>

namespace Axiom{

class Framebuffer{
public:
  Framebuffer(VkDevice device, const std::vector<VkImageView>& imageViews, const VkExtent2D& extent, VkRenderPass renderPass);
  ~Framebuffer();

  VkFramebuffer GetFramebuffer(size_t index) const { return m_framebuffers[index]; }
private:
  void Create(const std::vector<VkImageView>& imageViews, const VkExtent2D& extent, VkRenderPass renderPass);
private:
  VkDevice m_device;
  std::vector<VkFramebuffer> m_framebuffers;
};

}
