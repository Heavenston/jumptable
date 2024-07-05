#include "saltus/vulkan/vulkan_bind_group.hh"
#include <memory>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include "saltus/vulkan/vulkan_buffer.hh"

namespace saltus::vulkan
{
    VulkanBindGroup::VulkanBindGroup(std::shared_ptr<VulkanDevice> device, BindGroupCreateInfo info):
        BindGroup(info), device_(device)
    {
        auto bind_group_layout = 
            std::dynamic_pointer_cast<VulkanBindGroupLayout>(info.layout);
        bind_group_layout_ = bind_group_layout;

        std::vector<VkDescriptorPoolSize> sizes;
        for (const auto &binding : info.layout->bindings())
        {
            VkDescriptorPoolSize size{};
            size.descriptorCount = binding.count;
            size.type = binding_type_to_descriptor_type(binding.type);
            sizes.push_back(size);
        }

        VkDescriptorPoolCreateInfo pool_info{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
        pool_info.poolSizeCount = sizes.size();
        pool_info.pPoolSizes = sizes.data();
        pool_info.maxSets = 1;

        VkResult result =
            vkCreateDescriptorPool(*device_, &pool_info, nullptr, &descriptor_pool_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Could not create descriptor pool");
        
        VkDescriptorSetAllocateInfo alloc_info{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
        alloc_info.descriptorPool = descriptor_pool_;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &bind_group_layout_->layout();

        result =
            vkAllocateDescriptorSets(*device_, &alloc_info, &descriptor_set_);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate descriptor set");
    }

    VulkanBindGroup::~VulkanBindGroup()
    {
        if (descriptor_pool_ != VK_NULL_HANDLE)
            vkDestroyDescriptorPool(*device_, descriptor_pool_, nullptr);
    }

    const std::shared_ptr<VulkanDevice> &VulkanBindGroup::device() const
    {
        return device_;
    }

    const std::shared_ptr<VulkanBindGroupLayout> &VulkanBindGroup::bind_group_layout() const
    {
        return bind_group_layout_;
    }

    const VkDescriptorSet &VulkanBindGroup::descriptor_set() const
    {
        return descriptor_set_;
    }

    void VulkanBindGroup::set_binding(
        uint32_t binding_id,
        const std::shared_ptr<Buffer> &buffer,
        uint32_t array_index,
        uint64_t offset,
        std::optional<uint64_t> size
    ) {
        auto *bind_group = layout()->get_binding(binding_id);
        if (!bind_group)
        {
            throw std::runtime_error("The provided binding id isn't in the layout");
        }

        if (
            bind_group->type != BindingType::StorageBuffer &&
            bind_group->type != BindingType::UniformBuffer
        ) {
            throw std::runtime_error("Cannot set a binding with a buffer if the binding isn't a buffer type");
        }

        auto vkBuffer = std::dynamic_pointer_cast<VulkanBuffer>(buffer);

        VkDescriptorBufferInfo buff_info {
            .buffer = vkBuffer->buffer(),
            .offset = offset,
            .range = size.value_or(VK_WHOLE_SIZE),
        };

        VkWriteDescriptorSet write{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        write.dstSet = descriptor_set_;
        write.dstBinding = binding_id;
        write.dstArrayElement = array_index;
        write.descriptorType = binding_type_to_descriptor_type(bind_group->type);
        write.descriptorCount = 1;
        write.pBufferInfo = &buff_info;

        vkUpdateDescriptorSets(*device_, 1, &write, 0, nullptr);
    }
} // namespace saltus::vulkan
