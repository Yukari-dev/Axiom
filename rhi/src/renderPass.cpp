#include "renderPass.hpp"
#include <stdexcept>

namespace Axiom{

RenderPass::RenderPass(VkDevice device, VkFormat swapChainFormat) : m_device(device){
  Create(swapChainFormat);
}

RenderPass::~RenderPass(){
  vkDestroyRenderPass(m_device, m_renderPass, nullptr);
}

void RenderPass::Create(VkFormat swapChainFormat){
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = swapChainFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  
  VkAttachmentReference colorAttachRef{};
  colorAttachRef.attachment = 0;
  colorAttachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachRef;

  VkRenderPassCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  info.attachmentCount = 1;
  info.pAttachments = &colorAttachment;
  info.subpassCount = 1;
  info.pSubpasses = &subpass;

  VkResult result = vkCreateRenderPass(m_device, &info, nullptr, &m_renderPass);
  if(result != VK_SUCCESS)
    throw std::runtime_error("Failed to create SubPass");
}

}
