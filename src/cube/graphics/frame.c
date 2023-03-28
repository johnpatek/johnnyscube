#include "cube.h"

static int graphics_create_swapchain(cube_graphics *graphics);
static int graphics_create_descriptor_pool(cube_graphics *graphics);
static int graphics_create_frame(cube_graphics *graphics, VkImage *image, uint32_t index, cube_frame *frame);
static int graphics_create_descriptor_sets(cube_graphics *graphics);
static int graphics_create_sync_objects(cube_graphics *graphics);
static int graphics_render_update_object(cube_graphics *graphics, cube_frame *frame);
static int graphics_render_prepare_frame(cube_graphics *graphics, cube_frame *frame);
static void graphics_destroy_frame(cube_graphics *graphics, cube_frame *frame);

int graphics_create_frame_pool(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    uint32_t swapchain_image_count;
    VkImage *swapchain_images;
    uint32_t frame_index;

    CUBE_ASSERT(
        graphics_create_swapchain(graphics) == CUBE_SUCCESS,
        "failed to create swapchain")

    VK_CHECK_RESULT(
        vkGetSwapchainImagesKHR(
            graphics->logical_device,
            graphics->swapchain,
            &swapchain_image_count,
            NULL))

    swapchain_images = CUBE_CALLOC(swapchain_image_count, sizeof(VkImage));
    CUBE_ASSERT(swapchain_images != NULL, "failed to allocate swapchain images")

    VK_CHECK_RESULT(
        vkGetSwapchainImagesKHR(
            graphics->logical_device,
            graphics->swapchain,
            &swapchain_image_count,
            swapchain_images))

    graphics->frame_count = swapchain_image_count;
    graphics->frames = calloc(graphics->frame_count, sizeof(cube_frame));
    CUBE_ASSERT(graphics->frames != NULL, "failed to allocate frames")

    CUBE_ASSERT(
        graphics_create_descriptor_pool(graphics) == CUBE_SUCCESS,
        "failed to create descriptor pool")

    for (frame_index = 0; frame_index < swapchain_image_count; frame_index++)
    {
        CUBE_ASSERT(
            graphics_create_frame(
                graphics,
                swapchain_images,
                frame_index,
                (graphics->frames + frame_index)) == CUBE_SUCCESS,
            "failed to create frame")
    }

    CUBE_ASSERT(
        graphics_create_descriptor_sets(graphics) == CUBE_SUCCESS,
        "failed to create descriptor set")

    CUBE_ASSERT(
        graphics_create_sync_objects(
            graphics) == CUBE_SUCCESS,
        "failed to create sync objects")

    CUBE_END_FUNCTION
}

int graphics_create_sync_objects(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    const VkSemaphoreCreateInfo semaphore_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };
    const VkFenceCreateInfo fence_create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    VK_CHECK_RESULT(
        vkCreateSemaphore(
            graphics->logical_device,
            &semaphore_create_info,
            NULL,
            &graphics->frame_rendered))
    VK_CHECK_RESULT(
        vkCreateSemaphore(
            graphics->logical_device,
            &semaphore_create_info,
            NULL,
            &graphics->frame_presented))
    VK_CHECK_RESULT(
        vkCreateFence(
            graphics->logical_device,
            &fence_create_info,
            NULL,
            &graphics->command_fence))
    CUBE_END_FUNCTION
}

int graphics_render_acquire_frame(cube_graphics *graphics, cube_frame **frame)
{
    CUBE_BEGIN_FUNCTION
    uint32_t frame_index;

    VK_CHECK_RESULT(
        vkWaitForFences(
            graphics->logical_device,
            1,
            &graphics->command_fence,
            VK_TRUE,
            UINT64_MAX))

    VK_CHECK_RESULT(
        vkResetFences(
            graphics->logical_device,
            1,
            &graphics->command_fence))

    VK_CHECK_RESULT(
        vkAcquireNextImageKHR(
            graphics->logical_device,
            graphics->swapchain,
            UINT64_MAX,
            graphics->frame_presented,
            VK_NULL_HANDLE,
            &frame_index))

    *frame = (graphics->frames + frame_index);

    CUBE_END_FUNCTION
}

