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

class MTK_view_delegate : public MTK::ViewDelegate
{
public:
    MTK_view_delegate(MTL::Device* device);
    virtual ~MTK_view_delegate() override;
    virtual void drawInMTKView(MTK::View* view) override;

private:
    mtk_renderer* renderer;
};

class app_delegate : public NS::ApplicationDelegate
{
public:
    ~app_delegate();

    NS::Menu* createMenuBar();

    virtual void applicationWillFinishLaunching(NS::Notification* notification) override;
    virtual void applicationDidFinishLaunching(NS::Notification* notification) override;
    virtual bool applicationShouldTerminateAfterLastWindowClosed(NS::Application* sender) override;

private:
    NS::Window* window;
    MTK::View* mtk_view;
    MTL::Device* device;
    MTK_view_delegate* view_delegate = nullptr;
};

} // namespace soleil
