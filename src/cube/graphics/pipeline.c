#include "cube.h"

static int graphics_create_surface_properties(cube_graphics *graphics);
static int graphics_create_render_pass(cube_graphics *graphics);
static int graphics_create_graphics_pipeline(cube_graphics *graphics);

int graphics_create_pipeline(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    CUBE_ASSERT(
        graphics_create_surface_properties(graphics) == CUBE_SUCCESS,
        "failed to create surface properties")
    CUBE_ASSERT(
        graphics_create_render_pass(graphics) == CUBE_SUCCESS,
        "failed to create render pass")
    CUBE_ASSERT(
        graphics_create_graphics_pipeline(graphics) == CUBE_SUCCESS,
        "failed to create render pass")
    CUBE_END_FUNCTION
}

void graphics_destroy_pipeline(cube_graphics *graphics)
{
    if (graphics->graphics_pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(graphics->logical_device, graphics->graphics_pipeline, NULL);
    }
    if (graphics->render_pass != VK_NULL_HANDLE)
    {
        vkDestroyRenderPass(graphics->logical_device, graphics->render_pass, NULL);
    }
}

int graphics_create_surface_properties(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    uint32_t surface_format_count;
    VkSurfaceFormatKHR *surface_formats;
    uint32_t surface_format_index;
    VkBool32 found_format;

    VK_CHECK_RESULT(
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            graphics->physical_device,
            graphics->surface,
            &graphics->surface_capabilities))

    graphics->display_size.width = CLAMP(
        graphics->display_size.width,
        graphics->surface_capabilities.minImageExtent.width,
        graphics->surface_capabilities.maxImageExtent.width);

    graphics->display_size.height = CLAMP(
        graphics->display_size.height,
        graphics->surface_capabilities.minImageExtent.height,
        graphics->surface_capabilities.maxImageExtent.height);

    VK_CHECK_RESULT(
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            graphics->physical_device,
            graphics->surface,
            &surface_format_count,
            NULL))

    surface_formats = CUBE_CALLOC(surface_format_count, sizeof(VkSurfaceFormatKHR));

    VK_CHECK_RESULT(
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            graphics->physical_device,
            graphics->surface,
            &surface_format_count,
            surface_formats))

    found_format = VK_FALSE;
    for (surface_format_index = 0; surface_format_index < surface_format_count; surface_format_index++)
    {
        if (found_format == VK_FALSE)
        {
            graphics->surface_format = *(surface_formats + surface_format_index);
            if (graphics->surface_format.format == VK_FORMAT_B8G8R8A8_UNORM)
            {
                found_format = VK_TRUE;
            }
        }
    }
    CUBE_ASSERT(found_format == VK_TRUE, "failed to find surface format")
    CUBE_END_FUNCTION
}

int graphics_create_render_pass(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    const VkAttachmentDescription attachments[] = {
        // color attachment
        {
            .format = graphics->surface_format.format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        }, // TODO: add depth attachment
    };
    const VkAttachmentReference color_reference = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    const VkSubpassDescription subpass_description = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_reference,
        .pDepthStencilAttachment = NULL,
        .inputAttachmentCount = 0,
        .pInputAttachments = NULL,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = NULL,
        .pResolveAttachments = NULL,
    };
    const VkSubpassDependency subpass_dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = VK_ACCESS_MEMORY_READ_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
    };
    const VkRenderPassCreateInfo render_pass_create_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &attachments[0],
        .subpassCount = 1,
        .pSubpasses = &subpass_description,
        .dependencyCount = 1,
        .pDependencies = &subpass_dependency,
    };
    VK_CHECK_RESULT(vkCreateRenderPass(
        graphics->logical_device,
        &render_pass_create_info,
        NULL,
        &graphics->render_pass))
    CUBE_END_FUNCTION
}

int graphics_create_graphics_pipeline(cube_graphics *graphics)
{
    CUBE_BEGIN_FUNCTION
    VkShaderModule vertex_shader;
    VkShaderModule fragment_shader;

    VkVertexInputBindingDescription vertex_input_binding_descritpion = {
        .binding = 0,
        .stride = sizeof(cube_vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };
    VkVertexInputAttributeDescription vertex_input_attribute_descritpions[] = {
        {
            .binding = 0,
            .location = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(cube_vertex, position),
        },
        {
            .binding = 0,
            .location = 1,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(cube_vertex, color),
        },
    };
    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &vertex_input_binding_descritpion,
        .vertexAttributeDescriptionCount = 2,
        .pVertexAttributeDescriptions = &vertex_input_attribute_descritpions[0],
    };
    VkPipelineShaderStageCreateInfo shader_stages[] = {
        // vertex shader
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .pName = "main",
        },
        // fragment shader
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pName = "main",
        },
    };
    VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };
    VkPipelineViewportStateCreateInfo viewport_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1,
    };
    VkPipelineRasterizationStateCreateInfo rasterization_state_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .lineWidth = 1.0f,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
    };
    VkPipelineMultisampleStateCreateInfo multisample_state_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = VK_FALSE,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    };
    VkPipelineColorBlendAttachmentState color_blend_attachment = {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        .blendEnable = VK_FALSE,
    };
    VkPipelineColorBlendStateCreateInfo color_blend_state_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &color_blend_attachment,
        .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f},
    };
    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };
    VkPipelineDynamicStateCreateInfo dynamic_state_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = 2,
        .pDynamicStates = &dynamic_states[0],
    };
    VkDescriptorSetLayoutBinding descriptor_set_layout_binding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    };
    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &descriptor_set_layout_binding,
    };
    VkPipelineLayoutCreateInfo pipeline_layout_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
        .setLayoutCount = 1,
        .pSetLayouts = &graphics->descriptor_set_layout,
    };
    VkGraphicsPipelineCreateInfo pipeline_create_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = &shader_stages[0],
        .pVertexInputState = &vertex_input_info,
        .pInputAssemblyState = &input_assembly_info,
        .pViewportState = &viewport_state,
        .pRasterizationState = &rasterization_state_info,
        .pMultisampleState = &multisample_state_info,
        .pColorBlendState = &color_blend_state_info,
        .pDynamicState = &dynamic_state_info,
        .renderPass = graphics->render_pass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
    };

    CUBE_ASSERT(
        graphics_util_load_shader(
            graphics, "vert.spv",
            &vertex_shader) == CUBE_SUCCESS,
        "failed to load vertex shader")

    shader_stages[0].module = vertex_shader;

    CUBE_ASSERT(
        graphics_util_load_shader(
            graphics, "frag.spv",
            &fragment_shader) == CUBE_SUCCESS,
        "failed to load fragment shader")

    shader_stages[1].module = fragment_shader;

    VK_CHECK_RESULT(
        vkCreateDescriptorSetLayout(
            graphics->logical_device,
            &descriptor_set_layout_create_info,
            NULL,
            &graphics->descriptor_set_layout))

    VK_CHECK_RESULT(
        vkCreatePipelineLayout(
            graphics->logical_device,
            &pipeline_layout_info,
            NULL,
            &graphics->pipeline_layout))

    pipeline_create_info.layout = graphics->pipeline_layout;

    VK_CHECK_RESULT(
        vkCreateGraphicsPipelines(
            graphics->logical_device,
            VK_NULL_HANDLE,
            1,
            &pipeline_create_info,
            NULL,
            &graphics->graphics_pipeline))

    vkDestroyShaderModule(graphics->logical_device, fragment_shader, NULL);
    vkDestroyShaderModule(graphics->logical_device, vertex_shader, NULL);
    CUBE_END_FUNCTION
}