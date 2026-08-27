#include "framebuffer.hpp"
#include <stdexcept>

namespace Axiom{

Framebuffer::Framebuffer(
  VkDevice device, 
  const std::vector<VkImageView>& imageViews, 
  const VkExtent2D& extent, 
  VkRenderPass renderPass
) : m_device(device){
  Create(imageViews, extent, renderPass);
}

Framebuffer::~Framebuffer(){
  for(auto framebuffer : m_framebuffers)
    vkDestroyFramebuffer(m_device, framebuffer, nullptr);
}

void Framebuffer::Create(
  const std::vector<VkImageView>& imageViews, 
  const VkExtent2D& extent, 
  VkRenderPass renderPass
){
  m_framebuffers.resize(imageViews.size());

  for(size_t i = 0; i < imageViews.size(); i++){
    VkImageView attachments[] = {imageViews[i]};

    VkFramebufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.renderPass = renderPass;
    info.attachmentCount = 1;
    info.pAttachments = attachments;
    info.width = extent.width;
    info.height = extent.height;
    info.layers = 1;

    VkResult result = vkCreateFramebuffer(m_device, &info, nullptr, &m_framebuffers[i]);
    if(result != VK_SUCCESS)
      throw std::runtime_error("Failed to create Framebuffer");
  }
}

}
