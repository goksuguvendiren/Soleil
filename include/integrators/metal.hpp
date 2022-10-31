//
// Created by goksu on 6/7/19.
//

#pragma once

#include "camera.hpp"
#include "mc_integrator.hpp"

#include "Foundation/Foundation.hpp"
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>

#include <glm/vec3.hpp>

namespace soleil
{
class metal
{
public:
    metal(unsigned int w, unsigned int h, int sq = 1);
    
    std::vector<glm::vec3> render(const soleil::scene& scene);
    glm::vec3 render_pixel(const soleil::scene& scene, int i, int j);

private:
    int width = 100;
    int height = 100;
};

class mtk_renderer
{
public:
    mtk_renderer(MTL::Device* device);
    ~mtk_renderer();
    void build_shaders();
    void build_buffers();
    void draw(MTK::View* pView);

private:
    MTL::Device* device;
    MTL::CommandQueue* command_queue;
    MTL::RenderPipelineState* pipeline_state;
    MTL::Buffer* vertex_pos_buffer;
    MTL::Buffer* vertex_color_buffer;
};


} // namespace soleil
