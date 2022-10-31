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

private:
    int width = 100;
    int height = 100;
    
    MTL::Device* device;
    dispatch_semaphore_t in_flight_semaphore;
    MTL::TextureDescriptor* output_desc;
    MTL::Texture* texture;
    
    MTL::RenderPipelineDescriptor* build_shaders();
    MTL::Library* library;
};

} // namespace soleil