int graphics_render_draw_frame(cube_graphics *graphics, cube_frame *frame)
{
    CUBE_BEGIN_FUNCTION
    CUBE_ASSERT(
        graphics_render_update_object(graphics, frame) == CUBE_SUCCESS,
        "failed to update object")
    CUBE_ASSERT(
        graphics_render_prepare_frame(
            graphics,
            frame) == CUBE_SUCCESS,
        "failed to prepare frame")
    VkDeviceSize vertex_buffer_offsets[] = {0};
    vkCmdBindVertexBuffers(
        frame->command_buffer,
        0,
        1,
        &graphics->object->vertex_buffer,
        &vertex_buffer_offsets[0]);
    vkCmdBindIndexBuffer(
        frame->command_buffer,
        graphics->object->index_buffer,
        0,
        VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(
        frame->command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        graphics->pipeline_layout,
        0, 1,
        &frame->descriptor_set,
        0, NULL);
    vkCmdDrawIndexed(
        frame->command_buffer,
        graphics->object->index_count,
        1, 0, 0, 0);
    vkCmdEndRenderPass(frame->command_buffer);
    VK_CHECK_RESULT(vkEndCommandBuffer(frame->command_buffer))
    CUBE_END_FUNCTION
}

int graphics_render_submit_frame(cube_graphics *graphics, cube_frame *frame)
{
    CUBE_BEGIN_FUNCTION
    const VkPipelineStageFlags wait_dest_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    const VkSubmitInfo frame_submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &graphics->frame_presented,
        .pWaitDstStageMask = &wait_dest_stage_mask,
        .commandBufferCount = 1,
        .pCommandBuffers = &frame->command_buffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &graphics->frame_rendered,
    };
    const VkPresentInfoKHR frame_present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &graphics->frame_rendered,
        .swapchainCount = 1,
        .pSwapchains = &graphics->swapchain,
        .pImageIndices = &frame->index,
    };
    VK_CHECK_RESULT(
        vkQueueSubmit(
            graphics->graphics_queue,
            1,
            &frame_submit_info,
            graphics->command_fence))
    VK_CHECK_RESULT(
        vkQueuePresentKHR(
            graphics->present_queue,
            &frame_present_info))
    VK_CHECK_RESULT(
        vkQueueWaitIdle(
            graphics->present_queue))
    CUBE_END_FUNCTION
}

int graphics_render_update_object(cube_graphics *graphics, cube_frame *frame)
{
    CUBE_BEGIN_FUNCTION
    const float base_model[4][4] = {
        {1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f},
    };
    const clock_t timestamp = clock();
    const float delta = (float)(timestamp - graphics->timestamp) / CLOCKS_PER_SEC;
    const float angle = (1 * 3.14f) / 2.0f;
    const float axis[] = {0.0f, 0.0f, 1.0f};

    const float eye[] = {2.0f, 2.0f, 2.0f};
    const float center[] = {0.0f, 0.0f, 0.0f};
    const float up[] = {0.0f, 0.0f, 1.0f};

    const float fov = (45.0f * 3.14f) / 180.0f;
    const float aspect = (float)graphics->display_size.width / (float)graphics->display_size.height;
    const float znear = 0.1f;
    const float zfar = 10.0f;

    cube_ubo updated_ubo;

    CUBE_ASSERT(
        graphics_util_rotate(
            base_model,
            angle,
            axis,
            &updated_ubo) == CUBE_SUCCESS,
        "failed to rotate object")
    CUBE_ASSERT(
        graphics_util_look_at(
            eye,
            center,
            up,
            &updated_ubo) == CUBE_SUCCESS,
        "failed to calculate view")
    CUBE_ASSERT(
        graphics_util_perspective(
            fov,
            aspect,
            znear,
            zfar,
            &updated_ubo) == CUBE_SUCCESS,
        "failed to calculate perspective")

    SDL_memcpy(frame->uniform_buffer_mapping, &updated_ubo, sizeof(cube_ubo));
    CUBE_END_FUNCTION
}

int graphics_render_prepare_frame(cube_graphics *graphics, cube_frame *frame)
{
    CUBE_BEGIN_FUNCTION
    const VkCommandBufferResetFlags reset_flags = 0;
    const VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    const VkCommandBufferBeginInfo command_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
    };
    const VkRenderPassBeginInfo render_pass_begin_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .clearValueCount = 1,
        .pClearValues = &clear_color,
        .framebuffer = frame->framebuffer,
        .renderPass = graphics->render_pass,
        .renderArea = {
            .extent = graphics->display_size,
            .offset = {0, 0},
        },
    };
    const VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float)graphics->display_size.width,
        .height = (float)graphics->display_size.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    const VkRect2D scissor = {
        .offset = {0, 0},
        .extent = graphics->display_size,
    };
    VK_CHECK_RESULT(
        vkResetCommandBuffer(
            frame->command_buffer,
            reset_flags))
    VK_CHECK_RESULT(
        vkBeginCommandBuffer(
            frame->command_buffer,
            &command_buffer_begin_info))
    vkCmdBeginRenderPass(
        frame->command_buffer,
        &render_pass_begin_info,
        VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(
        frame->command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        graphics->graphics_pipeline);
    vkCmdSetViewport(frame->command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(frame->command_buffer, 0, 1, &scissor);
    CUBE_END_FUNCTION
}

void graphics_destroy_frame_pool(cube_graphics *graphics)
{
    uint32_t frame_index;
    for (frame_index = 0; frame_index < graphics->frame_count; frame_index++)
    {
        graphics_destroy_frame(graphics, graphics->frames + frame_index);
    }
    free(graphics->frames);
    if (graphics->frame_rendered != VK_NULL_HANDLE)
    {
        vkDestroySemaphore(graphics->logical_device, graphics->frame_rendered, NULL);
    }
    if (graphics->frame_presented != VK_NULL_HANDLE)
    {
        vkDestroySemaphore(graphics->logical_device, graphics->frame_presented, NULL);
    }
    if (graphics->command_fence != VK_NULL_HANDLE)
    {
        vkDestroyFence(graphics->logical_device, graphics->command_fence, NULL);
    }
    if (graphics->swapchain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(graphics->logical_device, graphics->swapchain, NULL);
    }
}

static int graphics_create_frame(cube_graphics *graphics, VkImage *images, uint32_t index, cube_frame *frame)
{
    CUBE_BEGIN_FUNCTION
    const VkImageViewCreateInfo image_view_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = graphics->surface_format.format,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
        .image = *(images + index),
    };
    const VkFramebufferCreateInfo framebuffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = graphics->render_pass,
        .attachmentCount = 1,
        .pAttachments = &frame->image_view,
        .width = graphics->display_size.width,
        .height = graphics->display_size.height,
        .layers = 1,
    };
    const VkCommandBufferAllocateInfo command_buffer_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = graphics->command_pool,
        .commandBufferCount = 1,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    };
    const VkBufferCreateInfo uniform_buffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(cube_ubo),
        .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };
    const VmaAllocationCreateInfo host_allocation_create_info = {
        .usage = VMA_MEMORY_USAGE_CPU_ONLY,
        .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT,
    };
    VmaAllocationInfo uniform_buffer_allocation_info;
    frame->index = index;

    VK_CHECK_RESULT(
        vkCreateImageView(
            graphics->logical_device,
            &image_view_create_info,
            NULL,
            &frame->image_view))
    VK_CHECK_RESULT(
        vkCreateFramebuffer(
            graphics->logical_device,
            &framebuffer_create_info,
            NULL,
            &frame->framebuffer))
    VK_CHECK_RESULT(
        vkAllocateCommandBuffers(
            graphics->logical_device,
            &command_buffer_allocate_info,
            &frame->command_buffer))
    VK_CHECK_RESULT(
        vmaCreateBuffer(
            graphics->allocator,
            &uniform_buffer_create_info,
            &host_allocation_create_info,
            &frame->uniform_buffer,
            &frame->uniform_buffer_allocation,
            &uniform_buffer_allocation_info))
    frame->uniform_buffer_mapping = uniform_buffer_allocation_info.pMappedData;
    CUBE_END_FUNCTION
}

int graphics_create_swapchain(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    const uint32_t queue_family_indices[] = {
        graphics->graphics_queue_family_index,
        graphics->present_queue_family_index,
    };
    VkSwapchainCreateInfoKHR swapchain_create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = graphics->surface,
        .minImageCount = CLAMP(
            2,
            graphics->surface_capabilities.minImageCount,
            graphics->surface_capabilities.maxImageCount),
        .imageFormat = graphics->surface_format.format,
        .imageColorSpace = graphics->surface_format.colorSpace,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .imageExtent = {
            .width = graphics->display_size.width,
            .height = graphics->display_size.height,
        },
        .preTransform = graphics->surface_capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .clipped = VK_TRUE,
    };

    if (graphics->graphics_queue_family_index != graphics->present_queue_family_index)
    {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = sizeof(queue_family_indices) / sizeof(queue_family_indices[0]);
        swapchain_create_info.pQueueFamilyIndices = &queue_family_indices[0];
    }

    VK_CHECK_RESULT(
        vkCreateSwapchainKHR(
            graphics->logical_device,
            &swapchain_create_info,
            NULL,
            &graphics->swapchain))
    CUBE_END_FUNCTION
}

int graphics_create_descriptor_pool(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    VkDescriptorPoolSize descriptor_pool_size = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = graphics->frame_count,
    };
    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = 1,
        .pPoolSizes = &descriptor_pool_size,
        .maxSets = graphics->frame_count,
    };
    VK_CHECK_RESULT(
        vkCreateDescriptorPool(
            graphics->logical_device,
            &descriptor_pool_create_info,
            NULL,
            &graphics->descriptor_pool))
    CUBE_END_FUNCTION
}

int graphics_create_descriptor_sets(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = graphics->descriptor_pool,
        .descriptorSetCount = graphics->frame_count,
    };
    VkDescriptorBufferInfo descriptor_buffer_info = {
        .offset = 0,
        .range = sizeof(cube_ubo),
    };
    VkWriteDescriptorSet write_descriptor_set = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .pBufferInfo = &descriptor_buffer_info,
        .dstBinding = 0,
        .dstArrayElement = 0,
    };
    VkDescriptorSetLayout *descriptor_set_layouts;
    VkDescriptorSet *descriptor_sets;
    uint32_t index;

    descriptor_set_layouts = CUBE_CALLOC(graphics->frame_count, sizeof(VkDescriptorSetLayout));
    CUBE_ASSERT(descriptor_set_layouts != NULL, "failed to allocate descriptor set layouts")
    for (index = 0; index < graphics->frame_count; index++)
    {
        *(descriptor_set_layouts + index) = graphics->descriptor_set_layout;
    }

    descriptor_sets = CUBE_CALLOC(graphics->frame_count, sizeof(VkDescriptorSet));
    CUBE_ASSERT(descriptor_sets != NULL, "failed to allocate descriptor sets")

    descriptor_set_allocate_info.pSetLayouts = descriptor_set_layouts;
    VK_CHECK_RESULT(vkAllocateDescriptorSets(graphics->logical_device, &descriptor_set_allocate_info, descriptor_sets));

    for (index = 0; index < graphics->frame_count; index++)
    {
        descriptor_buffer_info.buffer = (graphics->frames + index)->uniform_buffer;
        write_descriptor_set.dstSet = *(descriptor_sets + index);
        vkUpdateDescriptorSets(graphics->logical_device, 1, &write_descriptor_set, 0, NULL);
        (graphics->frames + index)->descriptor_set = *(descriptor_sets + index);
    }

    CUBE_END_FUNCTION
}

void graphics_destroy_frame(cube_graphics *graphics, cube_frame *frame)
{
    if (frame != NULL)
    {
        if (frame->uniform_buffer != VK_NULL_HANDLE)
        {
            vmaDestroyBuffer(graphics->allocator, frame->uniform_buffer, frame->uniform_buffer_allocation);
        }
        if (frame->framebuffer != VK_NULL_HANDLE)
        {
            vkDestroyFramebuffer(graphics->logical_device, frame->framebuffer, NULL);
        }
        if (frame->image_view != VK_NULL_HANDLE)
        {
            vkDestroyImageView(graphics->logical_device, frame->image_view, NULL);
        }
    }
}